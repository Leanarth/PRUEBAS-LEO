#include "../globals.hpp"
#include "screen_config.hpp"
#include "../platform/ipvalid.hpp"
#include "../config/config.hpp"
#include "../db/database.hpp"
#include "../ui/objects.hpp"
#include "../ui/input.hpp"
#include "../ui/drawing.hpp"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <string.h>

void screenConfigUpdate(Screen& currentScreen, bool& errorConfig, bool& errorUpdating,
                        bool& invalidIp, bool& inputEmpty, bool& fromAdmin)
{
    if (IsWindowResized()) loadConfig();

    if (IsGestureDetected(GESTURE_TAP))
    {
        // Seleccion de pestana de configuracion
        for (int i = 0; i < (int)configbuttons.size(); i++)
        {
            configbuttons[i]->status = isPressed(configbuttons[i]);
            if (configbuttons[i]->status == 4) configSelected = configbuttons[i]->name;
        }

        // Boton Guardar
        if (isPressed(saveConfigPtr) == 4)
        {
            int emptyValues = 0;
            for (int v = 0; v < (int)extraBars.size(); v++)
                if (extraBars[v]->input32.empty()) emptyValues++;
            for (int v = 0; v < (int)pathBars.size(); v++)
                if (pathBars[v]->input32.empty()) emptyValues++;

            if (!termBars[0]->input.empty() && !termBars[1]->input.empty() &&
                !termBars[2]->input.empty() && !termBars[3]->input.empty() &&
                !admPasswordBarPtr->input.empty() && emptyValues == 0)
            {
                if (validIP(termBars[0]->input))
                {
                    free(configurations[0]);  configurations[0]  = strdup(termBars[0]->input.data());         // IP
                    free(configurations[1]);  configurations[1]  = strdup(termBars[3]->input.data());         // Usuario
                    free(configurations[2]);  configurations[2]  = strdup(termBars[4]->input.data());         // Contrasena
                    free(configurations[3]);  configurations[3]  = strdup(termBars[2]->input.data());         // BD
                    free(configurations[4]);  configurations[4]  = strdup(termBars[1]->input.data());         // Puerto
                    free(configurations[5]);  configurations[5]  = strdup(admPasswordBarPtr->input.data());   // Contrasena admin
                    free(configurations[6]);  configurations[6]  = strdup(extraBars[0]->input.data());        // Tabla Estudiantes
                    free(configurations[7]);  configurations[7]  = strdup(extraBars[1]->input.data());        // Tabla Partidos
                    free(configurations[8]);  configurations[8]  = strdup(extraBars[5]->input.data());        // Columna Voto
                    free(configurations[9]);  configurations[9]  = strdup(extraBars[4]->input.data());        // Columna Votos
                    free(configurations[10]); configurations[10] = strdup(extraBars[2]->input.data());        // Columna Nombre partidos
                    free(configurations[11]); configurations[11] = strdup(extraBars[3]->input.data());        // Columna Nulo
                    free(configurations[12]); configurations[12] = strdup(pathBars[0]->input.data());         // Font programa
                    free(configurations[13]); configurations[13] = strdup(pathBars[1]->input.data());         // Font PDF
                    free(configurations[14]); configurations[14] = strdup(pathBars[2]->input.data());         // Nombre PDF
                }
                else { invalidIp = true; }
            }
            else { inputEmpty = true; }

            if (!invalidIp && !inputEmpty)
            {
                statusCodeUpdating = updateData();
                objectCreation();

                if (statusCodeUpdating == 0)
                {
                    fs::path pathToConfig = fs::current_path() / ".config";
                    std::ofstream cfgFile(pathToConfig);
                    std::string outCredentials =
                        "[Credentials]\nserver="s + *server + "\nuser="s + *user +
                        "\npassword="s + *password + "\ndatabase="s + *database +
                        "\nport="s + *port + "\nadmPassword="s + *admPassword + "\n"s;
                    std::string outExtra =
                        "\n[Extra]\nnameColumnPartidosNombre="s + *nameColumnPartidosNombre +
                        "\nnameColumnNuloPartido="s + *nameColumnNuloPartido +
                        "\nnameColumnVotosNombre="s + *nameColumnVotosNombre +
                        "\nnameColumnVotoNombre="s + *nameColumnVotoNombre +
                        "\nnameTableEstudiantes="s + *nameTableEstudiantes +
                        "\nnameTablePartidos="s + *nameTablePartidos + "\n"s;
                    std::string outPaths =
                        "\n[Paths]\npathProgramFont="s + *pathProgramFont +
                        "\npathPdfFont="s + *pathPdfFont +
                        "\ninformeName="s + *informeName + "\n"s;
                    cfgFile << outCredentials + outExtra + outPaths << std::endl;
                    cfgFile.close();
                    currentScreen = fromAdmin ? ADMINMENU : MAINMENU;
                    fromAdmin = false;
                }
                else
                {
                    errorUpdating = true;
                    fs::path pathToLogs = fs::current_path() / "logs.txt";
                    std::ofstream logFile(pathToLogs, std::ios::app);
                    logFile << "[ " << std::chrono::system_clock::now() << " ]  | >>> " << outQuery << "\n";
                    logFile.close();
                    loadConfig();
                }
            }
        }
    }

    // Procesamiento de barras de entrada segun pestaña activa
    if (configSelected == configbuttons[0]->name)   // Credenciales
    {
        for (int b = 0; b < (int)termBars.size(); b++)
        {
            if (IsKeyPressed(KEY_TAB))
            {
                if (termBars[b]->status > 1 && b + 1 < (int)termBars.size())
                { termBars[b]->status = 0; termBars[b + 1]->status = 2; beam = 0; break; }
                else if (termBars[b]->status > 1 && b + 1 == (int)termBars.size())
                { termBars[b]->status = 0; admPasswordBarPtr->status = 2; beam = 0; break; }
                else if (admPasswordBarPtr->status > 1)
                { termBars[0]->status = 2; admPasswordBarPtr->status = 0; beam = 0; break; }
            }
            else termBars[b]->status = isPressed(termBars[b]);

            if (termBars[b]->status > 1)
            {
                if (termBars[b]->name != termBars[1]->name)
                    inputfunc("backend", termBars[b], 45, "allchars", mediumFontSize, WHITE);
                else
                    inputfunc("backend", termBars[b], 45, "numberonly", mediumFontSize, WHITE);
            }
        }
        admPasswordBarPtr->status = isPressed(admPasswordBarPtr);
        if (admPasswordBarPtr->status > 1)
            inputfunc("backend", admPasswordBarPtr, 25, "allchars", mediumFontSize, WHITE);
    }
    else if (configSelected == configbuttons[1]->name)  // Extra
    {
        for (int b = 0; b < (int)extraBars.size(); b++)
        {
            if (IsKeyPressed(KEY_TAB))
            {
                if (extraBars[b]->status > 1 && b + 1 < (int)extraBars.size())
                { extraBars[b]->status = 0; extraBars[b + 1]->status = 2; beam = 0; break; }
                else if (extraBars[b]->status > 1 && b + 1 == (int)extraBars.size())
                { extraBars[b]->status = 0; extraBars[0]->status = 2; beam = 0; break; }
            }
            else extraBars[b]->status = isPressed(extraBars[b]);

            if (extraBars[b]->status > 1)
                inputfunc("backend", extraBars[b], 45, "allchars", littleFontSize, WHITE);
        }
    }
    else if (configSelected == configbuttons[2]->name)  // Paths
    {
        for (int b = 0; b < (int)pathBars.size(); b++)
        {
            if (IsKeyPressed(KEY_TAB))
            {
                if (pathBars[b]->status > 1 && b + 1 < (int)pathBars.size())
                { pathBars[b]->status = 0; pathBars[b + 1]->status = 2; beam = 0; break; }
                else if (pathBars[b]->status > 1 && b + 1 == (int)pathBars.size())
                { pathBars[b]->status = 0; pathBars[0]->status = 2; beam = 0; break; }
            }
            else pathBars[b]->status = isPressed(pathBars[b]);

            if (pathBars[b]->status > 1)
                inputfunc("backend", pathBars[b], 45, "allchars", mediumFontSize, WHITE);
        }
    }
}

