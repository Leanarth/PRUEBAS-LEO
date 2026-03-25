#include "../globals.hpp"
#include "screen_votation.hpp"
#include "../ui/input.hpp"
#include "../ui/drawing.hpp"

void screenVotationUpdate(Screen& currentScreen, bool& votoBlanco)
{
    for (int i = 0; i < (int)partidosVec.size(); i++)
    {
        partidosVec[i]->status = isPressed(partidosVec[i]);
        if (partidosVec[i]->status == 3)
            partidoSelected = partidosVec[i]->name;
    }

    if (isPressed(votarPtr) == 3)
    {
        if (!partidoSelected.empty()) currentScreen = CONFIRMATION;
        else                          votoBlanco = true;
    }
}

void screenVotationDraw(bool& votoBlanco)
{
    DrawTextEx(fontTtf, "Por favor, escoja un partido por el que desea votar"s.data(),
               (Vector2){(float)centertext("Por favor, escoja un partido por el que desea votar"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.1)},
               fontSize, 2, BLACK);
    DrawTextEx(fontTtf, "O si lo desea, puede votar nulo"s.data(),
               (Vector2){(float)centertext("O si lo desea, puede votar nulo"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.2)},
               fontSize, 2, BLACK);
    drawSelected(partidosVec, littleFontSize * 1.5, partidoSelected);
    PrettyDrawRectangle(votarPtr);
    DrawTextEx(fontTtf, votarPtr->name.data(),
               (Vector2){votarPtr->xloc + (float)centertext(votarPtr->name, continuarPtr->xsize, fontSize),
                          votarPtr->yloc + (float)((votarPtr->ysize - fontSize) / 2)},
               fontSize, 2, BLACK);
    if (votoBlanco) shortmessage("Tiene que seleccionar una opcion", mediumFontSize, votoBlanco);
}
