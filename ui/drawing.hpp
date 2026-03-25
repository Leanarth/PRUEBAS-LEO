#pragma once
#include "../globals.hpp"
#include "input.hpp"
#include <string>
#include <vector>

// ── Forward declarations ──────────────────────────────────────────────────────
int centertext(std::string message, double width, double fontsize);
std::string keepLastNLines(const std::string& text, int n = 1000);
// ── PrettyDrawRectangle ───────────────────────────────────────────────────────
template<typename T>
inline void PrettyDrawRectangle(T obj)
{
    DrawRectangleRounded({(float)obj->xloc, (float)obj->yloc,
                          (float)obj->xsize, (float)obj->ysize},
                         0.2f, 0, obj->status > 0 ? obj->highColor : obj->normalColor);
    DrawRectangleRoundedLinesEx({(float)obj->xloc, (float)obj->yloc,
                                 (float)obj->xsize, (float)obj->ysize},
                                0.2f, 0, 1.0f, Fade(BLACK, 0.3f));
}

// ── drawSelected ──────────────────────────────────────────────────────────────
template<typename V>
inline int drawSelected(V butVector, double fontsize, std::string selected)
{
    for (int i = 0; i < (int)butVector.size(); i++)
    {
        PrettyDrawRectangle(butVector[i]);
        float width = (butVector[i]->xloc + butVector[i]->xsize * 0.5f) * 2.0f;
        DrawTextEx(fontTtf, butVector[i]->name.data(),
                   (Vector2){(float)centertext(butVector[i]->name, width, fontsize),
                              (float)((butVector[i]->yloc + butVector[i]->ysize + butVector[i]->yloc) / 2.0 - fontsize / 2.0)},
                   fontsize, 2, BLACK);
    }
    return 0;
}

// ── Non-template declarations (bodies in drawing.cpp) ────────────────────────
int         logfunction(std::string selected, double lastColumnMeasures = 0, double fsize = 0);
std::string drawcolumns(std::vector<sqlobject*>& cTables, std::vector<column*>& cVector,
                        std::string& tSelected, double fsize,
                        std::string& selected, char* mode = (char*)"default");
int         shortmessage(std::string msg, double fs, bool& activator, int timeFps = 150);
std::vector<double> statistics(char* mode, std::string outputMode,
                               std::vector<double>& dataVec,
                               std::vector<sqlobject*> partVec,
                               int posx = 0, int posy = 0);
