# Contenidos de reports/
Esta carpeta únicamente tiene la función inform() junto a error_handler():

* inform(): crea el PDF de informe a través del nombre previamente definida en la variable informeName configurada por el archivo de configuración o el panel de configuración.
* error_handler(): en caso de que ocurra algún error en la creación del PDF, llamará a error_handler() para manejar el error y posteriormente especificar el error y mostrar el código de error con sus detalles en el frontend.