void screenConfigDraw(bool &inputEmpty, bool &invalidIp, bool &errorUpdating, bool &errorConfig)
{
    DrawTextEx(fontTtf, "Panel de Configuracion"s.data(),
               (Vector2){(float)centertext("Panel de Configuracion"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.05)},
               fontSize, 2, BLACK);

    drawSelected(configbuttons, littleFontSize, configSelected);
    DrawRectangle(adminPanel[0], adminPanel[1] + terminalBarPtr->ysize - 1,
                  adminPanel[2], adminPanel[3], VOCADORADOSUAVE);

    if (configSelected == configbuttons[0]->name)
    {
        for (int b = 0; b < (int)termBars.size(); b++)
        {
            DrawTextEx(fontTtf, termBars[b]->name.data(),
                       (Vector2){(float)screenWidth * 0.15f,
                                  (float)termBars[b]->yloc + ((termBars[b]->ysize * 0.5f) - (mediumFontSize * 0.5f))},
                       mediumFontSize, 0, BLACK);
            PrettyDrawRectangle(termBars[b]);
            inputfunc("frontend", termBars[b], 0, "allchars", mediumFontSize);
        }
        DrawTextEx(fontTtf, admPasswordBarPtr->name.data(),
                   (Vector2){(float)screenWidth * 0.15f,
                              (float)admPasswordBarPtr->yloc + ((admPasswordBarPtr->ysize * 0.5f) - (mediumFontSize * 0.5f))},
                   mediumFontSize, 0, BLACK);
        PrettyDrawRectangle(admPasswordBarPtr);
        inputfunc("frontend", admPasswordBarPtr, 0, "allchars", mediumFontSize);
    }
    else if (configSelected == configbuttons[1]->name)
    {
        for (int b = 0; b < (int)extraBars.size(); b++)
        {
            DrawTextEx(fontTtf, extraBars[b]->name.data(),
                       (Vector2){(float)screenWidth * 0.15f,
                                  (float)extraBars[b]->yloc + ((extraBars[b]->ysize * 0.5f) - (littleFontSize * 0.5f))},
                       littleFontSize, 0, BLACK);
            PrettyDrawRectangle(extraBars[b]);
            inputfunc("frontend", extraBars[b], 0, "allchars", littleFontSize);
        }
    }
    else if (configSelected == configbuttons[2]->name)
    {
        for (int b = 0; b < (int)pathBars.size(); b++)
        {
            DrawTextEx(fontTtf, pathBars[b]->name.data(),
                       (Vector2){(float)screenWidth * 0.15f,
                                  (float)pathBars[b]->yloc + ((pathBars[b]->ysize * 0.5f) - (mediumFontSize * 0.5f))},
                       mediumFontSize, 0, BLACK);
            PrettyDrawRectangle(pathBars[b]);
            inputfunc("frontend", pathBars[b], 0, "allchars", mediumFontSize);
        }
    }

    PrettyDrawRectangle(saveConfigPtr);
    DrawTextEx(fontTtf, saveConfigPtr->name.data(),
               (Vector2){saveConfigPtr->xloc + (float)centertext(saveConfigPtr->name, saveConfigPtr->xsize, mediumFontSize),
                          saveConfigPtr->yloc + (float)((saveConfigPtr->ysize - mediumFontSize) / 2)},
               mediumFontSize, 0, BLACK);
    if (inputEmpty) {shortmessage("Los datos se encuentran vacios", fontSize, inputEmpty);}
    else if (invalidIp) {shortmessage("La IP digitada es invalida", fontSize, invalidIp);}
    else if (errorUpdating || errorConfig)
    {
        std::string errorMessage = "";
        if      (statusCodeUpdating == 127) errorMessage = "Error | La IP del servidor no existe";
        else if (statusCodeUpdating == 1)   errorMessage = "Error en la conexion a la base de datos";
        else if (statusCodeUpdating == 3)   errorMessage = "Error con el nombre de la base de datos";
        else if (statusCodeUpdating == 6)   errorMessage = "Error en el nombre de la tabla de los estudiantes";
        else if (statusCodeUpdating == 7)   errorMessage = "Error en el nombre de la tabla de los partidos";
        else if (statusCodeUpdating == 8)   errorMessage = "Error en el nombre de la columna de los votos de los estudiantes";
        else if (statusCodeUpdating == 9)   errorMessage = "Error en el nombre de la columna de los votos de los partidos";
        else if (statusCodeUpdating == 10)  errorMessage = "Error en el nombre de la columna que almacena el nombre de los partidos";
        else if (statusCodeUpdating == 11)  errorMessage = "Error en el nombre de la opcion a votar nulo";
        else if (statusCodeUpdating == 12)  errorMessage = "Error en la ruta de la fuente de la letra del programa";
        else if (statusCodeUpdating == 13)  errorMessage = "Error en la ruta de la fuente de la letra para el PDF";
        else if (statusCodeUpdating == 20)  errorMessage = "Error desconocido";
        else if (statusCodeConfig == 1)     errorMessage = "Error | No se encontro el archivo de configuracion";
        else if (statusCodeConfig == 2)     errorMessage = "Error | Faltaron parametros en el archivo de configuracion";
        if (errorConfig)   shortmessage(errorMessage, mediumFontSize, errorConfig, 450);
        else               shortmessage(errorMessage, mediumFontSize, errorUpdating, 450);
    }
}
