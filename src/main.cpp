#include "Sistema.hpp"
#include "Excecoes.hpp" // Incluir Excecoes para capturar a excecao de saida
#include <iostream>

int main() {
    ufmg_carona::Sistema app;
    try {
        app.executar();
    } catch (const ufmg_carona::AppExcecao& e) {
        // Captura a excecao de saida
        if (std::string(e.what()) == "Sair") {
            // Se for a excecao de saida, nao faz nada, permite que o programa termine.
            // O destrutor de Sistema já imprimirá a mensagem de finalização.
        } else {
            // Outras excecoes AppExcecao sao tratadas como erro
            std::cerr << "ERRO FATAL: " << e.what() << std::endl;
            return 1; // Retorna com erro
        }
    } catch (const std::exception& e) {
        std::cerr << "ERRO INESPERADO: " << e.what() << std::endl;
        return 1; // Retorna com erro
    }
    return 0; // Retorno limpo
}