#include "pty.hpp"
#include <string>                     // Llama a la librería string, ya que se necesitan usar diversos formatos de string a continuación, como wstring y funciones para manipular los strings

#if defined(_WIN32)                   // A continuación, verifica si se compilará para windows, en este caso, compilará el código hasta la línea 138

#ifndef _WIN32_WINNT                  // ConPTY (CreatePseudoConsole / ClosePseudoConsole) requiere Windows 10+
#  define _WIN32_WINNT 0x0A00
#elif _WIN32_WINNT < 0x0A00
#  undef  _WIN32_WINNT
#  define _WIN32_WINNT 0x0A00
#endif

#include <winsock2.h>                 // Llama a la librería winsock2.h
#include <ws2tcpip.h>                 // Llamada a la librería de la comunicación TCP/IP
#pragma comment(lib, "Ws2_32.lib")    // La librería para su compilación la buscará como "Ws2_32.lib"
#include <windows.h>                  // Llamada a la librería para comunicarse con la API de Windows

static std::wstring to_wstring(const std::string& str)      // Función para convertir de string a wstring
{
    if (str.empty()) return L"";      // Si el string de entrada está vacío, devolverá un string en widechar vacío
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);   //
    std::wstring wstr(size_needed, 0);                                                          //
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);       //
    return wstr;                      //
}

std::string ptyfunc(std::string sqlinput,
                    std::string inputuser,
                    std::string inputpass,
                    std::string inputserver,
                    std::string inputport,
                    std::string inputdatabase)
{
    std::string outTerm = "";
    std::wstring sqliwstr = to_wstring(sqlinput);
    std::wstring userwstr = to_wstring(inputuser);
    std::wstring passwstr = to_wstring(inputpass);
    std::wstring srvrwstr = to_wstring(inputserver);
    std::wstring portwstr = to_wstring(inputport);
    std::wstring basewstr = to_wstring(inputdatabase);
    std::wstring cmd = L".\\bin\\mariadb.exe";
    cmd += L" -u"; cmd += userwstr;
    cmd += L" -p"; cmd += passwstr;
    cmd += L" -D "; cmd += basewstr;
    cmd += L" -h "; cmd += srvrwstr;
    cmd += L" -P "; cmd += portwstr;
    cmd += L" -e \""; cmd += sqliwstr;
    cmd += L"\"";

    HANDLE hPipeInRead, hPipeInWrite;
    HANDLE hPipeOutRead, hPipeOutWrite;
    SECURITY_ATTRIBUTES sa{ sizeof(sa), NULL, TRUE };
    CreatePipe(&hPipeInRead, &hPipeInWrite, &sa, 0);
    CreatePipe(&hPipeOutRead, &hPipeOutWrite, &sa, 0);

    HPCON hPC;
    COORD size = { 120, 30 };
    CreatePseudoConsole(size, hPipeInRead, hPipeOutWrite, 0, &hPC);

    STARTUPINFOEXW si{};
    si.StartupInfo.cb = sizeof(STARTUPINFOEXW);

    SIZE_T attrListSize;
    InitializeProcThreadAttributeList(NULL, 1, 0, &attrListSize);
    si.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attrListSize);
    InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attrListSize);
    UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                              hPC, sizeof(hPC), NULL, NULL);
    PROCESS_INFORMATION pi{};
    CreateProcessW(NULL, cmd.data(), NULL, NULL, FALSE,
                   EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &si.StartupInfo, &pi);
    CloseHandle(hPipeInWrite);
    CloseHandle(hPipeInRead);
    CloseHandle(hPipeOutWrite);

    char buffer[4096];
    DWORD bytesAvailable = 0;

    enum class VTState { NORMAL, OSC, CSI };
    VTState vtState = VTState::NORMAL;

    Sleep(500);
    while (true)
    {
        if (!PeekNamedPipe(hPipeOutRead, NULL, 0, NULL, &bytesAvailable, NULL)) break;
        if (bytesAvailable > 0)
        {
            DWORD bytesRead;
            ReadFile(hPipeOutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
            for (DWORD i = 0; i < bytesRead; i++)
            {
                unsigned char c = buffer[i];
                switch (vtState)
                {
                    case VTState::NORMAL:
                        if (c == 0x1B)
                        {
                            if (i + 1 < bytesRead)
                            {
                                if (buffer[i + 1] == ']') { vtState = VTState::OSC; i++; break; }
                                if (buffer[i + 1] == '[') { vtState = VTState::CSI; i++; break; }
                            }
                        }
                        if (c == '\r') break;
                        outTerm += c;
                        break;
                    case VTState::OSC:
                        if (c == 0x07 || (c == '\\' && i > 0 && buffer[i - 1] == 0x1B))
                            vtState = VTState::NORMAL;
                        break;
                    case VTState::CSI:
                        if (c >= 0x40 && c <= 0x7E) vtState = VTState::NORMAL;
                        break;
                }
            }
        }
        else
        {
            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            if (exitCode != STILL_ACTIVE) break;
        }
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hPipeOutRead);
    ClosePseudoConsole(hPC);
    DeleteProcThreadAttributeList(si.lpAttributeList);
    HeapFree(GetProcessHeap(), 0, si.lpAttributeList);

    outTerm += "\n";
    return outTerm;
}

#else

#include <pty.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory>

std::string ptyfunc(std::string sqlinput,
                    std::string inputuser,
                    std::string inputpass,
                    std::string inputserver,
                    std::string inputport,
                    std::string inputdatabase)
{
    std::string outTerm = "";
    using namespace std::string_literals;
    std::string userarg = "--user="s + inputuser;
    std::string passarg = "--password="s + inputpass;
    std::string srvrarg = "--host="s + inputserver;
    std::string portarg = "--port="s + inputport;
    std::string basearg = "--database="s + inputdatabase;
    std::string query   = "-e "s + sqlinput;

    int master;
    pid_t pid = forkpty(&master, nullptr, nullptr, nullptr);
    if (pid == 0)
    {
        execlp("mysql", "mysql",
               userarg.data(), passarg.data(), srvrarg.data(),
               basearg.data(), query.data(), nullptr);
        _exit(1);
    }

    char buffer[4096];
    while (true)
    {
        ssize_t ln = read(master, buffer, sizeof(buffer) - 1);
        if (ln <= 0) break;
        for (int w = 0; w < ln; w++)
            if (buffer[w] != '\r') outTerm += buffer[w];
    }
    wait(nullptr);
    return outTerm;
}

#endif
