#include "../globals.hpp"
#include "screen_ending.hpp"
#include "../ui/drawing.hpp"

void screenEndingUpdate(Screen& currentScreen, int verifyvote, bool& correctstudent)
{
    framesCounter++;
    if (framesCounter > 600)  // 60 fps * 10 seg = 600 frames
    {
        cedulaBarPtr->status = 3;
        cedulaBarPtr->input  = "";
        cedulaBarPtr->input32 = U"";
        currentScreen = MAINMENU;
        correctstudent = false;
        cedulaPtr->selfquery = "SELECT * FROM Estudiantes WHERE Cedula = '";
        votarPtr->selfquery  = "UPDATE Estudiantes SET Voto = '";
        framesCounter  = 0;
        partidoSelected = "";
    }
}

void screenEndingDraw(int verifyvote)
{
    if (verifyvote == 0)
    {
        char* cantvote = "Ya usted ha votado, no puede hacerlo de nuevo";
        DrawTextEx(fontTtf, cantvote,
                   (Vector2){(float)centertext(cantvote, screenWidth, fontSize),
                              (float)(screenHeight * 0.5)},
                   fontSize, 2, BLACK);
    }
    else
    {
        char* thanks = "Muchas gracias por su voto, puede retirarse";
        DrawTextEx(fontTtf, thanks,
                   (Vector2){(float)centertext(thanks, screenWidth, fontSize),
                              (float)(screenHeight * 0.5)},
                   fontSize, 2, BLACK);
    }
}
