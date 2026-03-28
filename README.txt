Primero que nada, para poder ejecutar el programa, actualmente hay 2 compiladores automáticos:

./compiler.sh: Se encarga de específicamente compilar el programa en el entorno de Linux

./exe_compiler.sh: Se encarga de compilar específicamente un ejecutable para windows, osea un archivo .exe, pero desde linux, además de cargar sus dependencias y archivos importantes, se comprime todo en un rar para que solo deba descomprimirse y ejecutarse

exe_compiler tiene 2 dependencias muy importantes para poder funcionar, tener actualmente mingw y rar instalado en linux

La manera más fácil de compilar el código es a través de compiler.sh

Ahora sí, para entender mejor el código, esta vez, está organizado de la siguiente manera:

POR FAVOR INTENTAR NO USAR LIBRERÍAS EXTRA EN EL CÓDIGO, O LIBRERÍAS POCO CONOCIDAS

── config/ ───────────────────────────────────────────────────────────────────
Esta carpeta lo que contiene son las funciones relacionadas a la configuración del programa, como la carga de los valores del archivo de configuración, el
cual es un archivo oculto en la carpeta, llamado ".config", el cual tiene este formato:

+-------------------------------------------------------+
|  [Credentials]                                        >-+
|  server=127.0.0.0                                       |
|  user=root                                              |
|  password=0th3r@N0n                                     +---- Todo esto se encarga de las credenciales para la conexión de la base de datos
|  database=Votaciones                                    |
|  port=3306                                              |
|  admPassword=1234                                     >-+
|
|  [Extra]
|  nameColumnPartidosNombre=Nombre                      >-+
|  nameColumnNuloPartido=NULO                             |
|  nameColumnVotosNombre=Votos                            +---- Esto se encarga de valores de la base de datos como columnas o tablas, que si por ejemplo la tabla
|  nameColumnVotoNombre=Voto                              |     "Estudiantes" llega a cambiar de nombre, se le indique en el archivo de configuración su nuevo nombre
|  nameTableEstudiantes=Estudiantes                       |
|  nameTablePartidos=Partidos                           >-+
|
|  [Paths]
|  pathProgramFont=./fonts/GoMonoNerdFont-Regular.ttf   >-+
|  pathPdfFont=./fonts/LibertinusMono-Regular.ttf         +--- Estas son rutas del sistema que se necesitan para encontrar las fuentes de las letras o asignar
|  informeName=Informe VOCA Votaciones 2026.pdf         >-+    un nuevo nombre al PDF de informe en caso de ser necesario
+-------------------------------------------------------+

La carpeta config/ tiene dos funciones:

./config/config.cpp:
  configureData() - Se encarga de verificar si existe el archivo de configuración y cargar los valores del archivo de configuración al programa
  loadConfig() - En caso de que la función anterior falle, el programa enviará al usuario a la pantalla de configuración, y cargará la información
                    de configuración que se haya podido obtener, para que el usuario no tenga que introducir toda la configuración variable por variable

── db/ ────────────────────────────────────────────────────────────────────────
Esta carpeta contiene las dos funciones relacionadas a la base de datos:

./db/database.cpp:
  sendquery() - Se encarga de enviar las queries a la base de datos, utiliza la conexión primaria del programa ("conn") con las respectivas credenciales que se
                cargaron a partir del archivo de configuración, y el resultado de la query lo almacena en una variable global llamada "outQuery"
  updateData() - Su propósito es actualizar información de la base de datos y cargarla al programa, sirve para verificar también si el archivo de configuración está
                 correctamente configurado, si ocurre un error devuelve un número específico sobre lo que causó el error, pero si ocurre todo bien ayuda a actualizar los datos

── platform/ ─────────────────────────────────────────────────────────────────
Se encarga de la funcionalidad multiplataforma de varias partes del código, en esta carpeta existe mucho código que necesita verificar si se compilará para windows
o para linux, además de también ser compatibles con UTF-8, UTF-16 y trabajar internamente con UTF-32 | La explicación de las codificaciones de texto se encuentra en ./platform/README.txt

./platform/clipboard.cpp:
  clipboard() - Carga el contenido del portapapeles desde Linux o Windows, hay 2 funciones en esta carpeta con el mismo nombre de clipboard() pero solo una se compila dependiendo del OS

./platform/encoding.cpp:
  UTF8ToUTF32() / UTF32ToUTF8() / UTF16ToUTF32() - Estas tres funciones se encargan de traducir las codificaciones de texto, no me pregunten como funcionan, si quieren saber preguntenle a Claude

./platform/validIP:
  validIP() - Verifica si una IP es válida, esto sirve para chequeos en la configuración del programa y conexiones a la base de datos desde el panel de administración por medio de la pestaña "Terminal"

./platform/pty.cpp:
  to_wstring() / ptyfunc() - La función to_wstring() es una que se necesita usar para traducir un string a wstring, ya que la API de windows necesita que los datos sean en wstring. La función principal
                             de ptyfunc() es poder ejecutar los comandos de la pestaña "Terminal" del panel de administración, así que crea una pseudoterminal (pty) lo que emula una terminal interactiva
./platform/compat.hpp:
  compat.hpp - No es una función, pero es un archivo importante, permite de que los conflictos que mencioné sobre las funciones DrawText, CloseWindow... que provienen de Windows, no choquen con raylib

── reports/ ──────────────────────────────────────────────────────────────────
Es la carpeta que tiene la función de la creación de los informes, es decir, la función de la creación de un PDF de salida con la información de los partidos, sus dos funciones principales son:

