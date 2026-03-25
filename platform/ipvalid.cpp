#include "ipvalid.hpp"
#include <string>

#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

using namespace std::string_literals;             // Sirve para concatenar de forma más eficaz los strings, escribiendo un string seguido de una s (algo así como "hola"s)

bool validIP(const std::string ip)                // Es una función que recibe una IP en formato string, devuelve true si la IP es válida, o false en caso de que no
{
#ifdef _WIN32                                     // Si el sistema operativo es windows, se inicializará winsock con las siguientes dos líneas
    WSADATA wsaData;                              // Crea la estructura donde windows guarda la información de red
    WSAStartup(MAKEWORD(2, 2), &wsaData);         // Inicializa winsock, con la versión 2.2 (MAKEWORK(2,2) le dice a winsock el uso de la versión 2.2)
#endif
    sockaddr_in  sa;            // Nombra a sa como estructura de IPv4
    sockaddr_in6 sa6;           // Nombra a sa6 como estructura de IPv6
    if (inet_pton(AF_INET,  ip.c_str(), &(sa.sin_addr))  == 1 ||              // Evalúa si la IP en formato IPv4 es válida, o...
        inet_pton(AF_INET6, ip.c_str(), &(sa6.sin6_addr)) == 1)               // Evalúa también si la IP en formato IPv6 es válida
    {
        std::string cmd = "";                                     // Crea a cmd con un string vacío
#ifdef _WIN32                                                     // Si el sistema operativo es windows...
        cmd = "ping -n 1 -w 1000 "s + ip + " > nul"s;             // Guarda el comando de un ping con las opciones de windows para ver si existe conexión con la IP solicitada
#else                                                             // Si el sistema operativo NO es windows...
        cmd = "ping -c 1 -W 1 "s + ip + "> /dev/null 2>&1"s;      // Guarda el comando de un ping con las opciones de linux para ver si existe conexión con la IP solicitada
#endif
        int success = std::system(cmd.data());    // Guarda el código de estado de la ejecución del comando (0 en caso de que sí haya conexión con la IP, otro número que no sea o en caso de que no haya conexión)
        return success == 0;    // Compara al código de estado anteriormente guardado en success, con 0, si el código de estado es igual a 0, retornará true, en caso contrario retornará false
    }
    return false;               // Retorna false en caso de que la IP no sea válida
}
