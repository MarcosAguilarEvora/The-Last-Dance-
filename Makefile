# ============================================================================
# Makefile para "The Last Dance" - Proyecto SFML con MinGW en Windows
# Compatible con mingw32-make y MSYS shell
# ============================================================================
# Compilador y herramientas
CXX := g++
RM := rm -f
RMDIR := rm -rf

# ============================================================================
# Directorios del proyecto
SRC_DIR := src
INCLUDE_DIR := include
OBJ_DIR := obj
BIN_DIR := bin

# ============================================================================
# Configuración de compilación
# Rutas de inclusión
CXXFLAGS := -I$(INCLUDE_DIR)

# Librerías de MinGW (ubicación)
MINGW_LIB := -LC:\msys64\mingw64\lib

# Flags de optimización y advertencias
CXXFLAGS += -Wall -Wextra -std=c++11

# Modo Windows (GUI, no consola)
CXXFLAGS += -mwindows

# ============================================================================
# Librerías SFML a enlazar
# Orden importante: librerías específicas primero, libsfml-main al final
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main

# ============================================================================
# Nombre del ejecutable
TARGET := $(BIN_DIR)/FutbolJuego.exe

# ============================================================================
# Archivos fuente y objetos
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# ============================================================================
# REGLAS DE COMPILACIÓN
# ============================================================================

# Regla por defecto (primera regla)
.PHONY: all
all: $(TARGET)

# Crear el ejecutable enlazando los archivos objeto
$(TARGET): $(OBJECTS)
	@echo [ENLAZANDO] Creando ejecutable: $@
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(MINGW_LIB) $(LIBS)
	@echo [EXITO] Compilación completada: $@

# Compilar archivos .cpp a archivos objeto .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo [COMPILANDO] $< ...
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================================================
# REGLAS ADICIONALES
# ============================================================================

# Limpiar archivos objeto y binarios
.PHONY: clean
clean:
	@echo [LIMPIANDO] Eliminando archivos objeto y binarios...
	$(RMDIR) $(OBJ_DIR) $(BIN_DIR) 2>/dev/null || true
	@echo [EXITO] Limpieza completada

# Limpiar solo objetos (mantiene el ejecutable)
.PHONY: clean-obj
clean-obj:
	@echo [LIMPIANDO] Eliminando archivos objeto...
	$(RMDIR) $(OBJ_DIR) 2>/dev/null || true
	@echo [EXITO] Archivos objeto eliminados

# Limpiar solo el ejecutable
.PHONY: clean-bin
clean-bin:
	@echo [LIMPIANDO] Eliminando ejecutable...
	$(RMDIR) $(BIN_DIR) 2>/dev/null || true
	@echo [EXITO] Ejecutable eliminado

# Mostrar información del proyecto
.PHONY: info
info:
	@echo ================================================
	@echo "The Last Dance - Información del Proyecto"
	@echo ================================================
	@echo Compilador: $(CXX)
	@echo Target: $(TARGET)
	@echo Archivos fuente: $(SOURCES)
	@echo Archivos objeto: $(OBJECTS)
	@echo Librerías: $(LIBS)
	@echo Directorios:
	@echo   - Fuentes: $(SRC_DIR)/
	@echo   - Incluidos: $(INCLUDE_DIR)/
	@echo   - Objetos: $(OBJ_DIR)/
	@echo   - Binarios: $(BIN_DIR)/
	@echo ================================================

# Mostrar ayuda
.PHONY: help
help:
	@echo ================================================
	@echo "The Last Dance - Ayuda de Makefile"
	@echo ================================================
	@echo Comandos disponibles:
	@echo   make              Compilar el proyecto (por defecto)
	@echo   make clean        Eliminar todos los archivos compilados
	@echo   make clean-obj    Eliminar solo archivos objeto
	@echo   make clean-bin    Eliminar solo el ejecutable
	@echo   make info         Mostrar información del proyecto
	@echo   make help         Mostrar esta ayuda
	@echo ================================================
	@echo Ejecución del juego:
	@echo   bin\FutbolJuego.exe
	@echo ================================================

# ============================================================================
# Marcar reglas que no generan archivos con ese nombre
.PHONY: all clean clean-obj clean-bin info help
