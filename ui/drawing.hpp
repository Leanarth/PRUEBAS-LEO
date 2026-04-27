#pragma once
#include "../globals.hpp"
#include "input.hpp"
#include <string>
#include <vector>
// Color dorado constante para los bordes de todos los botones del programa
static const Color DORADO_BORDE = {212, 175, 55, 255};  // Dorado para los bordes de todos los botones
// ── Forward declarations ──────────────────────────────────────────────────────
int centertext(std::string message, double width, double fontsize);
std::string keepLastNLines(const std::string& text, int n = 1000);
// ── PrettyDrawRectangle ───────────────────────────────────────────────────────
template<typename T>
inline void PrettyDrawRectangle(T obj)
{
    // Dorado puro para el borde de todos los botones — los 4 números son rojo, verde, azul y opacidad (255 = completamente visible)
    Color DORADO_BORDE = {212, 175, 55, 255};

    // Cuando status es 3 (presionado con cursor encima) o 4 (clic en este instante exacto),
    // el botón se desplaza 3 pixeles hacia abajo y se encoge 3 pixeles, simulando que fue físicamente hundido
    float pressOffset = (obj->status == 3 || obj->status == 4) ? 3.0f : 0.0f;
    float sizeReduce  = (obj->status == 3 || obj->status == 4) ? 3.0f : 0.0f;

    // Cuando status es 1 (el cursor está encima pero sin haber hecho clic todavía),
    // el botón crece 2 pixeles en todas las direcciones para avisarle al usuario que puede presionarlo
    float hoverExpand = (obj->status == 1) ? 2.0f : 0.0f;

    // Se recalculan las coordenadas y el tamaño del rectángulo aplicando los efectos de hover y presión.
    // rx y ry son la nueva esquina superior izquierda, rw y rh son el nuevo ancho y alto
    float rx = obj->xloc  - hoverExpand;                      // Se mueve a la izquierda si hay hover
    float ry = obj->yloc  - hoverExpand + pressOffset;        // Se mueve arriba si hay hover, abajo si hay presión
    float rw = obj->xsize + hoverExpand * 2 - sizeReduce;     // Crece si hay hover, encoge si hay presión
    float rh = obj->ysize + hoverExpand * 2 - sizeReduce;     // Igual en vertical

    // Sombra: se dibuja un rectángulo negro semitransparente desplazado 3px a la derecha y 4px abajo.
    // Desaparece al presionar (status 3 o 4) para reforzar visualmente el efecto de hundimiento,
    // ya que un objeto hundido no proyecta sombra hacia adelante
    if (obj->status != 3 && obj->status != 4)
        DrawRectangleRounded({rx + 3, ry + 4, rw, rh}, 0.5f, 0, Fade(BLACK, 0.25f));
    // El 0.5f es la redondez de las esquinas (va de 0.0 = cuadrado a 1.0 = círculo perfecto, 0.5 = forma de píldora)
    // El Fade(BLACK, 0.25f) hace el negro con solo 25% de opacidad, dando una sombra suave

    // Cuerpo principal del botón.
    // Si status > 1 (fue presionado antes) usa highColor (beige cálido), si no usa normalColor (beige pastel).
    // El operador ? : funciona como: si (condición) usa esto : sino usa esto
    DrawRectangleRounded({rx, ry, rw, rh},
                         0.5f, 0, obj->status > 1 ? obj->highColor : obj->normalColor);

    // Grosor del borde dorado según el estado:
    // — 3.0f pixeles cuando hay hover (status 1): borde más grueso para reforzar que es interactivo
    // — 1.0f pixeles cuando está presionado (status 3 o 4): borde fino porque el botón se "hundió"
    // — 2.0f pixeles en reposo (cualquier otro caso): grosor normal
    // Grosor del borde dorado: más grueso en hover, normal en reposo, más fino al presionar
    float borderThick = (obj->status == 1)                     ? 5.0f :   // Hover: 5px para dar feedback visual claro
                        (obj->status == 3 || obj->status == 4) ? 2.0f :   // Presionado: 2px, el botón se "hundió"
                                                                  3.5f;   // Reposo: 3.5px, borde dorado visible y elegante
    DrawRectangleRoundedLinesEx({rx, ry, rw, rh}, 0.5f, 0, borderThick, DORADO_BORDE);
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
                                 0.5f, 0, butVector[i]->highColor);
            DrawRectangleRoundedLinesEx({(float)butVector[i]->xloc, (float)butVector[i]->yloc,                      // Dibuja las líneas del borde del botón con bordes redondos
                                         (float)butVector[i]->xsize, (float)butVector[i]->ysize},
                                        0.5f, 0, 4.5f, DORADO_BORDE);
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
bool        alert(std::string botonActual);
