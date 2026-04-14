# Contenidos de platform/
Esta carpeta tiene principalmente funciones del backend que varían dependiendo del sistema operativo, la función de cada archivo sería:

* clipboard: se encarga de manejar el portapapeles dependiendo de cada sistema operativo, en caso de que se presione la tecla Ctrl+V la función inputfunc() llamará a la función encargada de recolectar los datos del portapapeles por medio de la función clipboard() que se encuentra en este archivo, esta función tiene una definición para linux como otra para windows.
* compat: importa las librerías de manera adecuada para que no ocurra ningún conflicto entre las librerías raylib y windows principalmente.
* encoding: sirve para las traducciones entre UTFs.
* ipvalid: tiene la función relacioada a verificar si una dirección IP es válida o no, esto ayuda a verificar en las credenciales de la conexión a la base de datos si una IP es real, o si es falsa, y evitar de que MySQL trate de establecer una conexión a una dirección que no existe.
* pty: es el archivo que se encarga de realizar las conexiones usando una terminal a través de la pestaña "Terminal" del panel de administración.

> [!NOTE]
> Hay que recordar que cada uno de estos archivos tiene una definición tanto para linux como para windows, que pueden haber mínimas diferencias como en ipvalid, pero igual siguen siendo distintas definiciones, debido a las macros de C++
