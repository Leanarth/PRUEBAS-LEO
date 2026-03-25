#include "../globals.hpp"
#include "screen_adminmenu.hpp"
#include "../config/config.hpp"
#include "../db/database.hpp"
#include "../ui/input.hpp"
#include "../ui/drawing.hpp"
#include "../ui/objects.hpp"
#include "../reports/inform.hpp"
#include "../platform/pty.hpp"
#include "../platform/ipvalid.hpp"

void screenAdminmenuUpdate(Screen &currentScreen,
                           bool &restartExplorar,                   // Verifica si se tiene que reiniciar la pestaña "Explorar" para actualizar sus datos
                           bool &restartResultados,                 // Verifica si se tiene que reiniciar la pestaña "Resultados" para actualizar sus datos
                           bool &restartTerminal,                   // Verifica si se tiene que reiniciar la pestaña "Terminal" para volver a pedir las credenciales
                           bool &invalidIp,                         // Sirve para saber si la IP digitada en la pestaña "Terminal" es válida
                           bool &inputEmpty,                        // Verifica si las barras de entrada en la pestaña "Terminal" están vacías
                           bool &invalidCredentials,                // Verifica si las credenciales para la pestaña "Terminal" son válidas
                           bool &successfulPdfCreation,             // Verifica si la función inform() en ./reports/inform.cpp tuvo un resultado exitoso
                           bool &fromAdmin,                         // Verifica si se va a acceder a la pantalla CONFIGURATION desde el panel de administración
                           bool &rqst,                              // Verifica si hay que realizar otra vez una query/request al servidor de base de datos en la pestaña "Explorar"
                           bool &correctstudent,                    // Sirve para verificar el estudiante actual
                           std::string &explorarFinalOutput,        // Muestra la salida de los datos de la pestaña "Explorar"
                           std::string &strresultados,              // Almacena datos la pestaña "Resultados"
                           std::string &votes,                      // Almacena los votos cargados desde la base de datos pedidos desde la pestaña "Resultados"
                           std::string &outputTerm,                 // Muestra la salida de los datos de la pestaña "Terminal"
                           std::string &modeInput,                  // Especifica el modo de entrada de datos en las barras de entrada (los modos se encuentran en inputfunc())
                           std::string &outResultsMode,             // Especifica el modo de salida de los datos de la gráfica (si se desean porcentajes o cantidades)
                           int &opcSize)
{
    int conCnt = 0, agrCnt = 0, actCnt = 0, borCnt = 0, cnt = 0;    // Declara todos estos contadores a 0
    bool added = false;                                             // Declara a added, verifica si se armó la query de la pestaña "Actualizar" con los valores de la barra de entrad
    std::string agregarVal = "", agregarCol = "";                   // Estos strings se usarán para la pestaña "Agregar", servirán para guardar los valores y posteriormente usarse para armar la query a la base de datos

    // Detectar la interacción con algun botón o pestaña

    if (IsGestureDetected(GESTURE_TAP))                             // Si se llega a presionar la pantalla, se ejecutará el siguiente bloque de código
    {

        exitAdminPtr->status   = isPressed(exitAdminPtr);           // Se empieza a verificar el estado
        enterConfigPtr->status = isPressed(enterConfigPtr);         // de los botones de salir, actualizar datos
        refreshPtr->status     = isPressed(refreshPtr);             // o ingresar a la pantalla CONFIGURATION

        if (exitAdminPtr->status == 3 || enterConfigPtr->status == 3 || refreshPtr->status == 3)          // Si alguno de esos tres botones anteriores llega a recibir el estado 3 (fue presionado) se ejecutará el siguiente bloque de código
        {
            restartTerminal = true;                                                                       // Se activa el reinicio de las credenciales de la pestaña "Terminal"
            columnSelected  = columnsVec[0]->id;                                                          // La columna seleccionada ahora será la primera en el vector columnsVec
            adminSelected   = adminButtons[0]->name;                                                      // La pestaña seleccionada será la primera en el vector adminButtons ("Consultar") para aparecer en esta pestaña automáticamente si se vuelve a acceder

            for (int z = 0; z < (int)adminButtons.size(); z++) {adminButtons[z]->outLog = "";}            // Este bucle recorre el atributo outLog de cada pestaña, para que en caso de volver a ingresar al panel, se encuentre limpio

            if (exitAdminPtr->status == 3)                          // Si el botón presionado fue el de salir del panel de administración...
            {
                currentScreen = MAINMENU;                           // La pantalla actual ahora será MAINMENU
                cedulaBarPtr->status = 3;                           // El estado de la barra de entrada cedulaBar, pasará a estado 3, para automáticamente recibir datos de entrada
                return;                                             // Retorno de la función
            }
            else if (refreshPtr->status == 3)
            {
                updateData();                                       // En caso de que el botón presionado fue el de actualizar los datos, procederá a ejecutar updateData()
                objectCreation();                                   // Como se actualizaron los datos, existe la posibilidad de que botones, tablas o columnas ya no existan, entonces se vuelven a crear los objetos
                return;                                             // Retorno de la función
            }
            else                                                    // Si el botón presionado no fue ni el de salir ni el de actualizar, entonces significa que fue el de entrar a la configuración, así que procederá a ejecutar lo siguiente
            {
                fromAdmin = true;                                   // fromAdmin pasará a true, dando a entender al programa de que se quiso entrar a la pantalla de configuración desde el panel de administración
                loadConfig();                                       // loadConfig() procederá a cargar los datos que se pudieron cargar del archivo de configuración a las barras de entrada, para no tener que escribir toda la configuración de cero
                currentScreen = CONFIGURATION;                      // La pantalla actual ahora será CONFIGURATION
                return;                                             // Retorno de la función
            }
        }

        for (int i = 0; i < (int)adminButtons.size(); i++)          // Si el programa llega hasta esta línea, es por que no se tocó ningun botón, entonces procederá a recorrer las pestañas
        {
            adminButtons[i]->status = isPressed(adminButtons[i]);   // Verifica el estado de cada pestaña

            if (adminButtons[i]->status == 3)                       // Si la pestaña actual que está recorriendo el bucle tiene estado 3 (fue presionada), procederá a ejecutar el siguiente bloque de código...
            {
                add = 0;                                            // Declara a add igual a 0, es una variable que sirve para sobre el cuadro de las respuestas que reciben cada pestaña, más información en ./ui/drawing.cpp -> logfunction()
                adminSelected = adminButtons[i]->name;              // Declara a adminSelected igual al nombre de la pestaña seleccionada
                if      (adminSelected == butnames[4]) { restartExplorar  = true; rqst = true; }          // Si la pestaña presionada fue "Explorar" (butnames[4]) restartExplorar y rqst los declara true, para que actualice los datos
                else if (adminSelected == butnames[5]) { restartResultados = true; }                      // Si la pestaña presionada fue "Resultados" (butnames[5]) restartExplorar lo declara true, para que actualice los datos
                else if (adminSelected == butnames[6]) { tabRestart = true; }                             // Si la pestaña presionada fue "Terminal" (butnames[6]) tabRestart se pasa a true, no pasa como con las pestañas anteriores
                else { columnsVec[0]->status = 3; }                                                       // En caso de que ninguno de esos botones fue el presionado, procede a declarar a la primera columna del vector columnsVec como activa
            }

        }
    }

    // ── Pestañas Consultar / Agregar / Actualizar / Borrar ────────────────────

    if (adminSelected != butnames[4] && adminSelected != butnames[5] && adminSelected != butnames[6])     // Si la pestaña presionada no es "Explorar", "Resultados" ni "Terminal", entonces ejecutará este bloque hasta la línea 297 aprox
    {
        for (int co = 0; co < (int)columnsVec.size(); co++)         // A este bucle llamemoslo CICLO FOR PADRE, se encarga de recorrer absolutamente todas las columnas de todas las tablas
        {
            // Hay columnas en la base de datos que solo admiten valores booleanos/tinyint, esos valores son iguales a 1 o iguales a 0, en caso de que se presione la tecla SPACE, esos valores cambiarán, SPACE sirve como un atajo
            if (IsKeyPressed(KEY_SPACE) && columnsVec[co]->id == columnSelected &&          // Si se presiona la tecla space, y la columna actual en el bucle es igual a la columna seleccionada y...
                columnsVec[co]->type == "tinyint" && actBarPtr->status == 0)                // el tipo de datos de la columna actual es booleano y el estado de la barra de entrada de la barra de actualizaciones es igual a 0 (para prevenir conflictos)
              // Se verifica si la barra de actualizaciones (actBar) tiene un estado 0, por que si el administrador se encuentra en la pestaña de actualizaciones y toca la tecla SPACE pueden ocurrir errores inesperados
            {
                if (columnsVec[co]->input == "")       {columnsVec[co]->input = "0";}        // Procederá a llenar la input con un 0, si la input estaba vacía
                else if (columnsVec[co]->input == "0") {columnsVec[co]->input = "1";}        // Llenará la input con un 1, si la input tenía un 0
                else                                   {columnsVec[co]->input = "";}         // Si la input tenía un valor, procederá a vaciarlo
            }

            // Al presionar la tecla TAB, se puede pasar de una columna a otra columna, es muy útil

            if (IsKeyPressed(KEY_TAB))                    // Si se presiona la tecla TAB...
            {
                if (adminSelected != butnames[6])         // Si la pestaña actual NO es la pestaña "Terminal"...
                {
                    if (tabRestart)                                                         // tabRestart es true cuando se hace click a una columna, y por ende debe actualizar a tabCnt la nueva columna en uso, sino seguiría con un valor viejo
                    {
                        tabCnt = 0;                                                                                     // Se reiniciará el contador de los tabs
                        for (int clm = 0; clm < (int)columnsVec.size(); clm++)                                          // Este bucle, recorrerá todas las columnas disponibles para averiguar la nueva columna activa
                        {
                            if (columnsVec[clm]->fromTable == tableSelected) {tabCnt++;}                                // Si la columna actual es de la tabla seleccionada, aumentará el contador por uno
                            if (columnsVec[clm]->id == columnSelected) { tabCnt--; tabRestart = false; break; }         // Si la columna actual en el bucle es la columna seleccionada, resta uno a tabCnt ya que se sumará en el siguiente if
                        }
                    }
                    if (columnsVec[co]->id == columnSelected && tabCnt < quancolumns - 1)   // Se verifica que tabCnt no sea mayor que la cantidad de columnas totales, se le resta un valor a quancolumns por que en las líneas siguientes el valor de
                    {                                                                       // tabCnt se suma, y no debe sobrepasar a quancolumns
                        tabCnt++;                                         // Se le suma un valor a tabCnt, indicando la nueva posición de la columna actual
                        columnsVec[co]->status = 0;                       // Pone en estado 0 la columna actual del bucle for de la línea 93, la cual debe ser la que se estaba usando para escribir
                        columnsVec[co + 1]->status = 1;                   // Cambia a 1 la columna siguiente, ahora sí permite escribir, esto logra el cambio entre las columnas
                        columnSelected = columnsVec[co + 1]->id;          // Ahora nombra como columna seleccionada al id de la columna siguiente
                        beam = 0; break;                                  // beam es la variable para que el cursor parpadee, lo resetea a 0, y rompe el bucle for de la línea 93
                    }
                    else if (columnsVec[co]->id == columnSelected && tabCnt == quancolumns - 1)               // Si tabCnt es mayor o igual a quancolumns, significa que ya está en la última columan que aparece en pantalla, entonces...
                    {
                        columnsVec[co]->status = 0;                                                           // Pasa el estado de la columna actual en uso, proveniente del bucle for de la línea 93, a 0 (no permite escribir)
                        columnsVec[co + 1 - quancolumns]->status = 1;                                         // Pasa a estado 1 la primera columna en las columnas de la tabla actual
                        columnSelected = columnsVec[co + 1 - quancolumns]->id;                                // Ahora, la columna seleccionada es la primera de la tabla actual
                        beam = 0; tabCnt = 0; break;                                                          // Resetea a beam y rompe el bucle for de la línea 93
                    }
                }
            }

            // ── Consultar ────────────────────────────────────────────────────
            if (adminSelected == butnames[0])
            {
                if (columnsVec[co]->fromTable == tableSelected)
                {
                    if (IsKeyPressed(KEY_ENTER))
                    {
                        conCnt++;
                        adminButtons[0]->selfquery += columnsVec[co]->name + " REGEXP '"s + columnsVec[co]->input + "'"s;
                        if (conCnt < quancolumns) adminButtons[0]->selfquery += " AND ";
                        else
                        {
                            if (adminButtons[0]->outLog != "") adminButtons[0]->outLog += "\n";
                            adminButtons[0]->outLog += adminButtons[0]->selfquery + ";\n"s;
                            sendquery(adminButtons[0]->selfquery.data(), 0, 0, 1, " | ");
                            adminButtons[0]->outLog    += outQuery;
                            adminButtons[0]->selfquery  = "SELECT * FROM "s + tableSelected + " WHERE "s;
                            conCnt = 0;
                        }
                    }
                }
            }
            // ── Agregar ──────────────────────────────────────────────────────
            else if (adminSelected == butnames[1])
            {
                if (columnsVec[co]->fromTable == tableSelected)
                {
                    if (IsKeyPressed(KEY_ENTER))
                    {
                        if (columnsVec[co]->type == "tinyint" && columnsVec[co]->input.empty())
                            columnsVec[co]->input = "0";
                        if (agrCnt < quancolumns)
                        {
                            agrCnt++;
                            agregarCol += columnsVec[co]->name;
                            agregarVal += columnsVec[co]->input;
                            if (agrCnt != quancolumns) { agregarCol += ", "; agregarVal += "', '"; }
                        }
                        if (agrCnt == quancolumns)
                        {
                            agregarCol += ") VALUES ('";
                            agregarVal += "')";
                            adminButtons[1]->selfquery += agregarCol + agregarVal;
                            if (adminButtons[1]->outLog != "") adminButtons[1]->outLog += "\n";
                            adminButtons[1]->outLog    += adminButtons[1]->selfquery + ";\n"s;
                            sendquery(adminButtons[1]->selfquery.data(), 0, 0, 2);
                            adminButtons[1]->outLog    += outQuery;
                            adminButtons[1]->selfquery  = "INSERT INTO "s + tableSelected + " (";
                            agrCnt = 0;
                        }
                    }
                }
            }
            // ── Actualizar ───────────────────────────────────────────────────
            else if (adminSelected == butnames[2])                                                                          // Si la pestaña actual, es la de "Actualizar", se ejecutará este bloque de código hasta la línea 257 aprox
            {
                // Este if a continuación, sirve para que no se repita de nuevo el armado de estos datos a la query de "Actualizar", ya que hay que recordar que estamos en un bucle desde la línea 29 aprox, y no se deben repetir datos en la query

              if (IsKeyPressed(KEY_ENTER) && !added && opcSelectedPtr->name != opcionActPtr->name)                        // Si se presiona la tecla ENTER y NO se ha añadido este pedazo para la query a la base de datos (para eso !added)
                {
                    if (!((opcSelectedPtr->type == "tinyint" && actBarPtr->input.empty()) ||                                // Si el campo NO es uno de tipo tinyint vacío o... (apreciar el símbolo ! en todo el OR ||, el cual niega todo)
                          (opcSelectedPtr->type == "int"     && actBarPtr->input.empty())))                                 // Si el campo NO es uno de tipo int vacío
                    {
                        adminButtons[2]->selfquery += opcSelectedPtr->name + " = '"s + actBarPtr->input + "' WHERE "s;      // Se le añade a la query de la pestaña "Actualizar" el nombre de la columna a actualizar y lo que se ingresó a actBar
                        added = true;                                                                                       // added pasa a ser true
                    }
                }

                // Ahora, los objetos opcionAct y actBar deben ser actualizados, ya que no se les nombró en objectCreation() ciertos valores, estos valores se nombran dependiendo de los tamaños de las columnas de la tabla actual y sus nombres

                opcionActPtr->xsize = maxLenName;                                                       // opcionAct es el botón de la lista desplegable, su tamaño se calcula dependiendo del nombre más largo de las columnas de la tabla actual
                opcionActPtr->yloc  = screenHeight * 0.25 + (littleFontSize * 2) * quancolumns +        // La ubicación de la altura del botón se calcula dependiendo de la ubicación de la última columna
                                      screenHeight * 0.02 * quancolumns;
                actBarPtr->yloc     = opcionActPtr->yloc;                                               // actBar es la barra de entrada de la columna que se quiera actualizar, su ubicación de la altura será la misma a opcionAct
                actBarPtr->xloc     = opcionActPtr->xloc + opcionActPtr->xsize + screenWidth * 0.02;    // Su ubicación en el eje x depende de la ubicación en ancho del botón opcionAct, mas el ancho del botón, mas un espacio extra enre el botón

                if (columnsVec[co]->fromTable == tableSelected)                                         // Ahora, si recordamos que estamos en un bucle desde la línea 29, podemos usarlo para buscar cuales son las columnas de la tabla actual
                {
                    opcionActPtr->status = isPressed(opcionActPtr);                                     // Se verifica el estado de opcionAct
                    if (opcionActPtr->status != 0 && (int)opcionesAct.size() < quancolumns)             // En caso de que se llegue a presionar opcionAct, para abrir la lista desplegable, y el vector opcionesAct es menor a la cantidad de columnas...
                    {
                        cnt++;                                                                          // Aumenta el contador, se usa para que opcionesAct no se llene infinitamente
                        auto opc = std::make_unique<sqlobject>();                                       // Se declara un objeto temporal llamado opc, el cual servorá para guardar entre las opciones a la columna actual del ciclo for padre
                        opc->name   = columnsVec[co]->name;                                             // El nombre será el de la columna actual del ciclo for padre
                        opc->xloc   = opcionActPtr->xloc;                                               // Su ubicación en el ancho de la pantalla será la de opcionAct
                        opc->yloc   = opcionActPtr->yloc + opcionActPtr->ysize * cnt;                   // Su ubicación en la altura de la pantalla será la de opcionAct mas su tamaño en altura, multiplicado por cnt para que no se sobrepongan
                        opc->xsize  = opcionActPtr->xsize;                                              // Su ancho será el mismo que opcionAct
                        opc->ysize  = opcionActPtr->ysize;                                              // Su altura será la misma que opcionAct
                        opc->status = 0;                                                                // Su estado será 0, ya que no debe de haber recibido alguna interacción aún
                        opc->type   = columnsVec[co]->type;                                             // Su tipo de dato será el mismo que el de la columna correspondiente
                        opc->maxlen = std::stoi(columnsVec[co]->maxlen);                                // Su tamaño máximo de datos de entrada será el mismo que el de la columna correspondiente
                        opc->id     = columnsVec[co]->id;                                               // Su id será el mismo que el de la columna correspondiente
                        opcionesAct.push_back(opc.get());                                               // El objeto se empuja a opcionesAct
                        adminObj.push_back(std::move(opc));                                             // Su puntero se almacena en adminObj
                    }

                    // Ahora verifica si se presionó alguna opción de la lista desplegable, con opcionesActPtr->status == 0 significa que ya el botón que abrió la lista desplegable, sino que ahora se presionó alguna de sus opciones

                    else if (opcionActPtr->status == 0 && (int)opcionesAct.size() == quancolumns)       // Si las opciones aún no han sido vaciadas, osea de que opcionesAct aún sigue teniendo el tamaño de quancolumns
                    {
                        for (auto& opt : opcionesAct)                                                   // Por cada opción en el vector opcionesAct...
                        {
                            opt->status = isPressed(opt);                                               // Busca la opción que haya sido presionada
                            if (opt->status == 3)                                                       // Si la opción en el bucle actual fue presionada...
                            {
                                opcSize      = opcionesAct.size();                                      // opcSize almacenará la cantidad total de opciones en opcionesAct
                                opcSelectedPtr = opt;                                                   // La opción seleccionada será la que está usando el bucle ahora mismo
                                while (!opcionesAct.empty()) opcionesAct.pop_back();                    // Empezará a vaciar todo el vector con las opciones, pero esto no afecta a opcSelected por que opcSelected guardó a opt como una copia
                            }
                            actBarPtr->xsize = opcSelectedPtr->maxlen * littleFontSize;                 // El tamaño de actBar se calcula por medio del ancho de las letras de littleFontSize multiplicado por el tamaño máximo de datos de entrada
                        }
                    }
                    if (opcionActPtr->status == 0 && !opcionesAct.empty())                              // Si en realidad ninguna opción fue presionada, llegará a este if...
                        while (!opcionesAct.empty()) opcionesAct.pop_back();                            // Lo cual solo vaciará a opcionesAct

                    if (opcSelectedPtr->name != opcionActPtr->name)                               // Si la opción seleccionada NO es la opción vacía
                    {
                        std::cout<<opcSize<<"\n";
                        actBarPtr->status = isPressed(actBarPtr);                                 // Verifica el estado de la barra de entrada de la opción seleccionada
                        if      (opcSelectedPtr->type == "tinyint") {                             // Si el tipo de la columna es booleano/tinyint...
                          modeInput = "boolean";                                                  // modeInput será "boolean"
                          actBarPtr->ysize = littleFontSize * 2;                                  // ysize será un tamaño más pequeño, y
                          actBarPtr->xsize = littleFontSize * 2;                                  // xsize también, para que den la ilusión de que es una casila
                        }
                        else if (opcSelectedPtr->type == "int")     modeInput = "regexponly";     // Si el tipo de dato de la columna es "int", modeInput será "regexponly"
                        else if (opcSelectedPtr->type == "varchar") modeInput = "allchars";       // Si el tipo de dato es varchar, modeInput será "allchars"
                        if (actBarPtr->status != 0)                                               // Si el estado de la barra de datos de entrada NO es 0 (SÍ fue presionada)
                            actBarPtr->input = inputfunc("backend", actBarPtr,                    // Se ejecutará la función inputfunc() para recibir datos de entrada, a la barra actBar
                                                         opcSelectedPtr->maxlen, modeInput,       // El tamaño máximo de los datos de entrada serán los especificados por el atributo maxlen, y el modo de entrada será el especificado antes
                                                         littleFontSize, WHITE,                   // El tamaño del font será littleFontSize, y la letra será blanca, aunque no aplica por ser desde el backend
                                                         8*(opcSize-1), 120*(opcSize-1)*2, 8*(opcSize-1));    // Se usa opcSize para lidiar con bugs de muchas repeticiones por segundo de esta función al estar dentro del bucle for padre...
                                                         //littleFontSize, WHITE);                // si no me explico bien, descomente esta línea y comente las dos anteriores, vaya a la pestaña "Actualizar" y escriba/borre/muevase en actBar

                        if (IsKeyPressed(KEY_ENTER) && !actBarPtr->input.empty())                 // Si se presiona ENTER y actBar no se encuentra vacío...
                        {
                            actCnt++;               // actCnt empieza a sumarse
                            adminButtons[2]->selfquery += columnsVec[co]->name + " REGEXP '"s + columnsVec[co]->input + "'"s;
                            if (actCnt < quancolumns) adminButtons[2]->selfquery += " AND ";
                            else
                            {
                                if (adminButtons[2]->outLog != "") adminButtons[2]->outLog += "\n";
                                adminButtons[2]->outLog += adminButtons[2]->selfquery + ";\n"s;
                                if (!sendquery(adminButtons[2]->selfquery.data(), 0, 0, 2)) actBarPtr->input = "";
                                adminButtons[2]->outLog    += outQuery;
                                adminButtons[2]->selfquery  = "UPDATE "s + tableSelected + " SET "s;
                                actCnt = 0;
                            }
                        }
                    }
                    else { actBarPtr->input = ""; }
                }
            }
            // ── Borrar ───────────────────────────────────────────────────────
            else if (adminSelected == butnames[3])
            {
                if (columnsVec[co]->fromTable == tableSelected)
                {
                    if (IsKeyPressed(KEY_ENTER))
                    {
                        borCnt++;
                        adminButtons[3]->selfquery += columnsVec[co]->name + " REGEXP '"s + columnsVec[co]->input + "'"s;
                        if (borCnt < quancolumns) adminButtons[3]->selfquery += " AND ";
                        else
                        {
                            if (adminButtons[3]->outLog != "") adminButtons[3]->outLog += "\n";
                            adminButtons[3]->outLog    += adminButtons[3]->selfquery + ";\n"s;
                            sendquery(adminButtons[3]->selfquery.data(), 0, 0, 2);
                            adminButtons[3]->outLog    += outQuery;
                            adminButtons[3]->selfquery  = "DELETE FROM "s + tableSelected + " WHERE "s;
                            borCnt = 0;
                        }
                    }
                }
            }
        }
    }

    // ── Explorar ──────────────────────────────────────────────────────────────
    else if (adminSelected == butnames[4])
    {
        oldSelected = adminSelected;
        std::string line;
        std::vector<std::string> exploreList;
        for (int t = 0; t < (int)tablesVec.size(); t++)
        {std::cout<<tablesVec[t]->name<<"\t|\t"<<tablesVec[t]->status<<"\n";}
        for (int t = 0; t < (int)tablesVec.size(); t++)
        {
            tablesVec[t]->status = isPressed(tablesVec[t]);
            if (IsKeyPressed(KEY_TAB))
            {
                if (tablesVec[t]->status != 0 && t + 1 < (int)tablesVec.size())
                { tablesVec[t]->status = 0; tablesVec[t + 1]->status = 3; tableSelected = tablesVec[t + 1]->name; rqst = true; break; }
                else if (tablesVec[t]->status != 0 && t + 1 == (int)tablesVec.size())
                { tablesVec[t]->status = 0; tablesVec[0]->status = 3; tableSelected = tablesVec[0]->name; rqst = true; break; }
            }
            else if (IsGestureDetected(GESTURE_TAP))
            {
                if (tablesVec[t]->status != 3) {tablesVec[t]->status = 0;}
                else if (tablesVec[t]->status == 3) {tableSelected = tablesVec[t]->name; rqst = true;}
            }

        }
        if (restartExplorar || rqst)
        {
            if (restartExplorar)
            {
                for (auto& tab : tablesVec) tab->status = 0;
                tablesVec[0]->status = 3;
                tableSelected   = tablesVec[0]->name;
                restartExplorar = false;
                rqst = true;
            }
            if (rqst)
            {
                adminButtons[4]->selfquery = "SELECT * FROM "s + tableSelected;
                sendquery(adminButtons[4]->selfquery.data(), 0, 0, 1, " | ");
                adminButtons[4]->outLog = outQuery;
                rqst = false; add = 0;
            }
        }

        if (adminButtons[4]->outLog != "")
        {
            while (!exploreList.empty()) exploreList.pop_back();
            explorarFinalOutput = "";
            if (GetTouchX() > explorarSquare[0] && GetTouchX() < explorarSquare[0] + explorarSquare[2] &&
                GetTouchY() > explorarSquare[1] && GetTouchY() < explorarSquare[1] + explorarSquare[3])
                add += GetMouseWheelMove();

            double possibleNLexplore = (explorarSquare[3] - explorarSquare[1] * 0.1) / (littleFontSize + nlSpacing);
            if (possibleNLexplore - (int)possibleNLexplore > 0.5) possibleNLexplore = (int)possibleNLexplore + 1;
            else                                                   possibleNLexplore = (int)possibleNLexplore;

            for (int o = 0; o < (int)adminButtons[4]->outLog.size(); o++)
            {
                if (adminButtons[4]->outLog[o] != '\n') line += adminButtons[4]->outLog[o];
                else { line += "\n"; exploreList.push_back(line); line = ""; }
            }
            while ((int)exploreList.size() - add < possibleNLexplore) add -= 1;
            if (add < 0) add = 0;

            int nc = 0;
            for (int l = 0 + add; l < (int)exploreList.size(); l++)
            {
                if (add == 0) { if (nc > possibleNLexplore - 1) break; }
                else          { if (nc >= possibleNLexplore) break; }
                explorarFinalOutput += exploreList[l]; nc++;
            }
        }
        else { explorarFinalOutput = "NO HAY DATOS\n"; }
    }

    // ── Resultados ────────────────────────────────────────────────────────────
    else if (adminSelected == butnames[5])
    {
        oldSelected = adminSelected;
        if (restartResultados)
        {
            if (nullOption)
            {
                std::string q1 = "SELECT "s + *nameColumnVotosNombre + " FROM "s + *nameTablePartidos +
                                 " WHERE "s + *nameColumnPartidosNombre + " != '"s + *nameColumnNuloPartido + "';"s;
                sendquery(q1.data(), 0, 0);
                strresultados = outQuery;
                std::string q2 = "SELECT "s + *nameColumnVotosNombre + " FROM "s + *nameTablePartidos +
                                 " WHERE "s + *nameColumnPartidosNombre + " = '"s + *nameColumnNuloPartido + "';"s;
                sendquery(q2.data(), 0, 0);
                strresultados += outQuery;
            }
            else
            {
                std::string q = "SELECT "s + *nameColumnVotosNombre + " FROM "s + *nameTablePartidos;
                sendquery(q.data(), 0, 0);
                strresultados = outQuery;
            }
            std::string qAbs = "SELECT COUNT(*) FROM "s + *nameTableEstudiantes +
                               " WHERE "s + *nameColumnVotoNombre + " = '0';";
            sendquery(qAbs.data(), 0, 0);
            strresultados += outQuery;
            restartResultados = false;
        }

        for (int chr = 0; chr < (int)strresultados.length(); chr++)
        {
            if (strresultados[chr] != '\n') votes += strresultados[chr];
            else if (!votes.empty()) { percentages.push_back(std::stoi(votes)); votes = ""; }
        }

        resTogglePtr->status = isPressed(resTogglePtr);
        if (resTogglePtr->status == 3)
        {
            if ((std::string)outResultsMode == "quantity") { outResultsMode = "percentages"; resTogglePtr->name = "#"; }
            else                                           { outResultsMode = "quantity";    resTogglePtr->name = "%"; }
        }

        informePtr->status = isPressed(informePtr);
        if (informePtr->status == 3)
        {
            statistics("backend", "percentages", percentages, partidosVec);
            std::vector<int> quantities;
            for (int chr = 0; chr < (int)strresultados.length(); chr++)
            {
                if (strresultados[chr] != '\n') votes += strresultados[chr];
                else if (!votes.empty()) { quantities.push_back(std::stoi(votes)); votes = ""; }
            }
            successfulPdfCreation = inform(percentages, quantities);
        }
        statistics("backend", outResultsMode, percentages, partidosVec);
    }

    // ── Terminal ──────────────────────────────────────────────────────────────
    else if (adminSelected == butnames[6])
    {
        oldSelected = adminSelected;
        if (restartTerminal)
        {
            if (adminAuthenticated) intentosRestantes = 3;
            adminAuthenticated = false;
            adminButtons[6]->outLog = "";
            for (int b = 0; b < (int)termBars.size(); b++)
            { termBars[b]->input32 = U""; termBars[b]->input = ""; termBars[b]->status = 0; }
            termBars[0]->status = 3;
            restartTerminal = false;
            invalidIp = false; invalidCredentials = false; inputEmpty = false;
            return;
        }
        else if (adminAuthenticated)
        {
            adminTerminalPtr->status   = isPressed(adminTerminalPtr);
            barAdminTerminalPtr->status = isPressed(barAdminTerminalPtr);
            if (adminTerminalPtr->status != 0 || barAdminTerminalPtr->status != 0)
            {
                if (adminTerminalPtr->status != 0 && barAdminTerminalPtr->status == 0)
                { adminTerminalPtr->status = 0; barAdminTerminalPtr->status = 1; }
                inputfunc("backend", barAdminTerminalPtr, 1024, "allchars-admin", littleFontSize, WHITE, 6);
                if (IsKeyPressed(KEY_ENTER))
                {
                    outputTerm = ptyfunc(barAdminTerminalPtr->input,
                                         termBars[3]->input, termBars[4]->input,
                                         termBars[0]->input, termBars[1]->input,
                                         termBars[2]->input);
                    adminButtons[6]->outLog += barAdminTerminalPtr->input + "\n" + outputTerm;
                    logCommands.push_back(barAdminTerminalPtr->input);
                    barAdminTerminalPtr->input32 = U"";
                    barAdminTerminalPtr->input   = "";
                    logpos = 0;
                }
            }
        }
        else  // No autenticado — formulario de credenciales
        {
            for (int b = 0; b < (int)termBars.size(); b++)
            {
                if (IsKeyPressed(KEY_TAB))
                {
                    if (termBars[b]->status != 0 && b + 1 < (int)termBars.size())
                    { termBars[b]->status = 0; termBars[b + 1]->status = 1; beam = 0; break; }
                    else if (termBars[b]->status != 0 && b + 1 == (int)termBars.size())
                    { termBars[b]->status = 0; termBars[0]->status = 1; beam = 0; break; }
                }
                else termBars[b]->status = isPressed(termBars[b]);

                if (termBars[b]->status != 0)
                {
                    if (termBars[b]->name != termBars[1]->name)
                        inputfunc("backend", termBars[b], 45, "allchars", mediumFontSize, WHITE);
                    else
                        inputfunc("backend", termBars[b], 50, "numberonly", mediumFontSize, WHITE);
                }

                if (IsKeyPressed(KEY_ENTER))
                {
                    if (!termBars[0]->input.empty() && !termBars[1]->input.empty() &&
                        !termBars[2]->input.empty() && !termBars[3]->input.empty())
                    {
                        if (validIP(termBars[0]->input))
                        {
                            auth = mysql_init(NULL);
                            if (!mysql_real_connect(auth,
                                                    termBars[0]->input.data(), termBars[3]->input.data(),
                                                    termBars[4]->input.data(), termBars[2]->input.data(),
                                                    std::stoi(termBars[1]->input), NULL, 0))
                            {
                                mysql_close(auth); auth = nullptr;
                                intentosRestantes--;
                                invalidCredentials = true;
                                break;
                            }
                            else adminAuthenticated = true;
                        }
                        else invalidIp = true;
                    }
                    else inputEmpty = true;
                }
            }
        }
    }
}

