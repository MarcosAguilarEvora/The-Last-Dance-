#include "juego.hpp"
#include <cstdlib>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#endif

void prepararDirectorioDelJuego() {
#ifdef _WIN32
    char rutaExe[MAX_PATH];
    DWORD len = GetModuleFileNameA(nullptr, rutaExe, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) return;

    std::string ruta(rutaExe);
    std::size_t slash = ruta.find_last_of("\\/");
    if (slash == std::string::npos) return;

    std::string carpeta = ruta.substr(0, slash);
    std::size_t slashRaiz = carpeta.find_last_of("\\/");
    if (slashRaiz != std::string::npos && carpeta.substr(slashRaiz + 1) == "bin") {
        carpeta = carpeta.substr(0, slashRaiz);
    }

    SetCurrentDirectoryA(carpeta.c_str());
#endif
}

int main() {
#ifdef _WIN32
    if (std::getenv("ALSOFT_DRIVERS") == nullptr) {
        _putenv("ALSOFT_DRIVERS=dsound,winmm,null");
    }
#endif
    prepararDirectorioDelJuego();

    std::ofstream log("game_log.txt", std::ios::app);
    log << "Iniciando juego" << std::endl;

    Juego g;
    log << "Juego creado" << std::endl;
    g.Iniciar();
    log << "Juego cerrado normalmente" << std::endl;
    return 0;
}
