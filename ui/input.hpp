#pragma once
#include "../globals.hpp"
#include "../platform/encoding.hpp"
#include "../platform/clipboard.hpp"

// Forward declaration — body is in drawing.hpp
template<typename T> void PrettyDrawRectangle(T obj);

// ── isPressed ─────────────────────────────────────────────────────────────────
/* isPressed() verifica el estado del objeto que se le pasa como argumento, actualmente hay 4 estados, pero me gustaría posteriormente agregar un estado más:

    0: No presionado, y tampoco hay interacción alguna
    3: Presionado en este instante
    2: Fue presionado anteriormente, y el cursor está encima del objeto
    1: Fue presionado anteriormente, pero el cursor NO está encima del objeto

    Me gustaría modificar los estados para que queden de esta forma:
    0: No presionado, y tampoco hay interacción alguna
    4: Presionado en este instante
    3: Fue presionado anteriormente, y el cursor está encima del objeto
    2: Fue presionado anteriormente, pero el cursor NO está encima del objeto
    1: No fue presionado anteriormente, pero el cursor está encima del objeto

*/

inline int isPressed(auto& obj)
{
    float xloc = obj->xloc, yloc = obj->yloc, xsize = obj->xsize, ysize = obj->ysize;
    int laststate = obj->status;

    if (IsGestureDetected(GESTURE_TAP) &&
        GetTouchX() >= xloc && GetTouchX() <= xloc + xsize &&
        GetTouchY() >= yloc && GetTouchY() <= yloc + ysize)
        return 3;

    if (IsGestureDetected(GESTURE_TAP) &&
        (GetTouchX() < xloc || GetTouchX() > xloc + xsize ||
         GetTouchY() < yloc || GetTouchY() > yloc + ysize))
        return 0;

    if (!IsGestureDetected(GESTURE_TAP) &&
        (GetTouchX() < xloc || GetTouchX() > xloc + xsize ||
         GetTouchY() < yloc || GetTouchY() > yloc + ysize))
        return (laststate == 0) ? 0 : 1;

    if (!IsGestureDetected(GESTURE_TAP) &&
        GetTouchX() >= xloc && GetTouchX() <= xloc + xsize &&
        GetTouchY() >= yloc && GetTouchY() <= yloc + ysize)
        return (laststate == 0) ? 0 : 2;

    return 0;
}

// ── GetCursorFromMouseClick ───────────────────────────────────────────────────
// Desplaza al cursor de las barras de entrada de datos inputBar, hacia donde apunte el cursor del mouse
inline int GetCursorFromMouseClick(auto* bar, int fsize)              // Necesita como argumentos la barra que se detecta el clic, y el tamaño del font de la barra actual
{
    float mouseX     = GetMouseX();                                   // Busca la posición del mouse en el eje X (ancho de la pantalla)
    float textStartX = bar->xloc * 1.04;                              // Busca la posición donde inicia a mostrarse el texto en el frontend, que es usualmente al 4% del largo de la barra

    if (mouseX <= textStartX) return 0;                               // Si la posición del mouse es menor o igual a la posición mínima en la que se encuentra la barra, entonces solo retorna 0, indicando que el cursor NO está en la barra

    for (int i = 0; i <= (int)bar->input32.size(); i++)               // Ahora, recorrerá cada carácter en la  barra de entrada
    {
        std::u32string sub(bar->input32.begin(), bar->input32.begin() + i);     // sub ahora almacenará todos los carácteres desde el inicio del string hasta el carácter que tenga el índice del contador, esto para ir sumandolos para después
        std::string sub8 = UTF32ToUTF8(sub);                                    // Se procede a traducir sub a UTF-8 y se almacena dentro de sub8
        Vector2 width = MeasureTextEx(fontTtf, sub8.c_str(), fsize, 0);         // Se calcula width, el tamaño acerca de la posición que toma el cursor de la barra de datos 
        if (mouseX < textStartX + width.x) return i;                            // Se compara acerca de que si la posición del mouse, sobre la barra de datos de entrada, coincide en su mayoría con la posición calculada, de ser así, se retorna
    }
    return bar->input32.size();               // En caso de que esto no ocurra, el cursor del mouse se encuentra en la posición máxima de la barra, entonces devuelve la posición máxima
}