// ── Frontend ──────────────────────────────────────────────────────────────────
void screenAdminmenuDraw(bool &invalidCredentials, bool &inputEmpty, bool &invalidIp,
                         bool &adminAuthenticated_local, bool &successfulPdfCreation,
                         const std::string &explorarFinalOutput, std::string& modeInput,
                         std::string &outResultsMode)
{
    drawSelected(adminButtons, littleFontSize, adminSelected);
    DrawRectangle(adminPanel[0], adminPanel[1] + terminalBarPtr->ysize - 1,
                  adminPanel[2], adminPanel[3], VOCADORADOSUAVE);
    DrawTextEx(fontTtf, "Panel de Administracion"s.data(),
               (Vector2){(float)centertext("Panel de Administracion"s, screenWidth, fontSize),
                          (float)(screenHeight * 0.05)},
               fontSize, 2, BLACK);

    if (adminSelected == butnames[0])
        oldSelected = drawcolumns(tablesVec, columnsVec, tableSelected, littleFontSize, adminSelected);
    else if (adminSelected == butnames[1])
        oldSelected = drawcolumns(tablesVec, columnsVec, tableSelected, littleFontSize, adminSelected);
    else if (adminSelected == butnames[2])
    {
        oldSelected = drawcolumns(tablesVec, columnsVec, tableSelected, littleFontSize, adminSelected);
        PrettyDrawRectangle(opcionActPtr);
        if (opcionActPtr->status != 0)
        {
            for (int counter = 0; counter < (int)opcionesAct.size(); counter++)
            {
                PrettyDrawRectangle(opcionesAct[counter]);
                DrawTextEx(fontTtf, opcionesAct[counter]->name.data(),
                           (Vector2){(float)(opcionActPtr->xloc + littleFontSize),
                                      (float)(opcionesAct[counter]->yloc + (opcionesAct[counter]->ysize * 0.5) - (littleFontSize * 0.5))},
                           littleFontSize, 2, BLACK);
            }
        }
        else
        {
            if (opcSelectedPtr->name == opcionActPtr->name)
            {
                DrawTextEx(fontTtf, opcionActPtr->name.data(),
                           (Vector2){(float)(opcionActPtr->xloc + littleFontSize),
                                      (float)(opcionActPtr->yloc + (opcionActPtr->ysize * 0.5) - (littleFontSize * 0.5))},
                           littleFontSize, 2, BLACK);
            }
            else
            {
                DrawTextEx(fontTtf, opcSelectedPtr->name.data(),
                           (Vector2){(float)(opcionActPtr->xloc + littleFontSize),
                                      (float)(opcionActPtr->yloc + (opcionActPtr->ysize * 0.5) - (littleFontSize * 0.5))},
                           littleFontSize, 2, BLACK);
                PrettyDrawRectangle(actBarPtr);
                if      (opcSelectedPtr->type == "tinyint") modeInput = "boolean";
                else if (opcSelectedPtr->type == "int")     modeInput = "regexponly";
                else if (opcSelectedPtr->type == "varchar") modeInput = "allchars";
                inputfunc("frontend", actBarPtr, opcSelectedPtr->maxlen, modeInput, littleFontSize);
            }
        }
    }
    else if (adminSelected == butnames[3])
        oldSelected = drawcolumns(tablesVec, columnsVec, tableSelected, littleFontSize, adminSelected);
    else if (adminSelected == butnames[4])
    {
        DrawRectangle(explorarSquare[0], explorarSquare[1], explorarSquare[2], explorarSquare[3],
                      Fade(VOCADORADO, 0.5f));
        drawSelected(tablesVec, littleFontSize, tableSelected);
        DrawTextEx(fontTtf, explorarFinalOutput.data(),
                   (Vector2){(float)(screenWidth * 0.13), (float)(explorarSquare[1] * 1.095)},
                   littleFontSize, 2, BLACK);
    }
    else if (adminSelected == butnames[5])
    {
        statistics("frontend", outResultsMode, percentages, partidosVec, screenWidth * 0.15, screenHeight * 0.5);
        PrettyDrawRectangle(resTogglePtr);
        DrawTextEx(fontTtf, resTogglePtr->name.data(),
                   (Vector2){(float)(resTogglePtr->xloc + littleFontSize / 2),
                              (float)(resTogglePtr->yloc + littleFontSize / 3)},
                   littleFontSize, 2, BLACK);
        PrettyDrawRectangle(informePtr);
        DrawTextEx(fontTtf, informePtr->name.data(),
                   (Vector2){(float)(informePtr->xloc + (informePtr->xsize * 0.5) - (informePtr->name.length() * littleFontSize) * 0.35),
                              (float)(informePtr->yloc + (informePtr->ysize * 0.5) - littleFontSize * 0.5)},
                   littleFontSize, 2, BLACK);
        if (pdfError)            shortmessage("ERROR: Ocurrio un error al crear el PDF", fontSize, pdfError);
        else if (pdfFontError)   shortmessage("ERROR: Ocurrio un error al cargar la fuente de texto", fontSize, pdfFontError);
        else if (pdfRandomError) shortmessage(pdfErrorString, fontSize, pdfRandomError);
        else if (successfulPdfCreation) shortmessage("PDF creado con exito!", fontSize, successfulPdfCreation);
    }
    else if (adminSelected == butnames[6])
    {
        if (adminAuthenticated)
        {
            PrettyDrawRectangle(adminTerminalPtr);
            PrettyDrawRectangle(barAdminTerminalPtr);
            inputfunc("frontend", barAdminTerminalPtr, 1024, "allchars-admin", littleFontSize, WHITE);
            logfunction(adminSelected);
        }
        else if (!adminAuthenticated && intentosRestantes > 0)
        {
            for (int b = 0; b < (int)termBars.size(); b++)
            {
                DrawTextEx(fontTtf, termBars[b]->name.data(),
                           (Vector2){(float)(screenWidth * 0.12),
                                      (float)(termBars[b]->yloc + (termBars[b]->ysize * 0.5) - (mediumFontSize * 0.5))},
                           mediumFontSize, 0, BLACK);
                PrettyDrawRectangle(termBars[b]);
                inputfunc("frontend", termBars[b], 0, "allchars", mediumFontSize);
            }
        }
        else
        {
            DrawTextEx(fontTtf, "Ha agotado sus intentos, retirese"s.data(),
                       (Vector2){(float)centertext("Ha agotado sus intentos, retirese"s, screenWidth, fontSize),
                                  (float)(screenHeight * 0.5)},
                       fontSize, 2, BLACK);
        }
        if (invalidCredentials)
            shortmessage("Credenciales invalidas, tiene "s + std::to_string(intentosRestantes) + " intentos restantes"s,
                         mediumFontSize, invalidCredentials);
        else if (inputEmpty) shortmessage("Los datos se encuentran vacios", fontSize, inputEmpty);
        else if (invalidIp)  shortmessage("La IP digitada es invalida",     fontSize, invalidIp);
    }

    PrettyDrawRectangle(enterConfigPtr);
    PrettyDrawRectangle(exitAdminPtr);
    PrettyDrawRectangle(refreshPtr);
}
