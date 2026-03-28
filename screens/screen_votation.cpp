#include "../globals.hpp"
#include "screen_votation.hpp"
#include "../ui/input.hpp"
#include "../ui/drawing.hpp"

// Función del backend de la pantalla VOTATION

void screenVotationUpdate(Screen& currentScreen,        // Necesita la variable currentScreen para cambiar la pantalla del programa
                          bool& votoBlanco)             // Necesita la variable votoBlanco para modificar y leer su valor, el cual indica si se intentó votar en blanco, es decir, ninguna opción incluyendo nulo, lo cual NO es válido
{
    for (int i = 0; i < (int)partidosVec.size(); i++)                       // Recorrerá todos los partidos
    {
        partidosVec[i]->status = isPressed(partidosVec[i]);                 // Verifica el estado de cada partido
        if (partidosVec[i]->status == 4)                                    // Si un partido recibe un clic
            partidoSelected = partidosVec[i]->name;                         // El partido que recibió el clic se declara como partido seleccionado
    }

    if (isPressed(votarPtr) == 4)                                           // Si se presiona el botón votar...
    {
        if (!partidoSelected.empty()) currentScreen = CONFIRMATION;         // Si partidoSelected NO se encuentra vacío, enviará al usuario a CONFIRMATION para confirmar su voto
        else                          votoBlanco = true;                    // Si partidoSelected SÍ se encuentra vacío, le mostrará al usuario en el frontend un mensaje diciendole de que NO se puede votar en blanco
    }
}

// Frontend de VOTATION

void screenVotationDraw(bool& votoBlanco)           // Necesita a la variable votoBlanco para verificar si debe mostrar el mensaje comunicando de que debe votar por una opción
{
    DrawTextEx(fontTtf, "Por favor, escoja un partido por el que desea votar"s.data(),                                            // En el frontend mostrará un mensaje diciendo de que vote por algún partido
               (Vector2){(float)centertext("Por favor, escoja un partido por el que desea votar"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.1)},
               fontSize, 2, BLACK);
    DrawTextEx(fontTtf, "O si lo desea, puede votar nulo"s.data(),                                                                // Además de que comunicará que puede votar nulo
               (Vector2){(float)centertext("O si lo desea, puede votar nulo"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.2)},
               fontSize, 2, BLACK);
    drawSelected(partidosVec, littleFontSize * 1.5, partidoSelected);                                                             // Dibujará a los partidos disponibles
    PrettyDrawRectangle(votarPtr);                                                                                                // Dibujará el botón para votar
    DrawTextEx(fontTtf, votarPtr->name.data(),                                                                                    // Y dibujará el nombre de ese botón
               (Vector2){votarPtr->xloc + (float)centertext(votarPtr->name, continuarPtr->xsize, fontSize),
                          votarPtr->yloc + (float)((votarPtr->ysize - fontSize) / 2)},
               fontSize, 2, BLACK);
    if (votoBlanco) shortmessage("Tiene que seleccionar una opción", mediumFontSize, votoBlanco);                                 // Si votoBlanco es verdadero, es decir, se intentó votar por ninguna opción, aparecerá un mensaje
}
