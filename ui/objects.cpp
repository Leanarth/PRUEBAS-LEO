#include "../globals.hpp"
#include "objects.hpp"
#include "../db/database.hpp"
#include <memory>
#include <string>

int objectCreation()
{
    // Limpiar colecciones
    while (!adminObj.empty())    adminObj.pop_back();
    while (!tablesVec.empty())   tablesVec.pop_back();
    while (!partidosVec.empty()) partidosVec.pop_back();
    while (!columnsVec.empty())  columnsVec.pop_back();
    while (!adminButtons.empty())     adminButtons.pop_back();
    while (!configbuttons.empty()) configbuttons.pop_back();
    while (!termBars.empty())    termBars.pop_back();
    while (!extraBars.empty())   extraBars.pop_back();
    while (!pathBars.empty())    pathBars.pop_back();

    explorarSquare[0] = screenWidth  * 0.12;
    explorarSquare[1] = screenHeight * 0.24;
    explorarSquare[2] = screenWidth  * 0.76;
    explorarSquare[3] = screenHeight * 0.66;

    adminPanel[0] = 0.0;
    adminPanel[1] = screenHeight * 0.1;
    adminPanel[2] = screenWidth;
    adminPanel[3] = screenHeight * 0.9;

    butnames[0] = "Consultar";
    butnames[1] = "Agregar";
    butnames[2] = "Actualizar";
    butnames[3] = "Borrar";
    butnames[4] = "Explorar";
    butnames[5] = "Resultados";
    butnames[6] = "Terminal";

    // Botones de las pestanas del panel de administracion
    for (int i = 0; i < 7; i++)
    {
        auto btn = std::make_unique<button>();
        btn->name        = butnames[i];
        btn->xloc        = adminPanel[2] / 7.22 * i + (7.22 * i);
        btn->yloc        = adminPanel[1] + screenHeight * 0.005;
        btn->xsize       = adminPanel[2] / 7.22;
        btn->ysize       = adminPanel[3] * 0.05;
        btn->outLog      = "";
        btn->status      = 0;
        btn->highColor   = VOCADORADOSUAVE;
        btn->normalColor = VOCAAMARILLOSUAVE;
        adminButtons.push_back(btn.get());
        adminObj.push_back(std::move(btn));
    }

    // Botones de las pestanas de configuracion
    std::string confignames[] = {"Credenciales", "Extra", "Paths"};
    for (int i = 0; i < 3; i++)
    {
        auto btn = std::make_unique<button>();
        btn->name        = confignames[i];
        btn->xloc        = adminPanel[2] / 7.22 * (i + 2) + (7.22 * (i + 2));
        btn->yloc        = adminPanel[1];
        btn->xsize       = adminPanel[2] / 7.22;
        btn->ysize       = adminPanel[3] * 0.05;
        btn->outLog      = "";
        btn->status      = 0;
        configbuttons.push_back(btn.get());
        adminObj.push_back(std::move(btn));
    }

    // opcionAct — opcion seleccionada en la pestana "Actualizar"
    auto opcionAct = std::make_unique<button>();
    opcionAct->name   = "...";
    opcionAct->xloc   = screenWidth * 0.12;
    opcionAct->yloc   = 0;
    opcionAct->xsize  = 0;
    opcionAct->ysize  = adminPanel[3] * 0.05;
    opcionActPtr = opcionAct.get();
    adminObj.push_back(std::move(opcionAct));

    // cedula button
    auto cedula = std::make_unique<button>();
    cedula->name      = "Ingresar";
    cedula->xloc      = (screenWidth * 0.5) - ((screenWidth * 0.16) * 0.5);
    cedula->yloc      = screenHeight * 0.7;
    cedula->xsize     = screenWidth * 0.16;
    cedula->ysize     = screenHeight * 0.1;
    cedula->selfquery = "SELECT * FROM Estudiantes WHERE Cedula = '";
    cedulaPtr = cedula.get();
    adminObj.push_back(std::move(cedula));

    // cedulaBar
    auto cedulaBar = std::make_unique<inputBar>();
    cedulaBar->name    = "Barra de Cedula";
    cedulaBar->xloc    = (screenWidth * 0.5) - ((screenWidth * 0.5) / 2);
    cedulaBar->yloc    = screenHeight * 0.5;
    cedulaBar->xsize   = screenWidth * 0.50;
    cedulaBar->ysize   = fontSize * 2;
    cedulaBar->status  = 3;
    cedulaBar->input   = "";
    cedulaBar->input32 = U"";
    cedulaBarPtr = cedulaBar.get();
    adminObj.push_back(std::move(cedulaBar));

    // votar button
    auto votar = std::make_unique<button>();
    votar->name      = "Votar";
    votar->xloc      = cedulaPtr->xloc;
    votar->yloc      = screenHeight * 0.7;
    votar->xsize     = screenWidth * 0.16;
    votar->ysize     = screenHeight * 0.1;
    votar->status    = 0;
    votar->selfquery = "UPDATE Estudiantes SET Voto = '";
    votarPtr = votar.get();
    adminObj.push_back(std::move(votar));

    // continuar button
    auto continuar = std::make_unique<button>();
    continuar->name   = "Continuar";
    continuar->xloc   = cedulaPtr->xloc * 1.3;
    continuar->yloc   = screenHeight * 0.7;
    continuar->xsize  = screenWidth * 0.16;
    continuar->ysize  = screenHeight * 0.1;
    continuar->status = 0;
    continuarPtr = continuar.get();
    adminObj.push_back(std::move(continuar));

    // regresar button
    auto regresar = std::make_unique<button>();
    regresar->name   = "Regresar";
    regresar->xloc   = cedulaPtr->xloc * 0.7;
    regresar->yloc   = screenHeight * 0.7;
    regresar->xsize  = screenWidth * 0.16;
    regresar->ysize  = screenHeight * 0.1;
    regresar->status = 0;
    regresarPtr = regresar.get();
    adminObj.push_back(std::move(regresar));

    // exitAdmin button
    auto exitAdmin = std::make_unique<button>();
    exitAdmin->name   = "Salir";
    exitAdmin->xloc   = screenWidth * 0.02;
    exitAdmin->yloc   = screenHeight * 0.02;
    exitAdmin->xsize  = screenWidth * 0.02;
    exitAdmin->ysize  = screenWidth * 0.02;
    exitAdmin->status = 0;
    exitAdminPtr = exitAdmin.get();
    adminObj.push_back(std::move(exitAdmin));

    // enterConfig button
    auto enterConfig = std::make_unique<button>();
    enterConfig->name   = "Configuracion";
    enterConfig->xloc   = screenWidth - (screenWidth * 0.02 * 2);
    enterConfig->yloc   = screenHeight * 0.02;
    enterConfig->xsize  = screenWidth * 0.02;
    enterConfig->ysize  = screenWidth * 0.02;
    enterConfig->status = 0;
    enterConfigPtr = enterConfig.get();
    adminObj.push_back(std::move(enterConfig));

    // refresh button
    auto refresh = std::make_unique<button>();
    refresh->name   = "Refrescar";
    refresh->xloc   = screenWidth - (screenWidth * 0.02);
    refresh->yloc   = screenHeight * 0.96;
    refresh->xsize  = screenWidth * 0.02;
    refresh->ysize  = screenWidth * 0.02;
    refresh->status = 0;
    refreshPtr = refresh.get();
    adminObj.push_back(std::move(refresh));

    // resToggle button
    auto resToggle = std::make_unique<button>();
    resToggle->name   = "#";
    resToggle->xloc   = screenWidth * 0.14;
    resToggle->yloc   = screenHeight * 0.17;
    resToggle->xsize  = littleFontSize * 1.5;
    resToggle->ysize  = littleFontSize * 1.5;
    resToggle->status = 0;
    resTogglePtr = resToggle.get();
    adminObj.push_back(std::move(resToggle));

    // informe button
    auto informe = std::make_unique<button>();
    informe->name   = "Informe";
    informe->xloc   = screenWidth * 0.8;
    informe->yloc   = screenHeight * 0.4;
    informe->xsize  = littleFontSize * (double)informe->name.length();
    informe->ysize  = littleFontSize * 1.5;
    informe->status = 0;
    informePtr = informe.get();
    adminObj.push_back(std::move(informe));

    // saveConfig button
    auto saveConfig = std::make_unique<button>();
    saveConfig->name   = "Guardar";
    saveConfig->xloc   = screenWidth * 0.5 - ((littleFontSize * saveConfig->name.length()) / 2);
    saveConfig->yloc   = screenHeight * 0.85;
    saveConfig->xsize  = littleFontSize * saveConfig->name.length();
    saveConfig->ysize  = littleFontSize * 1.5;
    saveConfig->status = 0;
    saveConfigPtr = saveConfig.get();
    adminObj.push_back(std::move(saveConfig));

    // terminalBar (barra de entrada de comandos de terminal)
    auto terminalBar = std::make_unique<inputBar>();
    terminalBar->name        = "TerminalBar";
    terminalBar->xloc        = adminPanel[0];
    terminalBar->yloc        = (adminPanel[1] + adminPanel[1] * 0.4 + adminPanel[3] - adminPanel[3] * 0.05) - 1;
    terminalBar->xsize       = adminPanel[2];
    terminalBar->ysize       = 1 + (0.5 * (adminPanel[3] - (adminPanel[3] - adminPanel[3] * 0.1)));
    terminalBar->status      = 0;
    terminalBar->highColor   = BLACK;
    terminalBar->normalColor = BLACK;
    terminalBar->input       = "";
    terminalBar->input32     = U"";
    terminalBarPtr = terminalBar.get();
    adminObj.push_back(std::move(terminalBar));

    // Barras de credenciales del terminal de administrador
    std::string nameTermCred[] = {"IP de BD:", "Puerto de BD:", "Nombre de BD:",
                                  "Usuario de BD:", "Contrasena de BD:"};
    for (int b = 0; b < 5; b++)
    {
        auto credBar = std::make_unique<inputBar>();
        credBar->name    = nameTermCred[b];
        credBar->xloc    = (screenWidth * 0.15) + ((credBar->name.length() * mediumFontSize) / 2) + (screenWidth * 0.022);
        credBar->yloc    = screenHeight * 0.17 + ((screenHeight * 0.14) * b);
        credBar->xsize   = mediumFontSize * 30;
        credBar->ysize   = mediumFontSize * 2;
        credBar->status  = 0;
        credBar->input   = "";
        credBar->input32 = U"";
        termBars.push_back(credBar.get());
        adminObj.push_back(std::move(credBar));
    }

    // Barras de configuracion "Extra"
    std::string nameExtra[] = {
        "Nombre de la tabla 'Estudiantes':",
        "Nombre de la tabla 'Partidos':",
        "Nombre de la columna 'Nombre' de la tabla 'Partidos':",
        "Nombre de la opcion para votar nulo:",
        "Nombre de la columna 'Votos' de la tabla 'Partidos':",
        "Nombre de la columna 'Voto' de la tabla 'Estudiantes':"
    };
    for (int b = 0; b < 6; b++)
    {
        auto extraBar = std::make_unique<inputBar>();
        extraBar->name    = nameExtra[b];
        extraBar->xloc    = (screenWidth * 0.15) + ((extraBar->name.length() * littleFontSize) / 2) + (screenWidth * 0.022);
        extraBar->yloc    = screenHeight * 0.17 + ((screenHeight * 0.13) * b);
        extraBar->xsize   = littleFontSize * 30;
        extraBar->ysize   = littleFontSize * 2;
        extraBar->status  = 0;
        extraBar->input   = "";
        extraBar->input32 = U"";
        extraBars.push_back(extraBar.get());
        adminObj.push_back(std::move(extraBar));
    }

    // Barras de configuracion "Paths"
    std::string namePaths[] = {"Path del font del programa:",
                               "Path del font del informe PDF:",
                               "Nombre del PDF de salida:"};
    for (int b = 0; b < 3; b++)
    {
        auto pathBar = std::make_unique<inputBar>();
        pathBar->name    = namePaths[b];
        pathBar->xloc    = (screenWidth * 0.15) + ((pathBar->name.length() * mediumFontSize) / 2) + (screenWidth * 0.022);
        pathBar->yloc    = screenHeight * 0.17 + ((screenHeight * 0.17) * b);
        pathBar->xsize   = mediumFontSize * 30;
        pathBar->ysize   = mediumFontSize * 2;
        pathBar->status  = 0;
        pathBar->input   = "";
        pathBar->input32 = U"";
        pathBars.push_back(pathBar.get());
        adminObj.push_back(std::move(pathBar));
    }

    // admPasswordBar
    auto admPasswordBar = std::make_unique<inputBar>();
    admPasswordBar->name    = "Contrasena del panel de administracion:";
    admPasswordBar->xloc    = (screenWidth * 0.15) + ((admPasswordBar->name.length() * mediumFontSize) / 2) + (screenWidth * 0.022);
    admPasswordBar->yloc    = screenHeight * 0.17 + ((screenHeight * 0.14) * (int)termBars.size());
    admPasswordBar->xsize   = termBars[0]->xsize / 2;
    admPasswordBar->ysize   = termBars[0]->ysize;
    admPasswordBar->status  = 0;
    admPasswordBar->input   = "";
    admPasswordBar->input32 = U"";
    admPasswordBarPtr = admPasswordBar.get();
    adminObj.push_back(std::move(admPasswordBar));

    // adminTerminal (cuadro de salida de la terminal)
    auto adminTerminal = std::make_unique<inputBar>();
    adminTerminal->name        = "Cuadro de Terminal de Administrador";
    adminTerminal->xloc        = screenWidth * 0.12;
    adminTerminal->yloc        = screenHeight * 0.17;
    adminTerminal->xsize       = screenWidth * 0.76;
    adminTerminal->ysize       = screenHeight * 0.70;
    adminTerminal->status      = 0;
    adminTerminal->highColor   = BLACK;
    adminTerminal->normalColor = BLACK;
    adminTerminal->input       = "";
    adminTerminal->input32     = U"";
    adminTerminalPtr = adminTerminal.get();
    adminObj.push_back(std::move(adminTerminal));

    // barAdminTerminal (barra de entrada de comandos de la terminal de administrador)
    auto barAdminTerminal = std::make_unique<inputBar>();
    barAdminTerminal->name        = "Terminal de Administrador";
    barAdminTerminal->xloc        = adminTerminalPtr->xloc;
    barAdminTerminal->yloc        = adminTerminalPtr->yloc + adminTerminalPtr->ysize;
    barAdminTerminal->xsize       = adminTerminalPtr->xsize;
    barAdminTerminal->ysize       = littleFontSize * 2;
    barAdminTerminal->status      = 0;
    barAdminTerminal->highColor   = BLACK;
    barAdminTerminal->normalColor = BLACK;
    barAdminTerminal->input       = "";
    barAdminTerminal->input32     = U"";
    barAdminTerminalPtr = barAdminTerminal.get();
    adminObj.push_back(std::move(barAdminTerminal));

    // actBar (barra de la pestana "Actualizar")
    auto actBar = std::make_unique<inputBar>();
    actBar->xloc    = 0;
    actBar->yloc    = opcionActPtr->yloc;
    actBar->xsize   = 0;
    actBar->ysize   = adminPanel[3] * 0.05;
    actBar->status  = 0;
    actBar->input   = "";
    actBar->input32 = U"";
    actBarPtr = actBar.get();
    adminObj.push_back(std::move(actBar));

    // opcSelected
    auto opcSelected = std::make_unique<sqlobject>();
    opcSelected->name        = opcionActPtr->name;
    opcSelected->normalColor = VOCAVERDE;
    opcSelected->highColor   = VOCADORADO;
    opcSelectedPtr = opcSelected.get();
    adminObj.push_back(std::move(opcSelected));

    adminSelected  = butnames[0];
    configSelected = "Credenciales";

    if (statusCodeUpdating == 0)
    {
        partidosVec.reserve(quanpartidos);
        tablesVec.reserve(quantables);
        columnsVec.reserve(quancolumns);
        percentages.reserve(quanpartidos);

        // Tablas
        for (int i = 0; i < quantables; i++)
        {
            auto table = std::make_unique<sqlobject>();
            table->name        = nametables[i];
            table->xloc        = (screenWidth * 0.12) + (screenWidth * 0.1 * i) + (screenWidth * 0.02 * i);
            table->yloc        = screenHeight * 0.17;
            table->xsize       = screenWidth * 0.1;
            table->ysize       = screenHeight * 0.05;
            table->status      = 0;
            table->normalColor = VOCAVERDESUAVE;
            table->highColor   = VOCAVERDE;
            tablesVec.push_back(table.get());
            adminObj.push_back(std::move(table));
        }

        tableSelected = tablesVec[0]->name;

        // Columnas
        int identifier = 0, typed = 0, named = 0;
        column* namecolumnPtr = nullptr;
        std::string typecolumn    = "";
        std::string varNameColumn = "";
        std::string maxlencolumn  = "";
        std::string strnamecolumns = "";
        std::string queryForColumns = "DESC ";

        for (int table = 0; table < (int)nametables.size(); table++)
        {
            queryForColumns += nametables[table];
            if (sendquery(queryForColumns.data(), 0, 4)) return 20;
            if (outQuery.empty()) return 20;
            strnamecolumns = outQuery;

            for (int c = 0; c < (int)strnamecolumns.size(); c++)
            {
                if (strnamecolumns[c] != '\n')
                {
                    if (strnamecolumns[c] != ' ' && !named)
                        varNameColumn.push_back(strnamecolumns[c]);
                    else if (strnamecolumns[c] == ' ' && !named)
                    {
                        auto namecolumn = std::make_unique<column>();
                        namecolumn->name      = varNameColumn;
                        namecolumn->fromTable = nametables[table];
                        named = 1;
                        namecolumn->status  = 0;
                        namecolumn->id      = ++identifier;
                        namecolumn->type    = "";
                        namecolumn->maxlen  = "";
                        namecolumn->input   = "";
                        namecolumn->input32 = U"";
                        namecolumnPtr = namecolumn.get();
                        columnsVec.push_back(namecolumn.get());
                        adminObj.push_back(std::move(namecolumn));
                    }
                    else if (strnamecolumns[c] != '(' && !typed)
                        typecolumn.push_back(strnamecolumns[c]);
                    else if (strnamecolumns[c] == '(' && !typed)
                    {
                        namecolumnPtr->type += typecolumn;
                        typed = 1;
                    }
                    else if (strnamecolumns[c] != ')' && typed)
                        maxlencolumn.push_back(strnamecolumns[c]);
                    else if (strnamecolumns[c] == ')' && typed)
                    {
                        namecolumnPtr->maxlen += maxlencolumn;
                        named = 0; typed = 0;
                        varNameColumn = ""; typecolumn = ""; maxlencolumn = "";
                    }
                }
            }
            strnamecolumns = ""; queryForColumns = "DESC ";
        }

        // Partidos
        for (int i = 0; i < quanpartidos; i++)
        {
            auto part = std::make_unique<sqlobject>();
            part->name   = namepartidos[i];
            part->xloc   = ((screenWidth / quanpartidos) * i) + (screenWidth / (quanpartidos * 2) / 2);
            part->yloc   = (screenHeight * 0.6) * 0.5;
            part->xsize  = screenWidth / (quanpartidos * 2);
            part->ysize  = screenHeight * 0.1;
            part->status = 0;
            partidosVec.push_back(part.get());
            adminObj.push_back(std::move(part));
        }

        // Opcion nula (si existe)
        if (nullOption)
        {
            auto nullPartido = std::make_unique<sqlobject>();
            nullPartido->name   = *nameColumnNuloPartido;
            nullPartido->xloc   = votarPtr->xloc;
            nullPartido->yloc   = votarPtr->yloc * 0.8;
            nullPartido->xsize  = votarPtr->xsize;
            nullPartido->ysize  = votarPtr->ysize;
            nullPartido->status = 0;
            partidosVec.push_back(nullPartido.get());
            adminObj.push_back(std::move(nullPartido));
        }

        // sinVotar (para estadisticas)
        auto sinVotar = std::make_unique<sqlobject>();
        sinVotar->name   = "ABS.";
        sinVotar->xloc   = screenWidth  * 2;
        sinVotar->yloc   = screenHeight * 2;
        sinVotar->xsize  = 0;
        sinVotar->ysize  = 0;
        sinVotar->status = 0;
        sinVotarPtr = sinVotar.get();
        partidosVec.push_back(sinVotar.get());
        adminObj.push_back(std::move(sinVotar));
    }

    return 0;
}
