// PrettyDrawRectangle and drawSelected bodies live in drawing.hpp (template functions).
// This file contains only the non-template drawing utilities.
#include "drawing.hpp"
#include <string>
#include <vector>

int centertext(std::string message, double width, double fontsize)
{
    Vector2 textWidth = MeasureTextEx(fontTtf, message.data(), fontsize, 2);
    return (int)((width - textWidth.x) / 2);
}

std::string adjustLimit(const std::string &text, int n = 1000) {
    int count = 0;
    size_t pos = text.size();

    while (pos > 0) {
        pos = text.rfind('\n', pos - 1);
        if (pos == std::string::npos) break;
        
        count++;
        if (count == n) {
            return text.substr(pos + 1); //                             +1 para excluir el '\n'
        }
    }

    return text;          // Si tiene menos de n líneas, retorna todo
}

int logfunction(std::string selected, double lastColumnMeasures, double fsize)
{
    std::vector<std::string> logList;
    std::string line = "";
    std::string log  = "";
    int modifiedFontSize = littleFontSize + (1 - littleFontSize) * 0.4;

    if (selected == adminButtons[2]->name) lastColumnMeasures += actBarPtr->ysize + (fsize * 2);

    if (selected != adminButtons[6]->name || lastColumnMeasures != 0)
    {
        outSquare[0] = (screenWidth * 0.5) - ((screenWidth * 0.76) * 0.5);
        outSquare[1] = lastColumnMeasures;
        outSquare[2] = screenWidth * 0.76;
        outSquare[3] = screenHeight - lastColumnMeasures - screenHeight * 0.04;
        DrawRectangle(outSquare[0], outSquare[1], outSquare[2], outSquare[3], BLACK);
    }
    else
    {
        outSquare[0] = adminTerminalPtr->xloc;
        outSquare[1] = adminTerminalPtr->yloc;
        outSquare[2] = adminTerminalPtr->xsize;
        outSquare[3] = adminTerminalPtr->ysize;
    }

    int cnt = 0, ch = 0, possibleChars = 0;
    while (ch < outSquare[2] - (outSquare[2] * 0.02))
    { possibleChars++; ch = (int)(possibleChars * modifiedFontSize); }

    double possibleNL = (outSquare[3] - (outSquare[3] * 0.05)) / (littleFontSize + nlSpacing);
    if (possibleNL - (int)possibleNL > 0.5) possibleNL = (int)possibleNL + 1;
    else                                    possibleNL = (int)possibleNL;

    if (GetMouseWheelMove() != 0 &&
        GetTouchX() > outSquare[0] && GetTouchX() < outSquare[0] + outSquare[2] &&
        GetTouchY() > outSquare[1] && GetTouchY() < outSquare[1] + outSquare[3])
        add += GetMouseWheelMove();

    logList.reserve(possibleNL);

    for (int opt = 0; opt < (int)adminButtons.size(); opt++)
    {
        if (selected == adminButtons[opt]->name && adminButtons[opt]->outLog != "")
        {
            int cntNL = 0;
            for (char c : adminButtons[opt]->outLog) {
                if (c == '\n') {
                    cntNL++;
                }
            }
            if (cntNL > 1000) {adminButtons[opt]->outLog = adjustLimit(adminButtons[opt]->outLog, 1000);}

            for (ch = 0; ch < (int)adminButtons[opt]->outLog.size(); ch++)
            {
                cnt++;
                if (cnt <= possibleChars && adminButtons[opt]->outLog[ch] == '\n') cnt = 0;
                else if (cnt >= possibleChars && adminButtons[opt]->outLog[ch] != '\n')
                { adminButtons[opt]->outLog.insert(ch, "\n"); cnt = 0; break; }
            }
            for (ch = 0; ch < (int)adminButtons[opt]->outLog.size(); ch++)
            {
                if (adminButtons[opt]->outLog[ch] != '\n') line += adminButtons[opt]->outLog[ch];
                else { line += "\n"; logList.insert(logList.begin(), line); line = ""; }
            }
            while ((int)logList.size() - add < possibleNL) add -= 1;
            if (add < 0) add = 0;

            int countr = 0;
            for (int ln = 0 + add; ln < (int)logList.size(); ln++)
            {
                if (countr >= (int)possibleNL) break;
                log.insert(0, logList[ln]);
                countr++;
            }
            DrawTextEx(fontTtf, log.data(),
                       (Vector2){(float)(outSquare[0] + screenWidth * 0.01),
                                  (float)(outSquare[1] + outSquare[3] * 0.05)},
                       littleFontSize, 2, WHITE);
        }
    }
    return 0;
}

