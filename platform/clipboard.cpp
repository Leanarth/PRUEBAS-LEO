#include "clipboard.hpp"
#include "encoding.hpp"
#include <string>       // Llamada a la librería string, debido a que se usarán strings especiales como UTF-32 y funciones relacionadas a strings

#if defined(_WIN32)     // En caso de que se detecte que se está compilando el código para windows, se declarará la función clipboard() de la siguiente manera...

#include <winsock2.h>   // Llamada a la librería winsock2.h, encargada de la creación de sockets y conexiones de windows
#include <windows.h>    // Llamada a la librería windows.h, la cual interactúa con la API de Windows

std::u32string clipboard()                                            // Definición de clipboard, std::u32string significa que es una función que devolverá un string en formato UTF-32
{
    if (!OpenClipboard(nullptr)) return U"";                          // Si no se puso abrir el portapapeles de Windows, retornará un string en formato UTF-32 vacío, y terminará la función

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);                  // HANDLE es un tipo de Windows que representa un recurso del sistema, GetClipboardData(CF_UNICODETEXT) pide el contenido del clipboard en formato Unicode
    if (!hData) {CloseClipboard(); return U"";}                       // Si falla la línea anterior o está vacío, devuelve un string vacío en formato UTF-32

    wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));      // Bloquea el portapapeles a través de GlobalLock(hData) para que solo el programa pueda acceder a él a través de un puntero, y ese puntero lo convierte a wchar_t (UTF-16)
    if (!pszText) {CloseClipboard(); return U"";}                     // Si ocurre algo inesperado en la línea anterior, devuelve un string vacío en formato UTF-32

    size_t len = wcslen(pszText);                                     // Se calcula el tamaño del string con wcslen(), ya que al ser UTF-16 no puede usar strlen() para medirse
    std::u16string u16(reinterpret_cast<char16_t*>(pszText),          // Entre estas líneas, convierto el wchar_t que usa Windows, a char16_t que usa C++ para UTF-16
                       reinterpret_cast<char16_t*>(pszText) + len);   // desde el inicio hasta el final del texto
    GlobalUnlock(hData);                                              // Desbloqueo el portapapeles, ya que no necesito accederlo más
    CloseClipboard();                                                 // Cierro el portapapeles
    return UTF16ToUTF32(u16);                                         // Convierto el string de UTF-16 a UTF-32, por medio de la función UTF16ToUTF32()
}

#else                   // En caso de que se detecte que NOOOOO se está compilando el código para windows, se declarará la función clipboard() de la siguiente manera...

#include <cstdio>       // Llama a la librería de C (no c++) de entrada y salida de datos
#include <cstdlib>      // Llama a la librería de las funciones básicas de C (no c++)
#include <memory>       // Llama a la librería memory, debido a que necesita funciones relacionadas a punteros (unique_ptr<>)

std::u32string clipboard()                                            // Declaro la función clipboard como u32string, por la misma razón como la declaré para windows
{
    std::string utf8data;                                             // Declara una variable llamada utf8data, que almacenará datos en string en UTF-8
    char buffer[4096];                                                // Declara un buffer de 4096 bytes para almacenar la salida de los comandos a continuación
    std::string cmd;                                                  // Declara un string que almacenará el comando a ejecutar, dependiendo del entorno

    if (std::getenv("WAYLAND_DISPLAY")) {                             // Si al ejecutar std::getenv() se detecta que el entorno es de Wayland, asignará al string cmd el comando wl-paste para obtener el texto del portapapeles
        cmd = "wl-paste";                                             // Comando que muestra el texto del portapapeles en Wayland se le asigna a cmd
    }

    else {cmd = "xclip -selection clipboard -o";}                     // En caso contrario, se usará el comando que suelen usar entornos distintos para obtener el portapapeles

    auto closer = [](FILE* fp) { pclose(fp); };   // Es una lambda para limpiar a la variable de tipo FILE
    // Se declara un puntero para los datos que se reciban de resultado del comando ejecutado
    // FILE es el lugar que recibirá los datos
    // La función popen() se encarga de ejecutar el comando cmd al convertirlo en formato legible para C (ya que la función popen() está hecha en C) por medio de data()
    // El argumento que se le pasa a pipe() el cual es una "r" significa que hará la accion "read" (leer) la salida del comando
    std::unique_ptr<FILE, decltype(closer)> pipe(popen(cmd.data(), "r"), closer);
    if (!pipe) return U"";                                            // Si ocurre un error en la línea anterior, devolverá un string en UTF-32 vacío

    while (true)                                                      // Se ejecutará un bucle infinito que solo se romperá cuando detecte que n sea igual a 0 en las líneas siguientes...
    {
        // Lee hasta el tamaño posible en bytes que se pueda meter en la variable buffer (es decir, 4096 bytes), y procede a meter los bytes dentro de buffer
        // El tamaño de n es la cantidad de bytes que logró leer en el buffer
        size_t n = fread(buffer, 1, sizeof(buffer), pipe.get());
        if (n == 0) break;                                            // Si n es igual a 0 (es decir, el buffer quedó vacío) procederá a romper el bucle
        utf8data.append(buffer, n);                                   // En caso contrario a la línea anterior, introducirá los bytes leídos adentro del string utf8data, el cual sirve como acumulador
    }

    return UTF8ToUTF32(utf8data);                                     // Por último, cuando el while termine, convertirá los bytes a formato UTF-32
}

#endif
