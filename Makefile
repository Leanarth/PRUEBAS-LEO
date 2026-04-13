#
# Configuración del compilador
#
CXX = g++
CXXFLAGS = -fsanitize=address -g -O0 -std=c++20 # Modo de depuración
# CXXFLAGS = -fsanitize=address -g -O0 -std=c++20 # Modo release

#
# Carpetas del proyecto
#
SRC_DIRS = config db platform reports screens ui
DEPS_DIR = $(shell find ./build/deps/linux -maxdepth 1 -type d)
OBJ_DIR = build/obj
BIN_DIR = build/bin/linux
TARGET = $(BIN_DIR)/main

#
# Archivos fuente (.cpp) y objetos (.o)
#
SRCS = main.cpp globals.cpp $(shell find $(SRC_DIRS) -name '*.cpp')  # Busca los archivos .cpp
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)                                  # Reemplaza el .cpp a .o y los guardará en la ruta de los objetos

#
# Includes
#
INC_FLAGS = $(addprefix -I, $(SRC_DIRS) $(DEPS_DIR))
CXXFLAGS += $(INC_FLAGS)

#
# Enlazado de dependencias
#
LDFLAGS = -L./build/deps/linux/src -L./build/deps/linux/demo -L./build/deps/linux/mysql -L./build/deps/linux/include -lhpdf -lpng -lstdc++ -lmysqlclient -lz -lssl -lcrypto -lresolv -lm -lraylib -lGL -lraylib -lm -lpthread -ldl -lrt -lX11 -latomic -fsanitize=address
LDFLAGS += -Wl,-rpath,'$$ORIGIN'

#
# Regla principal
#
all: $(TARGET)

#
# Creación de main
#
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

#
# Regla para compilar los .cpp a .o
#
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

#
# Limpiar los archivos después
#
clean:
	rm -r build/bin/linux/main build/obj/*
