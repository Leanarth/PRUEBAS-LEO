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

echo "\nCompilando en modo de depuración...\n\n" && cmd="g++ -fsanitize=address -g -O0 -std=c++20"

# Compilado en modo release (el modo release es el modo en el que el ejecutable se compilará para su uso real, es más rápido y sirve para probar la velocidad real del ejecutable):

#echo "\nCompilando en modo release...\n\n" && cmd="g++ -s -DNDEBUG -O3 -std=c++20"

# Añade el resto de argumentos para el compilador:

cmd=$cmd" ./*/*.cpp  ./*.cpp"                                                                                                                                 # Le dice al compilador dónde están los archivos para compilar
cmd=$cmd" -I . -I ./deps/linux/src/ -I ./deps/linux/include/ -I ./deps/linux/demo/ -I ./deps/linux/mysql/ -I ./config/ -I ./db/ -I ./platform/ -I ./reports/ -I ./screens/ -I ./ui/"  # Le dice al compilador dónde están las carpetas con las dependencias
cmd=$cmd" -L . -L ./deps/linux/src/ -L ./deps/linux/include/ -L ./deps/linux/demo/ -L ./deps/linux/mysql/ -L ./config/ -L ./db/ -L ./platform/ -L ./reports/ -L ./screens/ -L ./ui/"  # Le dice al compilador dónde están las librerías necesarias
cmd=$cmd" -lhpdf -lpng -lstdc++ -lmysqlclient -lz -lssl -lcrypto -lresolv -lm -lraylib -lGL -lraylib -lm -lpthread -ldl -lrt -lX11 -latomic"                  # Le dice al compilador cuáles librerías debe de llamar
cmd=$cmd" -o ./bin/linux/main"                                                                                                                                # Le dice al compilador el nombre del archivo de salida, el cual se llama main

echo "Comando ejecutado:\n\n$cmd\n\n"                                                                                                     # Muestra el comando para compilar en la pantalla
$cmd                                                                    # Ejecuta el comando para compilar | Es normal si aparecen advertencias en morado, pero si hay un error, aparecerá en rojo, y no logrará concluir la compilación

# Darle permisos de ejecución al archivo del código compilado para poder ejecutarse | chmod es un comando que permite otorgar permisos de lectura, escritura y ejecución, con esto, le doy permisos de ejecución:

if [ $? -eq 0 ]; then                 # Verifica el código de estado del compilado, si ocurrió con éxito (código de estado 0) procederá a declararlo con permisos de ejecución
  chmod +x ./bin/linux/main
else                                # En caso de que ocurran errores, imprimirá en pantalla que abortará el programa
  echo "\nLa creación del compilado tuvo errores, abortando..."
  exit
fi

# Ejecutar el archivo compilado:
export LD_LIBRARY_PATH=$PWD/bin/linux:$LD_LIBRARY_PATH              # En linux, el ejecutable necesita la librería dinámica que se encuentra en la carpeta bin/linux para poder ejecutarse, entonces la almacena en la variable LD_LIBRARY_PATH
./bin/linux/main

# Cuando ya termina el programa, se procede a borrar el archivo compilado, esto para automatizar no tener que borrarlo de nuevo para compilarlo otra vez

if [ $? -eq 0 ]; then                 # Verifica el código de estado del compilado, si ocurrió con éxito (código de estado 0) procederá a declararlo con permisos de ejecución
  rm ./bin/linux/main
else
  echo "\nHubo un error en el programa"
  exit
fi

if [ $? -ne 0 ]; then                 # Verifica el código de estado del borrado, si no ocurrió con éxito (código de estado distinto a 0) procederá a dar un mensaje
  echo "\nHubo un error en el borrado del archivo compilado"
fi

# Se verifica si el compilador tuvo algún error | Si el código de estado NO es 0, entonces ocurrió un error en alguna parte del compilador

if [ $? -eq 0 ]; then                 # Verifica el código de estado del compilado, si ocurrió con éxito (código de estado 0) procederá a declararlo con permisos de ejecución
  echo "\n\nEl proceso de compilado y de borrado resultaron de manera exitosa"
else
  echo "\n\nHubo un error en alguna parte del proceso de compilado y borrado"
fi

# En caso de que no quiera que se borre, solo comente la línea que dice "rm ./main" agregando un hashtag # al inicio
#
# En caso de que quiera cancelar la compilación por cualquier razón, presione Ctrl+C, esto permite cancelar la compilación
