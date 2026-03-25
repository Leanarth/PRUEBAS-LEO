#pragma once

// Para poder compilar desde linux un .exe con MinGW, necesito "renombrar" las variables
// siguientes despues de la palabra #define, ya que al importar la libreria mysql.h, esta
// misma por si sola importa a windows.h si detecta que se esta compilando para un sistema
// windows, y al llamarse windows.h ocurren conflictos de nombres de funciones ya nombradas
// de la libreria windows.h contra la libreria raylib, ya que windows.h es de C, como raylib
// tambien, y no tienen namespaces para evitar conflictos de nombres, asi que lo que hago con
// #define es "renombrar" esas funciones para evitar conflictos entre raylib.h y windows.h.

#define DrawText    DrawText_Windows
#define LoadImage   LoadImage_Windows
#define ShowCursor  ShowCursor_Windows
#define Rectangle   Rectangle_Windows
#define PlaySound   PlaySound_Windows
#define CloseWindow CloseWindow_Windows
#define DrawTextEx  DrawTextEx_Windows

#if defined(_WIN32)  // En caso de que al compilarse se detecte que se compilará para windows, pasará por este if para llamar a winsock2.h
#include <winsock2.h>   // Debe incluirse antes de windows.h para prevenir errores
#endif

#include <mysql/mysql.h>

#undef DrawTextEx
#undef CloseWindow
#undef PlaySound
#undef Rectangle
#undef ShowCursor
#undef LoadImage
#undef DrawText

#include "raylib.h"  // Llamada a la librería raylib, la cual es responsable de la interfaz gráfica
#include "hpdf.h"  // Llamada a la librería HPDF, la cual se encarga de la creación de los PDFs
