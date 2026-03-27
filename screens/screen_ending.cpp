#include "../globals.hpp"
#include "screen_ending.hpp"
#include "../ui/drawing.hpp"

void screenEndingUpdate(Screen& currentScreen, int verifyvote, bool& correctstudent)
{
    framesCounter++;
    if (framesCounter > 600)  // 60 fps * 10 seg = 600 frames
    {
        cedulaBarPtr->status = 4;
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
        DrawTextEx(fontTtf, "Ya usted ha votado, no puede hacerlo de nuevo",
                   (Vector2){(float)centertext("Ya usted ha votado, no puede hacerlo de nuevo", screenWidth, fontSize),
                              (float)(screenHeight * 0.5)},
                   fontSize, 2, BLACK);
    }
    else
    {
        DrawTextEx(fontTtf, "Muchas gracias por su voto, puede retirarse",
                   (Vector2){(float)centertext("Muchas gracias por su voto, puede retirarse", screenWidth, fontSize),
                              (float)(screenHeight * 0.5)},
                   fontSize, 2, BLACK);
    }
}