// ── inputfunc ─────────────────────────────────────────────────────────────────
/* Esta función se encarga de recibir los datos de entrada de los objetos inputBar, además del movimiento en la barra y el pegado (copiado no se ha implementado, y no pienso hacerlo la verdad), además del uso de las codificaciones UTF-32 y UTF-8
   Se encarga de traducir los datos de entrada a UTF-32 a UTF-8 por que lamentablemente Raylib, la librería gráfica, es un dolor de huevos para mostrar carácteres en UTF-32, lo cual habría facilitado todo, además de que funciones como
   desplazarse entre los carácteres por medio de las flechas del teclado, usar el botón DELETE y el botón BACKSPACE y diversas funciones más, se dificultarían más con el uso de UTF-8 internamente, ya que carácteres como la Ñ usan 2 bits,
   no como los carácteres del abecedario ingles como la A, que ocupa solo un bit, entonces tendría que identificar cuándo debe el cursor desplazarse 2 bits en el string, y cuando borrar dos bits, etc, así que decidí usar UTF-32, en el que
   todos los carácteres usan 4 bits, y es mucho mejor de manejar, así que a esto se debe la traducción entre UTF-8 a UTF-32 y viceversa, y también a esto se debe el uso de input32 e input como propiedades de las inputBar  */

inline std::string inputfunc(std::string mode,                            // Verifica cual es el modo, si "backend" o "frontend"
                             auto& bar,                                   // Es el objeto de la barra la cual recibirá datos de entrada o solo mostrará los datos actuales con "frontend"
                             int maxlen = 21,                             // Es el tamaño máximo aceptado de los datos de entrada, por defecto suele admitir únicamente 21 carácteres
                             std::string inputmode = "allchars",          // Verifica el modo de datos de entrada: 
                                                                          /* boolean:         Solo admite unos y ceros
                                                                             allchars:        Admite letras y números, excepto comillas simples
                                                                             numberonly:      Admite solo números
                                                                             regexponly:      Admite números y símbolos
                                                                             allchars-admin:  Admite absolutamente todo tipo de carácteres en el abecedario latín */
                             double fsize = 0,                            // Es el tamaño del font de la letra a mostrar en el frontend
                             Color col = BLACK,                           // Es el color de la letra a mostrar en el frontend
                             int maxBackspace = 7,                        // Es un contador para que la tecla BACKSPACE y DELETE no borren todo el contenido inmediatamente cuando se presionen
                             int maxBeam = 120,                           // Es un contador para verificar cuanto tiempo se debe mostrar la barra del cursor y cuanto tiempo debe ocultarse, para dar el efecto de parpadeo
                             int maxStopArrows = 8)                       // Es parecido al contador de la tecla BACKSPACE y DELETE, solo que con las flechas de derecha e izquierda, sino se desplazaría inmediatamente hasta el otro extremo del texto
{
    if (mode == "backend")                              // Si el modo es "backend"
    {
        if (inputmode != "boolean")                     // Y el modo NO es boolean
        {
            if ((inputpos > (int)bar->input32.size()) || inputpos < 0)
                inputpos = bar->input32.size();

            bool pasted = false;
            std::u32string clipdata = U"";

            if (oldbar != bar->name) { oldbar = bar->name; inputpos = bar->input32.size(); beam = 0; }
            else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_V) &&
                     !stopCtrls && (int)bar->input.length() < maxlen)
            {
                clipdata  = clipboard();
                pasted    = true;
                stopCtrls = 1;
            }

            int codepoint = GetCharPressed();
            while (codepoint > 0 || pasted)
            {
                if (pasted)
                {
                    codepoint = (int)clipdata[0];
                    clipdata.erase(0, 1);
                    if (clipdata.empty()) pasted = false;
                }
                if ((int)bar->input32.size() < maxlen)
                {
                    if ((inputmode == "numberonly" && (codepoint > 47 && codepoint < 58)) ||
                        (inputmode == "regexponly" && codepoint != 39 &&
                         ((codepoint > 32 && codepoint < 65) ||
                          (codepoint > 90 && codepoint < 96) ||
                          (codepoint > 122 && codepoint < 127))) ||
                        (((inputmode == "allchars" && codepoint != 39) ||
                          inputmode == "allchars-admin") &&
                         ((codepoint > 160 && codepoint < 253) ||
                          (codepoint > 31  && codepoint < 128))))
                    {
                        bar->input32.insert(bar->input32.begin() + inputpos, (char32_t)codepoint);
                        inputpos++;
                    }
                }
                codepoint = GetCharPressed();
            }

            if ((!IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL)) &&
                !IsKeyDown(KEY_V) && stopCtrls)
                stopCtrls = 0;

            if (IsKeyDown(KEY_UP) && stopArrows > maxStopArrows &&
                logpos < (int)logCommands.size() && inputmode == "allchars-admin")
            {
                stopArrows = 0; beam = 0; logpos++;
                bar->input32 = UTF8ToUTF32(logCommands[logCommands.size() - logpos]);
                bar->input   = UTF32ToUTF8(bar->input32);
                inputpos     = bar->input32.size();
            }

            if (IsKeyDown(KEY_DOWN) && stopArrows > maxStopArrows &&
                logpos > 0 && inputmode == "allchars-admin")
            {
                stopArrows = 0; beam = 0; logpos--;
                if (logpos != 0)
                {
                    bar->input32 = UTF8ToUTF32(logCommands[logCommands.size() - logpos]);
                    bar->input   = UTF32ToUTF8(bar->input32);
                    inputpos     = bar->input32.size();
                }
                else { bar->input = ""; bar->input32 = U""; }
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && bar->status == 3)
            { inputpos = GetCursorFromMouseClick(bar, fsize); beam = 0; }

            if (IsKeyDown(269) || IsKeyDown(321)) { inputpos = (int)bar->input32.size(); beam = 0; }
            if (IsKeyDown(268) || IsKeyDown(327)) { inputpos = 0; beam = 0; }

            if (IsKeyDown(KEY_LEFT)      && inputpos > 0                          && stopArrows    > maxStopArrows)  { inputpos--;                                                     stopArrows    = 0; beam = 0; }
            if (IsKeyDown(KEY_RIGHT)     && inputpos < (int)bar->input32.size()   && stopArrows    > maxStopArrows)  { inputpos++;                                                     stopArrows    = 0; beam = 0; }
            if (IsKeyDown(KEY_BACKSPACE) && inputpos > 0                          && stopBackspace > maxBackspace)   { bar->input32.erase(bar->input32.begin() + inputpos - 1); inputpos--; stopBackspace = 0; beam = 0; }
            if (IsKeyDown(KEY_DELETE)    && inputpos < (int)bar->input32.size()   && stopBackspace > maxBackspace)   { bar->input32.erase(bar->input32.begin() + inputpos);             stopBackspace = 0; beam = 0; }

            bar->input = UTF32ToUTF8(bar->input32);

            beam++;
            if (beam <= maxBeam / 2)  showBeam = true;
            else if (beam > maxBeam)  beam = 0;
            else                      showBeam = false;
            if (stopBackspace <= maxBackspace) stopBackspace++;
            if (stopArrows    <= maxStopArrows) stopArrows++;
        }
        else  // boolean
        {
            int codepoint = GetCharPressed();
            if (codepoint == 48 || codepoint == 49) {bar->input = (char32_t)codepoint;}
            showBeam    = false;
            bar->status = isPressed(bar);
            if (bar->status == 3)
            {
                if (bar->input == "0") {bar->input = "1"; bar->input32 = U"1";}
                else                   {bar->input = "0"; bar->input32 = U"0";}
            }
        }
    }

    if (mode == "frontend")
    {
        if (bar->name != barAdminTerminalPtr->name) {PrettyDrawRectangle(bar);}

        if (inputmode != "boolean") {
            DrawTextEx(fontTtf, bar->input.c_str(),
                       (Vector2){(float)(bar->xloc * 1.04),
                                  (float)(((bar->ysize - fsize) / 2 + bar->yloc) - (bar->ysize * 0.05))},
                       fsize, 0, col);

            if (bar->status != 0 && showBeam && inputpos <= (int)bar->input32.size())
            {
                DrawLine(
                    (int)((bar->xloc * 1.04) + inputpos * fsize / 2),
                    (int)(bar->yloc + ((bar->ysize - fsize) / 4)),
                    (int)((bar->xloc * 1.04) + inputpos * fsize / 2),
                    (int)(bar->yloc + fsize * 1.6),
                    bar->name != barAdminTerminalPtr->name ? BLACK : WHITE);
            }
        }
        else
        {
            DrawTextEx(fontTtf, bar->input.c_str(),
                       (Vector2){(float)(((bar->xsize - fsize) / 2 + bar->xloc) + (fsize * 0.3)),
                                  (float)(((bar->ysize - fsize) / 2 + bar->yloc) - (fsize * 0.05))},
                       fsize, 0, col);
        }
    }

    return bar->input;
}