./reports/inform.cpp:
  inform() - Es la función que crea el PDF acerca de la información de los partidos, hay que hacerlo más bonito sí, pero eso luego
  error_handler() - En caso de que ocurra un error con la creación del PDF, se llama a error_handler() para que el programa no se explote, ya que por defecto la librería de los PDFs sin esta función, explotaría

── screens/ ──────────────────────────────────────────────────────────────────
Posee todas las pantallas del programa, desde su backend hasta su frontend, cada archivo tiene dos funciones principales, una que termina en "Update" (backend) y otra que termina en "Draw" ("frontend")
Existen 7 pantallas:
  LOGO: Es la pantalla del inicio del programa, cuando esta pantalla termina su tiempo en la que se muestra, empieza a llamar a las funciones de configuracion y carga de los datos de la base de datos
  CONFIGURATION: En caso de que la función de configuración o la de carga de los datos de la base de datos (es decir, configureData() y updateData()) lleguen a dar error, se enviará a la pantalla de
                 configuración, para que el usuario arregle los inconvenientes con la conexión a la base de datos, detalles de las tablas y columnas como nombres de columnas incorrectas o rutas de font no existentes
  MAINMENU: Es el menú principal en caso de que no ocurra ningun error después de la pantalla LOGO o de que se arregle la configuración en CONFIGURATION, muestra una barra de entrada para digitar la cédula,
            y si se digita una la contraseña de administración, envía al usuario al panel de administración
  ADMINMENU: Es el panel de administración, muestra 7 pestañas: "Consultar", "Agregar", "Actualizar", "Borrar", "Explorar", "Resultados" y "Terminal":
                  - Consultar: Es la pestaña que permite realizar consultas a la base de datos
                  - Agregar: Permite agregar valores a la base de datos
                  - Actualizar: Se encarga de cambiar o actualizar valores
                  - Borrar: Como su nombre lo dice, borra valores en la base de datos
                  - Explorar: Es como un explorador de archivos pero en la base de datos, muestra absolutamente todos los datos de cada tabla, en caso de la necesidad de buscar algo rápido
                  - Resultados: Muestra la cantidad de votos por cada partido, como también los porcentajes de cada partido, para cambiar el valor, se toca el botón de "#" o "%" arriba de la gráfica,
                                además de que en esta pestaña se puede crear un PDF de informe de los resultados de las votaciones del programa
                  - Terminal: En caso de que el administrador de la base de datos lo necesite, puede ejecutar una terminal dentro del programa para conectarse a la base de datos
             !!! El panel también posee 3 botones extra:
                  - Configuración: Entra a la pantalla CONFIGURATION en caso de que se necesite configurar algo
                  - Actualizar: En caso de que hayan datos que deban ser actualizados
                  - Salir: Sale del panel de administración
  CONFIRMATION: En caso de que en MAINMENU se introduzca una cédula de verdad, enviará al usuario a CONFIRMATION, le servirá para confirmar si su cédula existe, es correcta, o no
  VOTATION: Si se llega a verificar la cédula, el estudiante tiene la pantalla para votar por un partido o NULO, pero no puede dejar el voto en blanco
  ENDING: Cuando ya el estudiante realiza la votación de manera exitosa, se muestra una pantalla de muchas gracias por 10 segundos, y se devuelve a MAINMENU, para esperar otro estudiante

── ui/ ───────────────────────────────────────────────────────────────────────
Su contenido son principalmente funciones relacionadas a agarrar datos del backend y mostrarlas en el frontend:

./ui/drawing.cpp:
  logfunction() - Es la función que se encarga de que en cada pestaña del panel de administración se pueda ver la respuesta, por ejemplo una consulta en la pestaña "Consular" aparece
                  la respuesta en un cuadro negro abajo, como también en "Agregar", "Actualizar" y "Borrar"
  centertext() - Como dice su nombre, se encarga de centrar el texto en la pantalla o en una ubicación en el eje x, no tiene mucha ciencia
  drawcolumns() - Es de las funciones más importantes, siendo sincero cometí una atrocidad, tuve que mezclar un poco del backend con el frontend con esta función para poder conseguir lo que hace, esta
                  función se encarga de recibir los datos de las columnas que se ingresen en sus respectivas barras, y también si se cambia la tabla en la que se encuentra en el panel de administración,
                  también cambiarán las columnas de la respectiva tabla, esta función se usa en el panel de administración para mostrar el nombre de las columnas y los datos que se han introducido

./ui/input.hpp:
  isPressed() - Verifica si un objeto ha sido presionado (devuelve estado 3), tiene el cursor encima y fue presionado (devuelve estado 2), el cursor está en una posición fuera del objeto pero
                fue presionado (devuelve estado 1), o definitivamente el usuario no lo ha presionado pero puede que el cursor esté encima del objeto o no, solo que mientras no lo presione no cambiará
                de estado (estado 0)
  GetCursorFromMouseClick() - Busca la posición en la que el cursor presionó la barra en su eje x, para posicionarlo en la barra detrás o delante de ciertas letras según la posición del cursor
  inputfunc() - Es la función que recibe datos del teclado a partir de una barra de entrada, acepta Ctrl+V, HOME, DELETE, BACKSPACE (obviamente), END, y teclas de flechas, lo único que no pienso implementar: Ctrl+C

./ui/objects.cpp:
  objectCreation() - Es la función que se encarga de la creación de todos los objetos en el programa, y si las funciones de configuración y conexión a la base de datos salen exitosas, también configura los objetos
                     relacionados a la base de datos, como columnas, tablas y los partidos

./ui/widgets.hpp:
  widgets.hpp - No es una función, pero se encarga de la creación de las clases para los objetos, lo cual lo hace un archivo muy importante de mencionar
