# 🛠️ MinGW32-Make: Guía de Uso en Windows

## ¿Qué es mingw32-make?

**mingw32-make** es la herramienta Make que viene con MinGW. Es compatible con Makefiles estándar y es el equivalente de Windows a `make` en sistemas Unix/Linux.

---

## 🔍 ¿Cuál versión tengo instalada?

### Verificar versión
```bash
mingw32-make --version
```

### Verificar ubicación
```bash
where mingw32-make
```

**Ubicación típica en tu sistema:**
```
C:\msys64\mingw64\bin\mingw32-make.exe
```

---

## 📍 Agregar mingw32-make a tu PATH (Opcional)

Si mingw32-make no funciona desde la terminal, necesitas agregar MinGW a tu PATH.

### En Windows (Permanentemente):

1. Abre **Variables de entorno**
   - Presiona `Win + Pause` o busca "Variables de entorno" en el menú de Inicio

2. Haz clic en **"Variables de entorno..."**

3. En la sección **"Variables del sistema"**, selecciona **"Path"** y haz clic en **"Editar"**

4. Agrega esta línea:
   ```
   C:\msys64\mingw64\bin
   ```

5. Haz clic en **"Aceptar"** para guardar

6. **Reinicia tu terminal** (PowerShell, CMD, etc.)

7. Verifica que funcione:
   ```bash
   mingw32-make --version
   ```

---

## 🚀 Cómo usar mingw32-make desde diferentes Shells

### **1️⃣ PowerShell** (Recomendado en Windows)

```powershell
# Compilar
mingw32-make

# Compilar + Ejecutar
mingw32-make && .\bin\FutbolJuego.exe

# Limpiar
mingw32-make clean

# Ver ayuda
mingw32-make help
```

**Nota:** PowerShell es shell moderno de Windows, muy recomendado para desarrollo.

---

### **2️⃣ CMD (Símbolo del sistema)**

```cmd
REM Compilar
mingw32-make

REM Compilar + Ejecutar
mingw32-make && bin\FutbolJuego.exe

REM Limpiar
mingw32-make clean
```

**Nota:** CMD es el shell clásico de Windows, funciona pero es menos flexible.

---

### **3️⃣ Git Bash** (Si tienes Git instalado)

```bash
# Compilar
mingw32-make

# Compilar + Ejecutar
mingw32-make && ./bin/FutbolJuego.exe

# Limpiar
mingw32-make clean
```

**Nota:** Git Bash es POSIX-compatible, ideal si vienes de Linux.

---

### **4️⃣ MSYS2 Terminal** (Incluido en MinGW)

```bash
# Compilar
mingw32-make

# Compilar + Ejecutar
mingw32-make && bin/FutbolJuego.exe

# Limpiar
mingw32-make clean
```

**Nota:** Es la shell nativa de MSYS2/MinGW, altamente compatible.

---

## 📋 Tablas de Referencia Rápida

### Windows PowerShell vs CMD vs Bash

| Acción | PowerShell | CMD | Git Bash |
|--------|-----------|-----|----------|
| Compilar | `mingw32-make` | `mingw32-make` | `mingw32-make` |
| Ejecutar | `.\bin\FutbolJuego.exe` | `bin\FutbolJuego.exe` | `./bin/FutbolJuego.exe` |
| Compilar + Ejecutar | `mingw32-make && .\bin\FutbolJuego.exe` | `mingw32-make && bin\FutbolJuego.exe` | `mingw32-make && ./bin/FutbolJuego.exe` |
| Listar archivos | `Get-ChildItem` | `dir` | `ls` |
| Cambiar directorio | `cd ruta` | `cd ruta` | `cd ruta` |

---

## 🎯 Flujo de Trabajo Completo (Copia y Pega)

### PowerShell
```powershell
# 1. Navega al proyecto
cd "C:\Users\Karla\OneDrive\Documentos\GitHub\The-Last-Dance-"

# 2. Verifica que mingw32-make existe
mingw32-make --version

# 3. Compila
mingw32-make

# 4. Ejecuta
.\bin\FutbolJuego.exe

# 5. (Opcional) Para limpiar antes de GitHub
mingw32-make clean
```

