#pragma once
#include "../globals.hpp"
#include "input.hpp"
#include <string>
#include <vector>

// ── Forward declarations ──────────────────────────────────────────────────────
int centertext(std::string message, double width, double fontsize);
std::string keepLastNLines(const std::string& text, int n = 1000);
// ── PrettyDrawRectangle ───────────────────────────────────────────────────────
template<typename T>
inline void PrettyDrawRectangle(T obj)              // Esta función se encarga de recibir un objeto y dibujarlo con bordes redondos, además de líneas redondas
{
    DrawRectangleRounded({(float)obj->xloc, (float)obj->yloc,           // Dibuja el objeto según sus propiedades xloc, yloc, xsize y ysize con bordes redondos
                          (float)obj->xsize, (float)obj->ysize},
                         0.2f, 0, obj->status > 1 ? obj->highColor : obj->normalColor);       // Lo rellena con un color dependiendo de su estado, si el objeto tiene un estado mayor a cero, lo rellena con highColor, sino con normalColor
    DrawRectangleRoundedLinesEx({(float)obj->xloc, (float)obj->yloc,    // Dibuja las líneas del borde del objeto según sus propiedades xloc, yloc, xsize y ysize con bordes redondos
                                 (float)obj->xsize, (float)obj->ysize},
                                0.2f, 0, 1.0f, Fade(BLACK, 0.3f));
}

// ── drawSelected ──────────────────────────────────────────────────────────────
template<typename V>
inline void drawSelected(V butVector,               // Esta función se encarga de dibujar los objetos que se encuentran en un vector
                        double fontsize,            // También pide un tamaño del font para los nombres
                        std::string selected)       // String que se compara para verificar si por ejemplo, el botón en el vector coincide con la pestaña en uso en el panel de administración, entonces aunque su status cambie, se rellena resaltado
{
    for (int i = 0; i < (int)butVector.size(); i++)       // Recorre todos los botones del vector
    {
        if (butVector[i]->name != selected)               // Si el botón actual en el vector NO coincide con el string del nombre del botón resaltado, entonces...
        {
            PrettyDrawRectangle(butVector[i]);            // Simplemente lo dibuja con PrettyDrawRectangle, el cual por sí mismo se encarga de verificar su estado
        }
        else                                              // En caso de que el nombre del botón SÍ coincida con el string del nombre del botón resaltado, entonces se procederá a dibujar el botón de forma manual resaltado
        {
            DrawRectangleRounded({(float)butVector[i]->xloc, (float)butVector[i]->yloc,                             // Dibuja el botón con bordes redondos
                                  (float)butVector[i]->xsize, (float)butVector[i]->ysize},
                                 0.2f, 0, butVector[i]->highColor);
            DrawRectangleRoundedLinesEx({(float)butVector[i]->xloc, (float)butVector[i]->yloc,                      // Dibuja las líneas del borde del botón con bordes redondos
                                         (float)butVector[i]->xsize, (float)butVector[i]->ysize},
                                        0.2f, 0, 1.0f, Fade(BLACK, 0.3f));
        }
        float width = (butVector[i]->xloc + butVector[i]->xsize * 0.5f) * 2.0f;                 // Se calcula el ancho del botón, esto se usará para luego llamar a centertext() para centrar el nombre en el botón
        DrawTextEx(fontTtf, butVector[i]->name.data(),                                          // Se dibuja el nombre del botón
                   (Vector2){(float)centertext(butVector[i]->name, width, fontsize),
                              (float)((butVector[i]->yloc + butVector[i]->ysize + butVector[i]->yloc) / 2.0 - fontsize / 2.0)},
                   fontsize, 2, BLACK);
    }
}

// ── Non-template declarations (bodies in drawing.cpp) ────────────────────────
int         logfunction(std::string selected, double lastColumnMeasures = 0, double fsize = 0);
std::string drawcolumns(std::vector<sqlobject*>& cTables, std::vector<column*>& cVector,
                        std::string& tSelected, double fsize,
                        std::string& selected, char* mode = (char*)"default");
int         shortmessage(std::string msg, double fs, bool &activator, int timeFps = 150);
std::vector<double> statistics(std::string mode, std::string outputMode,
                               std::vector<double>& dataVec,
                               std::vector<sqlobject*> partVec,
                               int posx = 0, int posy = 0);
