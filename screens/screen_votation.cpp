#include "../globals.hpp"
#include "screen_votation.hpp"
#include "../ui/input.hpp"
#include "../ui/drawing.hpp"

// Función del backend de la pantalla VOTATION
void screenVotationUpdate(Screen& currentScreen,        // Necesita la variable currentScreen para cambiar la pantalla del programa
                          bool& votoBlanco)             // Necesita la variable votoBlanco para modificar y leer su valor
{
    // Se actualiza el estado de todos los botones cada frame para detectar hover correctamente
    votarPtr->status = isPressed(votarPtr);             // Actualiza el estado del botón votar cada frame para detectar hover

    for (int i = 0; i < (int)partidosVec.size(); i++)  // Recorrerá todos los partidos
    {
        partidosVec[i]->status = isPressed(partidosVec[i]);                 // Verifica el estado de cada partido
        if (partidosVec[i]->status == 4)                                    // Si un partido recibe un clic
            partidoSelected = partidosVec[i]->name;                         // El partido que recibió el clic se declara como partido seleccionado
    }

    if (votarPtr->status == 4)                          // Si se presiona el botón votar...
    {
        if (!partidoSelected.empty())                   // Si hay un partido seleccionado...
        {
            continuarPtr->status = 0;                   // Se resetea el estado de continuar para que no aparezca resaltado al llegar a CONFIRMATION
            regresarPtr->status  = 0;                   // Se resetea el estado de regresar también por la misma razón
            votarPtr->status     = 0;                   // Se resetea el estado de votar
            for (int i = 0; i < (int)partidosVec.size(); i++)
                partidosVec[i]->status = 0;             // Se resetea el estado de todos los partidos
            currentScreen = CONFIRMATION;
        }
        else votoBlanco = true;                         // Si no hay partido seleccionado, muestra mensaje de error
    }
}

// Frontend de VOTATION
void screenVotationDraw(bool& votoBlanco)               // Necesita a la variable votoBlanco para verificar si debe mostrar el mensaje
{
    DrawTextEx(fontTtf, "Por favor, escoja un partido por el que desea votar"s.data(),
               (Vector2){(float)centertext("Por favor, escoja un partido por el que desea votar"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.1)},
               fontSize, 2, WHITE);                     // WHITE porque flota sobre el fondo negro
    DrawTextEx(fontTtf, "O si lo desea, puede votar nulo"s.data(),
               (Vector2){(float)centertext("O si lo desea, puede votar nulo"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.2)},
               fontSize, 2, WHITE);                     // WHITE porque flota sobre el fondo negro
    drawSelected(partidosVec, littleFontSize * 1.5, partidoSelected);       // Dibujará a los partidos disponibles
    PrettyDrawRectangle(votarPtr);                                           // Dibujará el botón para votar
    DrawTextEx(fontTtf, votarPtr->name.data(),
               (Vector2){votarPtr->xloc + (float)centertext(votarPtr->name, continuarPtr->xsize, fontSize),
                          votarPtr->yloc + (float)((votarPtr->ysize - fontSize) / 2)},
               fontSize, 2, BLACK);                     // BLACK porque el botón es beige
    if (votoBlanco) shortmessage("Tiene que seleccionar una opción", mediumFontSize, votoBlanco);   // Si votoBlanco es verdadero, aparecerá un mensaje
}