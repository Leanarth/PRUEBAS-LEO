# El compilador se ejecuta de la siguiente manera:
#
# ./compiler.sh
#
# En caso de que no pueda ejecutarlo, escriba:
#
# chmod +x ./compiler.sh
#
# Esto lo que hace es darle permisos de ejecución al compilador
#
# Lo que hace este compilador es:
#
# Armar el comando para compilar el programa:

# Compilado en modo depuración (el modo de depuración puede hacer que el ejecutable sea más lento y consuma más recursos, pero sirve para debuggear errores):

#cmd="g++ -fsanitize=address -g -O0 -std=c++20"

# Compilado en modo release (el modo release es el modo en el que el ejecutable se compilará para su uso real, es más rápido y sirve para probar la velocidad real del ejecutable):

cmd="g++ -s -DNDEBUG -O3 -std=c++20"

# Añade el resto de argumentos para el compilador:

cmd=$cmd" ./*/*.cpp  ./*.cpp"                                                                                                                                 # Le dice al compilador dónde están los archivos para compilar
cmd=$cmd" -I . -I ./deps/src/ -I ./deps/include/ -I ./deps/demo/ -I ./deps/mysql/ -I ./config/ -I ./db/ -I ./platform/ -I ./reports/ -I ./screens/ -I ./ui/"  # Le dice al compilador dónde están las carpetas con las dependencias
cmd=$cmd" -L . -L ./deps/src/ -L ./deps/include/ -L ./deps/demo/ -L ./deps/mysql/ -L ./config/ -L ./db/ -L ./platform/ -L ./reports/ -L ./screens/ -L ./ui/"  # Le dice al compilador dónde están las librerías necesarias
cmd=$cmd" -lhpdf -lpng -lstdc++ -lmysqlclient -lz -lssl -lcrypto -lresolv -lm -lraylib -lGL -lraylib -lm -lpthread -ldl -lrt -lX11 -latomic"                  # Le dice al compilador cuáles librerías debe de llamar
cmd=$cmd" -o main"                                                                                                                                            # Le dice al compilador el nombre del archivo de salida, el cual se llama main

echo "Comando ejecutado:\n\n$cmd\n\n"                                                                                                     # Muestra el comando para compilar en la pantalla
$cmd                                                                    # Ejecuta el comando para compilar | Es normal si aparecen advertencias en morado, pero si hay un error, aparecerá en rojo, y no logrará concluir la compilación

# Darle permisos de ejecución al archivo del código compilado para poder ejecutarse | chmod es un comando que permite otorgar permisos de lectura, escritura y ejecución, con esto, le doy permisos de ejecución:

chmod +x ./main

# Ejecutar el archivo compilado:

./main

# Cuando ya termina el programa, se procede a borrar el archivo compilado, esto para automatizar no tener que borrarlo de nuevo para compilarlo otra vez

rm ./main

# Se verifica si el compilador tuvo algún error | Si el código de estado NO es 0, entonces ocurrió un error en alguna parte del compilador

echo "\n$0 ha finalizado con un código de estado $?\nRecuerde que si el código de estado no es igual a 0, ocurrió un error en alguna parte del compilado o ejecución"

# En caso de que no quiera que se borre, solo comente la línea que dice "rm ./main" agregando un hashtag # al inicio
#
# En caso de que quiera cancelar la compilación por cualquier razón, presione Ctrl+C, esto permite cancelar la compilación
