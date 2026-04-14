# Contenidos de screens/
Esta carpeta tiene tanto el backend como el frontend de cada pantalla del programa, hay que recordar que existen 7 pantallas, y cada una de estas pantallas cumplen determinadas funciones:

* LOGO: Es la primera pantalla que aparece en el programa, tiene 2 segundos que se muestra en pantalla, cuando se cumplen estos 2 segundos procede a verificar la configuración de la base de datos para comprobar la conexión, en caso de que todo se encuentre bien envía al usuario a MAINMENU, en caso de que haya algún fallo envía al usuario a CONFIGURATION.
* ADMINMENU: Esta pantalla posee el panel de administración, tiene 7 pestañas:
   * Consultar: Es la pestaña que sirve para la búsqueda de datos a través de expresiones regulares de MySQL.
   * Agregar: Permite agregar datos a la base de datos.
   * Actualizar: Sirve para modificar datos de la base de datos, tanto individualmente, como en masa.
   * Eliminar: Como su nombre lo dice elimina datos, permite eliminar desde registros individuales, como tablas enteras.
   * Explorar: Permite visualizar absolutamente todos los datos de las tablas disponibles en una lista.
   * Resultados: Muestra los resultados de las votaciones a través de una gráfica en el programa, y también permite realizar un informe PDF de los resultados.
   * Terminal: Muy útil en caso de necesitar ejecutar comandos específicos para la base de datos, provee 5 barras para digitar las credenciales a la conexión de la base de datos y también almacena un historial de los comandos digitados que se borra después de finalizar la sesión del administrador.
* CONFIGURATION: Permite poder reconfigurar el programa, posee 3 pestañas:
   * Credenciales: Sirve para configurar las credenciales a la base de datos, como también la dirección IP y puerto, además de la contraseña del panel de administración.
   * Extra: Ayuda a configurar demás datos relacionados a la base de datos, críticos para que el programa funcione correctamente, aunque igual poco probable de que cambien, pero igual se incorpora para prevenir la caída del programa en caso de que ocurran cambios en la base de datos.
   * Paths: Configura la ruta de los archivos de las fuentes de las letras, en caso de que alguna ruta de alguna letra sea distinta a la especificada en el archivo de configuración del programa, se puede reconfigurar desde acá.
* MAINMENU: Accede al menú principal del programa, el cual tiene un mensaje acerca de digitar la cédula y de los formatos permitidos, y también posee una barra para recibir los datos de entrada, y cuando se digita una cédula se envía al estudiante a la pantalla CONFIRMATION para confirmar su cédula.
* CONFIRMATION: Ayuda a confirmar de que la cédula digitada sea del estudiante correcta, si la cédula fue correcta procederá a verificar luego si el estudiante ha votado o no, si NO ha votado, mandará al estudiante a VOTATION, en caso de que SÍ haya votado, mandará al estudiante a ENDING, además de que también sirve para verificar que el voto digitado sea correcto cuando esta pantalla se llama después de la pantalla VOTATION.
* VOTATION: Es la pantalla que muestra los partidos a votar, y al seleccionar uno, enviará al estudiante a la pantalla CONFIRMATION.
* ENDING: Es la pantalla final, mostrará un mensaje de agradecimiento si se llama después de la pantalla CONFIRMATION después de haber votado de manera exitosa, si el estudiante no pudo votar ya que se encontró que ya había votado, entonces solo mostrará un mensaje de que ya ha votado. Esta pantalla tiene un tiempo de 10 segundos que se muestra, después de los 10 segundos se reinician las variables usadas para el flujo de votación del programa para recibir al siguiente estudiante.