std::string drawcolumns(std::vector<sqlobject*>& cTables, std::vector<column*>& cVector,
                        std::string& tSelected, double fsize,
                        std::string& selected, char* mode)
{
    quancolumns = 0;
    int lastColumnMeasures = 0;
    std::string nameModified;
    bool changed  = false;
    std::string modeInput = "regexponly";
    int oldMax = 0, newMax = 0;
    int oldMaxBar = 0, newMaxBar = 0;

    if (oldSelected != selected || oldTableSelected != tSelected)
    {
        for (int number = 0; number < (int)cVector.size(); number++)
        {
            cVector[number]->input   = "";
            cVector[number]->input32 = U"";
            cVector[number]->status  = 0;
            changed = true;
        }
        tabCnt = 0;
        opcSelectedPtr->name  = opcionActPtr->name;
        oldSelected      = selected;
        oldTableSelected = tSelected;
        adminButtons[0]->selfquery = "SELECT * FROM "s + tSelected + " WHERE ";
        adminButtons[1]->selfquery = "INSERT INTO "s  + tSelected + " (";
        adminButtons[2]->selfquery = "UPDATE "s       + tSelected + " SET ";
        adminButtons[3]->selfquery = "DELETE FROM "s  + tSelected + " WHERE ";
    }

    if ((std::string)mode == "default")
    {
        for (int numberTable = 0; numberTable < (int)cTables.size(); numberTable++)
        {
            cTables[numberTable]->status = isPressed(cTables[numberTable]);
            if (cTables[numberTable]->status == 3) tSelected = cTables[numberTable]->name;
        }
        drawSelected(cTables, fsize, tSelected);
    }

    for (int number = 0; number < (int)cVector.size(); number++)
    {
        if (cVector[number]->fromTable == tSelected)
        {
            cVector[number]->xloc = (screenWidth * 0.12) +
                                    ((int)cVector[number]->name.size() * fsize) -
                                    ((int)cVector[number]->name.size() * (fsize * 0.41)) +
                                    (screenWidth * 0.018);
            newMax = (int)cVector[number]->name.size() * fsize;

            cVector[number]->yloc = screenHeight * 0.25 +
                                    ((fsize * 2) * quancolumns) +
                                    (screenHeight * 0.02 * quancolumns);

            if      (cVector[number]->type == "varchar") { modeInput = "allchars";   cVector[number]->xsize = fsize * std::stoi(cVector[number]->maxlen); }
            else if (cVector[number]->type == "int")     { modeInput = "regexponly"; cVector[number]->xsize = fsize * std::stoi(cVector[number]->maxlen); }
            else if (cVector[number]->type == "tinyint") { modeInput = "boolean";    cVector[number]->xsize = fsize * 2; }

            newMaxBar = cVector[number]->xsize;
            cVector[number]->ysize = fsize * 2;

            if ((std::string)mode == "default")
            {
                nameModified = cVector[number]->name + ":";
                DrawTextEx(fontTtf, nameModified.data(),
                           (Vector2){(float)(screenWidth * 0.12),
                                      (float)(cVector[number]->yloc + (cVector[number]->ysize * 0.5) - (fsize * 0.5))},
                           fsize, 2, BLACK);
                PrettyDrawRectangle(cVector[number]);
                cVector[number]->status = isPressed(cVector[number]);
                if (cVector[number]->status != 0)
                {
                    if (cVector[number]->status == 3) tabRestart = true;
                    cVector[number]->input = inputfunc("backend", cVector[number],
                                                       std::stoi(cVector[number]->maxlen),
                                                       modeInput, fsize, BLACK);
                    columnSelected = cVector[number]->id;
                }
                inputfunc("frontend", cVector[number], 0, modeInput, fsize);
            }

            quancolumns++;
            if (newMax    > oldMax)    oldMax    = newMax;
            if (newMaxBar > oldMaxBar) oldMaxBar = newMaxBar;
            lastColumnMeasures = (int)(cVector[number]->yloc + cVector[number]->ysize + (fsize * 2));
        }
    }

    maxLenName   = oldMax;
    maxBarLenght = oldMaxBar;

    if (changed)
    {
        for (int number = 0; number < (int)cVector.size(); number++)
        {
            if (cVector[number]->fromTable == tSelected) { cVector[number]->status = 3; break; }
        }
    }
    if (lastColumnMeasures != 0) logfunction(selected, lastColumnMeasures, fsize);
    return oldSelected;
}

