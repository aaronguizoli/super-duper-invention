#include "Sistema.hpp"
#include "Excecoes.hpp"
#include <iostream>

int main() {
    ufmg_carona::Sistema app;
    try {
        app.executar();
    } catch (const ufmg_carona::AppExcecao& e) {
        if (std::string(e.what()) == "Sair") {
            
        } else {
            std::cerr << "ERRO FATAL: " << e.what() << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "ERRO INESPERADO: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}