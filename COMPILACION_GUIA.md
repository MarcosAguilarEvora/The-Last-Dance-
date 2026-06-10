# 🏈 The Last Dance - Guía de Compilación con Makefile

## ✅ **Arquitectura de Compilación Escalable**

Has actualizado tu proyecto a un **sistema de compilación profesional** basado en Makefile, compatible con MinGW en Windows.

---

## 📂 **Estructura de Carpetas Final**

```
The-Last-Dance-/
├── src/                          ← Archivos fuente (.cpp)
│   └── main.cpp
├── include/                       ← Archivos encabezado (.hpp)
│   ├── juego.hpp
│   ├── personaje.hpp
│   ├── enemigo.hpp
│   ├── entidades.hpp
│   └── interfazusuario.hpp
├── obj/                           ← Archivos objeto (.o) [GENERADO]
│   └── main.o
├── bin/                           ← Ejecutable final [GENERADO]
│   └── FutbolJuego.exe
├── assets/                        ← Recursos (texturas, sonidos)
├── Makefile                       ← Sistema de compilación 🆕
├── COMPILACION_GUIA.md           ← Este archivo
├── README.md
└── .vscode/
    └── launch.json (tasks.json eliminado ✅)
```

---

## 🛠️ **Herramienta de Compilación**

### **¿Qué comando usar?**

```bash
mingw32-make
```

**O alternativamente** (si tienes `make` en PATH):
```bash
make
```

**Verificar qué tienes instalado:**
```bash
mingw32-make --version
make --version
```

---

## 📋 **Comandos Disponibles**

### **1️⃣ Compilar el proyecto (por defecto)**
```bash
mingw32-make
```
**O simplemente:**
```bash
mingw32-make all
```

**Qué hace:**
- Compila `src/main.cpp` → `obj/main.o`
- Enlaza `obj/main.o` → `bin/FutbolJuego.exe`

### **2️⃣ Ejecutar el juego**
```bash
bin\FutbolJuego.exe
```

### **3️⃣ Compilar y ejecutar de una vez**
```bash
mingw32-make && bin\FutbolJuego.exe
```

### **4️⃣ Limpiar todo (objetos + ejecutable)**
```bash
mingw32-make clean
```

### **5️⃣ Limpiar solo archivos objeto**
```bash
mingw32-make clean-obj
```

### **6️⃣ Limpiar solo el ejecutable**
```bash
mingw32-make clean-bin
```

### **7️⃣ Ver información del proyecto**
```bash
mingw32-make info
```

### **8️⃣ Ver esta ayuda**
```bash
mingw32-make help
```

---

## 🔧 **Flujo de Trabajo Recomendado**

### **Día 1: Primer build**
```bash
cd C:\Users\Karla\OneDrive\Documentos\GitHub\The-Last-Dance-
mingw32-make
bin\FutbolJuego.exe
```

### **Día 2+: Compilación rápida**
```bash
mingw32-make && bin\FutbolJuego.exe
```

### **Antes de cambios importantes**
```bash
mingw32-make clean
mingw32-make
```

### **Limpiar antes de subir a Git**
```bash
mingw32-make clean
git add .
git commit -m "Mensaje"
git push
```

---

## 📊 **Configuración del Makefile**

| Variable | Valor |
|----------|-------|
| **Compilador** | `g++` |
| **Versión GCC** | 14.2.0 (MinGW) |
| **Ruta MinGW** | `C:\msys64\mingw64\lib` |
| **Directorio fuentes** | `src/` |
| **Directorio includes** | `include/` |
| **Directorio objetos** | `obj/` |
| **Directorio binarios** | `bin/` |
| **Flags** | `-Wall -Wextra -std=c++11 -mwindows` |
| **Librerías SFML** | graphics, window, system, main |

### **Orden de enlace (CRÍTICO)**
```
-lsfml-graphics -lsfml-window -lsfml-system -lsfml-main
```
⚠️ `libsfml-main` **SIEMPRE** al final

---

## 🎯 **Arquitectura del Makefile Explicada**

### **Archivos que se generan**

1. **Compilación:** `.cpp` → `.o` (en carpeta `obj/`)
   ```
   src/main.cpp → obj/main.o
   ```

2. **Enlace:** `.o` + librerías → `.exe` (en carpeta `bin/`)
   ```
   obj/main.o + SFML libs → bin/FutbolJuego.exe
   ```

### **Regla de patrón para compilación**

```makefile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

Significa: "Para cada `.cpp` en `src/`, crear un `.o` en `obj/`"

### **Regla de enlace**

```makefile
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(MINGW_LIB) $(LIBS)
```

Significa: "Tomar todos los `.o` y crear el ejecutable"

---

## 🐛 **Troubleshooting**

### **Error: "mingw32-make: command not found"**
```bash
# Verifica que MinGW esté en tu PATH
echo %PATH%

# O usa la ruta completa:
C:\msys64\mingw64\bin\mingw32-make all
```

### **Error: "No rule to make target"**
- Verifica que `src/main.cpp` existe
- Verifica que `include/` contiene los `.hpp`

### **El ejecutable no se crea**
```bash
mingw32-make clean
mingw32-make -B
```

La opción `-B` fuerza la recompilación de todo.

### **"permission denied" al compilar**
- Verifica que no tengas `bin/FutbolJuego.exe` abierto
- Cierra el juego si está ejecutándose

---

## 📝 **Ventajas de este Makefile**

✅ **Compilación incremental** - Solo recompila lo que cambió  
✅ **Limpio y modular** - Directorios separados (obj/, bin/)  
✅ **Fácil de escalar** - Agregar más `.cpp` automáticamente  
✅ **Compatible con MinGW** - Usa MSYS shell (POSIX-compatible)  
✅ **Documentado** - Comandos `help` e `info` incluidos  
✅ **Windows-friendly** - Rutas con backslashes correctamente escapadas  

---

## 🚀 **Próximos Pasos**

Si necesitas **agregar más archivos `.cpp`**:
1. Crea el archivo en `src/`
2. Crea el encabezado en `include/`
3. `#include` desde tu código
4. Ejecuta `mingw32-make` (automáticamente detectará el nuevo archivo)

### Ejemplo:
```bash
# Crear nuevo archivo
echo #include "juego.hpp" > src/nivel.cpp

# Compilar (detecta automáticamente src/nivel.cpp)
mingw32-make
```

---

## 📞 **Resumen Rápido**

| Tarea | Comando |
|-------|---------|
| Compilar | `mingw32-make` |
| Ejecutar | `bin\FutbolJuego.exe` |
| Limpiar | `mingw32-make clean` |
| Compilar + Ejecutar | `mingw32-make && bin\FutbolJuego.exe` |
| Ver ayuda | `mingw32-make help` |

---

**Generado con:** Makefile profesional para MinGW en Windows  
**Fecha:** 2026-06-09  
**Estado:** ✅ Compilación y enlace verificados  

---

