#pragma once
#include "../globals.hpp"
#include "../platform/encoding.hpp"
#include "../platform/clipboard.hpp"

// Forward declaration — body is in drawing.hpp
template<typename T> void PrettyDrawRectangle(T obj);

// ── isPressed ─────────────────────────────────────────────────────────────────
inline int isPressed(auto& obj)
{
    double xloc = obj->xloc, yloc = obj->yloc, xsize = obj->xsize, ysize = obj->ysize;
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
inline int GetCursorFromMouseClick(auto* bar, int fsize)
{
    float mouseX     = GetMouseX();
    float textStartX = bar->xloc * 1.04;

    if (mouseX <= textStartX) return 0;

    for (int i = 0; i <= (int)bar->input32.size(); i++)
    {
        std::u32string sub(bar->input32.begin(), bar->input32.begin() + i);
        std::string sub8 = UTF32ToUTF8(sub);
        Vector2 width = MeasureTextEx(fontTtf, sub8.c_str(), fsize, 0);
        if (mouseX < textStartX + width.x) return i;
    }
    return bar->input32.size();
}

// ── inputfunc ─────────────────────────────────────────────────────────────────
inline std::string inputfunc(std::string mode, auto& bar,
                             int maxlen = 21, std::string inputmode = "allchars",
                             double fsize = 0, Color col = BLACK,
                             int maxBackspace = 8, int maxBeam = 120, int maxStopArrows = 8)
{
    if (mode == "backend")
    {
        if (inputmode != "boolean")
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
            showBeam    = false;
            bar->status = isPressed(bar);
            if (bar->status == 3)
            {
                if (bar->input == "")       bar->input = "0";
                else if (bar->input == "0") bar->input = "1";
                else                        bar->input = "";
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