### CMD
```cmd
REM 1. Navega al proyecto
cd "C:\Users\Karla\OneDrive\Documentos\GitHub\The-Last-Dance-"

REM 2. Verifica que mingw32-make existe
mingw32-make --version

REM 3. Compila
mingw32-make

REM 4. Ejecuta
bin\FutbolJuego.exe

REM 5. (Opcional) Para limpiar antes de GitHub
mingw32-make clean
```

### Git Bash
```bash
# 1. Navega al proyecto
cd "/c/Users/Karla/OneDrive/Documentos/GitHub/The-Last-Dance-"

# 2. Verifica que mingw32-make existe
mingw32-make --version

# 3. Compila
mingw32-make

# 4. Ejecuta
./bin/FutbolJuego.exe

# 5. (Opcional) Para limpiar antes de GitHub
mingw32-make clean
```

---

## 🚀 Alias útiles (PowerShell)

Si usas PowerShell, puedes crear alias para los comandos más frecuentes.

### Crear alias temporal (solo esta sesión)
```powershell
Set-Alias make mingw32-make
make          # Ahora funciona como mingw32-make
make clean    # Limpiar
```

### Crear alias permanente (todas las sesiones)

1. Abre PowerShell **como administrador**

2. Crea un perfil de PowerShell si no existe:
   ```powershell
   if (!(Test-Path $PROFILE)) { New-Item -Path $PROFILE -Type File -Force }
   ```

3. Abre el perfil en un editor:
   ```powershell
   notepad $PROFILE
   ```

4. Agrega esta línea al final:
   ```powershell
   Set-Alias make mingw32-make
   ```

5. Guarda y cierra Notepad

6. Reinicia PowerShell

7. Ahora funciona:
   ```powershell
   make
   make clean
   ```

---

## 🐛 Solucionar Problemas

### Error: "mingw32-make: command not found"

**Solución 1: Usar ruta completa**
```bash
C:\msys64\mingw64\bin\mingw32-make.exe all
```

**Solución 2: Agregar MinGW a PATH (ver sección anterior)**

**Solución 3: Usar make en su lugar**
```bash
make
```
(Si tienes `make` instalado directamente)

---

### Error: "Makefile not found"

Asegúrate de estar en la carpeta correcta:
```bash
cd "C:\Users\Karla\OneDrive\Documentos\GitHub\The-Last-Dance-"
dir Makefile    # Debe existir
mingw32-make
```

---

### El ejecutable no se crea

1. Verifica que no hay errores de compilación
2. Limpia y recompila:
   ```bash
   mingw32-make clean
   mingw32-make -B    # Fuerza compilación completa
   ```

3. Verifica que `bin/` se creó:
   ```bash
   dir bin
   ```

---

## 🔗 Referencias Útiles

### Variables automáticas en Makefiles

| Variable | Significado |
|----------|-------------|
| `$@` | Nombre del target (archivo a crear) |
| `$<` | Primera dependencia |
| `$^` | Todas las dependencias |
| `$?` | Dependencias más nuevas que el target |

### Directives útiles

```makefile
.PHONY: target      # Target que no genera archivo
@echo "mensaje"     # Imprime sin mostrar el comando
@mkdir -p dir       # Crea directorio sin mostrar
2>/dev/null         # Suprime mensajes de error
```

---

## 📝 Notas Importantes

1. **MinGW vs MSVC**: Estamos usando MinGW (GCC), no MSVC. MinGW es más compatible con código POSIX/Linux.

2. **MSYS2 Shell**: MinGW viene con MSYS2, que es una shell POSIX-compatible. Esto permite usar comandos similares a Linux.

3. **Rutas**: En MSYS/Git Bash, puedes usar `/c/Users/...` en lugar de `C:\Users\...`.

4. **Compilación Incremental**: MinGW-Make solo recompila archivos que han cambiado, ahorrando tiempo.

---

## ✅ Verificación Final

Para confirmar que todo está configurado correctamente:

```bash
mingw32-make --version       # Ver versión
mingw32-make info            # Ver información del proyecto
mingw32-make                 # Compilar
mingw32-make help            # Ver todas las reglas
```

---

**¡Listo para compilar desde terminal con Makefile! ⚽**
