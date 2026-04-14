# Contenidos de ui/
En esta carpeta se encuentran archivos relacionados a la interfaz de usuario (UI), es decir, principalmente funciones relacionadas al frontend como también datos de entrada del usuario, a continuación la función de cada archivo:

* drawing: los archivos con nombre drawing se refieren principalmente al frontend del usuario, estos archivos muestran barras, botones o cuadros en el frontend, en este archivo se pueden apreciar funciones como logfunction() que se encarga de dibujar una gráfica en la pestaña "Resultados" del panel de administración, o también se encarga de la función shortmessage() que muestra un breve mensaje en el centro de la pantalla del usuario.
* input: su función principal es recibir datos de entrada, así como información de las teclas digitadas en el teclado para ser luego introducidas en una barra que reciba datos con la función inputfunc(), o sino también verificar el estado de un objeto en caso de que tenga el cursor encima, haya sido presionado, o no haya recibido interacción alguna.
* objects: su contenido principal es la función objectCreation(), esta función permite la creación de todos los objetos con sus respectivas clases en el programa, es una función demasiado larga, así que por eso necesita un archivo a parte, además de que esta función también se encarga de crear los objetos que hayan sido cargados de la base de datos como botones de tablas o barras de datos de entrada de las columnas, para luego ser usados en la función drawcolumns del archivo drawing.
* widgets: posee todas las clases de cada objeto del programa, es una clase padre con cuatro clases hijas:
   * nxyxys: es la clase padre, posee las propiedades básicas de cada objeto, como name, xloc (ubicación en eje x), yloc (ubicación en eje y), xsize (tamaño en ancho), ysize (tamaño en alto), status (estado del objeto).
   * button: sirve para botones en el programa como su nombre lo dice.
   * inputbar: sirve para recibir datos de entrada como una barra rectangular con un cursor parpadeante.
   * sqlobject: es para objetos propiamente cargados de una base de datos sql
   * column: su función es exclusivamente declarar a las columnas cargadas de una base de datos con información extra, como la tabla correspondiente a la columna actual, o el id de la columna.

Cabe aclarar que existen 4 tipos de estados para cada objeto en el programa, los cuales resumidamente son así:

   * 0: No presionado, y tampoco hay interacción alguna
   * 4: Presionado en este instante
   * 3: Fue presionado anteriormente, y el cursor está encima del objeto
   * 2: Fue presionado anteriormente, y el cursor NO está encima del objeto
   * 1: No fue presionado anteriormente, pero el cursor está encima del objeto
