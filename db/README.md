# Contenidos de db/
Esta carpeta contiene dos funciones importantes que se encuentran en el archivo database.cpp, las cuales son:

* sendquery(): se encarga del envío de queries ya armadas a la base de datos, solo usa la conexión almacenada en la variable "conn" y la utiliza para ejecutar las queries a la base de datos.
* updateData(): procede a actualizar los datos cargados de la base de datos y comprobar de que cada uno sea correcto, en caso de que encuentre algún error se almacenará en una variable global que almacena el código de error de updateData() y mostrará el error en el frontend para su arreglo.
