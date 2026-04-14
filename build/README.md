# Contenidos de build/
En esta carpeta, se encuentran los binarios y dependencias tanto de Linux como de Windows, estos sirven de mucho para el compilado del programa dependiendo del sistema operativo, y sus librerías necesarias para su funcionamiento

## bin/
Esta carpeta cuenta con las librerías dinámicas para la ejecución del programa, además de otras dependencias extra, en bin/ existen dos subcarpetas:

* linux/: esta carpeta cuenta con las librerías dinámicas que necesita el programa para poder ejecutarse, como libraylib.so y libhpdf.so
* windows/: esta carpeta cuenta con los archivos de mariadb para la ejecución correcta del mariadb.exe en la llamada de la función ptyfunc(), además del ejecutable VC_redist.x64.exe el cual instala dlls necesarios relacionados a C++ en caso de que no se encuentren, y otros dlls a instalar se encuentran disponibles en la carpeta windows/dlls, todos estos archivos son posteriormente comprimidos en un .rar al ejecutar exe_compiler.sh.

## deps/
En esta carpeta, se encuentran todas las dependencias necesarias del programa para compilarse, es decir, al compilar el programa usando compiler.sh o exe_compiler.sh, los dos compiladores buscan en esta carpeta para encontrar las dependencias del programa, en esta carpeta, también existen dos subcarpetas:

* linux/: también se encuentra linux/, cuyo propósito es tener las dependencias del programa en caso de que se compile para linux.
* windows/: su propósito a este punto ya es muy obvio, se encarga de tener las dependencias del programa en caso de que se compile para windows.

## obj/
Esta carpeta tiene los archivos objeto (formato .o) de cada archivo de código fuente, los archivos objeto sirven para únicamente ser enlazados en el compilador y unirlos con los demás, estos archivos sirven para que el compilado con el archivo compiler.sh sea mucho más rápido, por que al tener los archivos objeto de cada parte del código ayuda a que no se deba de volver a compilar todos los pedazos del código fuente de cada archivo .cpp, y únicamente verificar si un archivo ha sido modificado o no, en caso de que haya sido modificado, se elimina el archivo objeto viejo del archivo .cpp modificado, y lo compila para archivo objeto para posteriormente enlazarlo con el compilador y crear el ejecutable, y en caso de que el archivo .cpp correspondiente al archivo objeto NO haya sido modificado, solo lo enlazará al compilador para crear el ejecutable. En resumen, estos archivos son los que tienen los datos de cada archivo .cpp, y los almacenan a cada uno en un archivo .o, estos archivos .o, sirven para luego crear el archivo ejecutable, y sin estos archivos .o, se tendrían que compilar desde cero todos los .cpp para llegar al archivo ejecutable.
En caso de que no se encuentre esta carpeta en el repositorio de github o en la primera instalación del repositorio, no se preocupe, se creará posteriormente después del compilado del programa con compiler.sh.

> [!NOTE]
> Por esta razón, es por la que sugiero no usar más librerías, ya que si se usan más, puede llegar a generar más peso en este repositorio, y posteriormente ser más duradero el compilado del programa, como también puede ser más complicado el compilado desde linux para windows, ya que tengo que tener las dependencias para linux como para windows, y verificar sus librerías dinámicas en caso de que tenga, o buscar sus librerías estáticas en caso de que existan.