int shortmessage(std::string msg, double fs, bool& activator, int timeFps)
{
    if (framesCounter < timeFps)
    {
        framesCounter++;
        double w    = msg.length() * fs;
        double r[4] = {(screenWidth * 0.5) - (w / 2),
                       (screenHeight * 0.5) - ((screenHeight * 0.3) / 2),
                       w, screenHeight * 0.3};
        DrawRectangle(r[0], r[1], r[2], r[3], WHITE);
        DrawRectangleLines(r[0], r[1], r[2], r[3], BLACK);
        DrawTextEx(fontTtf, msg.data(),
                   (Vector2){(float)(r[0] + centertext(msg, r[2], fontSize)),
                              (float)(r[1] + (r[3] - fontSize) / 2)},
                   fontSize, 2, BLACK);
    }
    else { activator = false; framesCounter = 0; }
    return 0;
}

std::vector<double> statistics(char* mode, std::string outputMode,
                               std::vector<double>& dataVec,
                               std::vector<sqlobject*> partVec,
                               int posx, int posy)
{
    if ((std::string)mode == "backend")
    {
        if ((std::string)outputMode == "percentages")
        {
            int total = 0;
            for (int item = 0; item < (int)dataVec.size(); item++)
            {
                total += dataVec[item];
                if (item + 1 == (int)dataVec.size())
                {
                    for (int n = 0; n < (int)dataVec.size(); n++)
                    {
                        if (dataVec[dataVec.size() - 1] <= 0)
                        { dataVec.insert(dataVec.begin(), 0); dataVec.pop_back(); }
                        else
                        { dataVec.insert(dataVec.begin(), dataVec[dataVec.size() - 1] * 100.0 / total); dataVec.pop_back(); }
                    }
                }
            }
        }
        return dataVec;
    }
    else
    {
        char* numberOutput = (char*)"";
        float maxPartName  = 0;
        for (int item = 0; item < (int)dataVec.size(); item++)
        {
            if ((int)partVec[item]->name.length() > maxPartName)
                maxPartName = partVec[item]->name.length();
            else if ((int)std::to_string(dataVec[item]).length() - 4 > maxPartName)
                maxPartName = std::to_string(dataVec[item]).length();
        }

        DrawRectangle(posx - screenWidth * 0.02,
                      posy - screenHeight * 0.003 * 100 - screenHeight * 0.02,
                      (screenWidth * 0.04 * dataVec.size()) +
                      (maxPartName * ((littleFontSize / 2) * dataVec.size())) + screenWidth * 0.02,
                      screenHeight * 0.003 * 100 + (littleFontSize * 3) + screenHeight * 0.04,
                      Fade(VOCADORADO, 0.5f));

        for (int item = 0; item < (int)dataVec.size(); item++)
        {
            if ((std::string)outputMode == "percentages")
            {
                DrawRectangle(posx + (screenWidth * 0.04 * item) + (maxPartName * (littleFontSize / 2) * item),
                              posy - screenHeight * 0.003 * dataVec[item],
                              screenWidth * 0.04,
                              screenHeight * 0.003 * dataVec[item], VOCADORADO);
            }
            else
            {
                if (dataVec[item] != 0)
                {
                    double studentsVoted  = dataVec[item] / quanstudents;
                    double maximumSizeBar = screenHeight * 0.3;
                    DrawRectangle(posx + (screenWidth * 0.04 * item) + (maxPartName * (littleFontSize / 2) * item),
                                  posy - maximumSizeBar * studentsVoted,
                                  screenWidth * 0.04,
                                  maximumSizeBar * studentsVoted, BLACK);
                }
            }

            std::string outputRounded = "";
            std::string floatString   = std::to_string(dataVec[item]);
            int count = 10;
            for (int digit = 0; digit < (int)floatString.size(); digit++)
            {
                if (count == 0) break;
                if (floatString[digit] == '.')
                {
                    if ((std::string)outputMode == "percentages") count = 3;
                    else break;
                }
                outputRounded += floatString[digit];
                count--;
                numberOutput = outputRounded.data();
            }

            DrawTextEx(fontTtf, numberOutput,
                       (Vector2){(float)(posx + screenWidth * 0.04 * item + maxPartName * (littleFontSize / 2) * item),
                                  (float)(posy + littleFontSize)},
                       littleFontSize, 2, BLACK);
            DrawTextEx(fontTtf, partVec[item]->name.data(),
                       (Vector2){(float)(posx + screenWidth * 0.04 * item + maxPartName * (littleFontSize / 2) * item),
                                  (float)(posy + littleFontSize * 2)},
                       littleFontSize, 2, BLACK);
        }

        while (!dataVec.empty()) dataVec.pop_back();
        return dataVec;
    }
}
