#include "../globals.hpp"
#include "screen_confirmation.hpp"
#include "../db/database.hpp"
#include "../ui/input.hpp"
#include "../ui/drawing.hpp"

void screenConfirmationUpdate(Screen& currentScreen,
                              bool& existstudent, bool& correctstudent,
                              int& verifyvote)
{
    if (!existstudent)
    {
        if (isPressed(regresarPtr) == 3)
        {
            cedulaBarPtr->status = 3;
            correctstudent = false;
            cedulaPtr->selfquery  = "SELECT * FROM Estudiantes WHERE Cedula = '";
            votarPtr->selfquery   = "UPDATE Estudiantes SET Voto = '";
            regresarPtr->xloc     = cedulaPtr->xloc * 0.7;
            currentScreen = MAINMENU;
        }
        return;
    }

    if (isPressed(regresarPtr) == 3)
    {
        if (!correctstudent)
        {
            cedulaBarPtr->status = 3;
            correctstudent = false;
            cedulaPtr->selfquery  = "SELECT * FROM Estudiantes WHERE Cedula = '";
            votarPtr->selfquery   = "UPDATE Estudiantes SET Voto = '";
            regresarPtr->xloc     = cedulaPtr->xloc * 0.7;
            currentScreen = MAINMENU;
        }
        else { currentScreen = VOTATION; }
        return;
    }

    if (isPressed(continuarPtr) == 3)
    {
        if (!correctstudent)
        {
            correctstudent = true;
            continuarPtr->selfquery = cedulaPtr->selfquery + "' && Voto = '0'"s;
            sendquery(continuarPtr->selfquery.data(), 0, 0);
            verifyvote = outQuery.length();
            currentScreen = verifyvote > 0 ? VOTATION : ENDING;
        }
        else
        {
            // El estudiante confirma su voto
            votarPtr->selfquery += "1' WHERE Cedula = '"s + cedulaBarPtr->input + "'";
            sendquery(votarPtr->selfquery.data(), 0, 0, 0);
            votarPtr->selfquery  = "UPDATE Partidos SET Votos = Votos + 1 WHERE Nombre = '";
            votarPtr->selfquery += partidoSelected;
            votarPtr->selfquery += "';";
            sendquery(votarPtr->selfquery.data(), 0, 0, 0);
            currentScreen = ENDING;
        }
    }
}

void screenConfirmationDraw(bool existstudent, bool correctstudent,
                            const std::string& studentName)
{
    if (!correctstudent && existstudent)
    {
        DrawTextEx(fontTtf, "Verifique si su nombre es correcto:"s.data(),
                   (Vector2){(float)centertext("Verifique si su nombre es correcto:"s, screenWidth, fontSize),
                              (float)(screenHeight * 0.2)},
                   fontSize, 2, BLACK);
        DrawTextEx(fontTtf, studentName.data(),
                   (Vector2){(float)centertext(studentName, screenWidth, fontSize),
                              (float)(screenHeight * 0.3)},
                   fontSize, 2, BLACK);
        PrettyDrawRectangle(continuarPtr);
        PrettyDrawRectangle(regresarPtr);
        DrawTextEx(fontTtf, continuarPtr->name.data(),
                   (Vector2){continuarPtr->xloc + (float)centertext(continuarPtr->name, continuarPtr->xsize, fontSize),
                              continuarPtr->yloc + (float)((continuarPtr->ysize - fontSize) / 2)},
                   fontSize, 2, BLACK);
        DrawTextEx(fontTtf, regresarPtr->name.data(),
                   (Vector2){regresarPtr->xloc + (float)centertext(regresarPtr->name, regresarPtr->xsize, fontSize),
                              regresarPtr->yloc + (float)((regresarPtr->ysize - fontSize) / 2)},
                   fontSize, 2, BLACK);
    }
    else if (!existstudent)
    {
        DrawTextEx(fontTtf, "Usted no se encontro en la base de datos del VOCA"s.data(),
                   (Vector2){(float)centertext("Usted no se encontro en la base de datos del VOCA"s, screenWidth, fontSize),
                              (float)(screenHeight * 0.2)},
                   fontSize, 2, BLACK);
        DrawTextEx(fontTtf, "Verifique que su cedula fue digitada correctamente:"s.data(),
                   (Vector2){(float)centertext("Verifique que su cedula fue digitada correctamente:"s, screenWidth, fontSize),
                              (float)(screenHeight * 0.3)},
                   fontSize, 2, BLACK);
        DrawTextEx(fontTtf, cedulaBarPtr->input.data(),
                   (Vector2){(float)centertext(cedulaBarPtr->input, screenWidth, fontSize),
                              (float)(screenHeight * 0.4)},
                   fontSize, 2, BLACK);
        PrettyDrawRectangle(regresarPtr);
        DrawTextEx(fontTtf, regresarPtr->name.data(),
                   (Vector2){regresarPtr->xloc + (float)centertext(regresarPtr->name, regresarPtr->xsize, fontSize),
                              regresarPtr->yloc + (float)((regresarPtr->ysize - fontSize) / 2)},
                   fontSize, 2, BLACK);
    }
    else
    {
        std::string mselected = "Selecciono "s + partidoSelected + " es correcto?"s;
        DrawTextEx(fontTtf, mselected.data(),
                   (Vector2){(float)centertext(mselected, screenWidth, fontSize),
                              (float)(screenHeight * 0.2)},
                   fontSize, 2, BLACK);
        PrettyDrawRectangle(continuarPtr);
        PrettyDrawRectangle(regresarPtr);
        DrawTextEx(fontTtf, continuarPtr->name.data(),
                   (Vector2){continuarPtr->xloc + (float)centertext(continuarPtr->name, continuarPtr->xsize, fontSize),
                              continuarPtr->yloc + (float)((continuarPtr->ysize - fontSize) / 2)},
                   fontSize, 2, BLACK);
        DrawTextEx(fontTtf, regresarPtr->name.data(),
                   (Vector2){regresarPtr->xloc + (float)centertext(regresarPtr->name, regresarPtr->xsize, fontSize),
                              regresarPtr->yloc + (float)((regresarPtr->ysize - fontSize) / 2)},
                   fontSize, 2, BLACK);
    }
}
