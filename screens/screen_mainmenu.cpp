#include "../globals.hpp"
#include "screen_mainmenu.hpp"
#include "../db/database.hpp"
#include "../ui/input.hpp"
#include "../ui/drawing.hpp"

void screenMainmenuUpdate(Screen& currentScreen, bool& existstudent, std::string& studentName)
{
    cedulaBarPtr->status = isPressed(cedulaBarPtr);
    if (cedulaBarPtr->status > 1)
        inputfunc("backend", cedulaBarPtr, 21, "allchars", fontSize);

    cedulaPtr->status = isPressed(cedulaPtr);
    std::cout<<cedulaPtr->status<<"\n";
    if ((IsKeyPressed(KEY_ENTER) || cedulaPtr->status == 4) &&
        cedulaBarPtr->input == *admPassword)
    {
        currentScreen = ADMINMENU;
        cedulaBarPtr->status = 0;
        cedulaBarPtr->input  = "";
        return;
    }

    if ((IsKeyPressed(KEY_ENTER) || cedulaPtr->status == 4) &&
        !cedulaBarPtr->input.empty())
    {
        cedulaPtr->selfquery += cedulaBarPtr->input;
        sendquery((cedulaPtr->selfquery + "'"s).data(), 1, 1);

        if (!outQuery.empty())
        {
            existstudent = true;
            studentName  = outQuery;
        }
        else
        {
            existstudent = false;
            regresarPtr->xloc = cedulaPtr->xloc;
        }
        cedulaBarPtr->status = 0;
        currentScreen = CONFIRMATION;
    }
}

void screenMainmenuDraw()
{
    DrawTextEx(fontTtf, "Digite su cedula en este campo"s.data(),
               (Vector2){(float)centertext("Digite su cedula en este campo"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.23)},
               fontSize, 2, BLACK);
    DrawTextEx(fontTtf, "con uno de los siguientes formatos"s.data(),
               (Vector2){(float)centertext("con uno de los siguientes formatos"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.3)},
               fontSize, 2, BLACK);
    DrawTextEx(fontTtf, "101110111 | 123456789012 | AZ0000-11111"s.data(),
               (Vector2){(float)centertext("101110111 | 123456789012 | AZ0000-11111"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.37)},
               fontSize, 2, BLACK);
    PrettyDrawRectangle(cedulaBarPtr);
    PrettyDrawRectangle(cedulaPtr);
    DrawTextEx(fontTtf, cedulaPtr->name.data(),
               (Vector2){cedulaPtr->xloc + (float)centertext(cedulaPtr->name, cedulaPtr->xsize, fontSize),
                          cedulaPtr->yloc + (float)((cedulaPtr->ysize - fontSize) / 2)},
               fontSize, 2, BLACK);
    inputfunc("frontend", cedulaBarPtr, 0, "allchars", fontSize);
}
