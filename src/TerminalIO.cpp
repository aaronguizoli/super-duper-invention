#include "TerminalIO.hpp"
#include "Zona.hpp" // Para os enums Zona e UFMGPosicao
#include "Genero.hpp" // Para o enum Genero
#include "Utilitarios.hpp" // Para utilitários de Zona e UFMGPosicao to string (opcional)

#include <iostream>
#include <limits> // Para numeric_limits
#include <map>    // Para mapeamentos de Zona/UFMGPosicao, se não estiverem em Utilitarios
#include <string>

namespace ufmg_carona {

    void TerminalIO::exibirMenuInicialNaoLogado() const {
        std::cout << "\n--- Menu Principal ---" << std::endl;
        std::cout << "(1) Cadastro | (2) Login | (0) Sair" << std::endl;
    }

    void TerminalIO::exibirMenuLogado(const std::string& nome_usuario) const {
        std::cout << "\nLogado como " << nome_usuario << std::endl;
        std::cout << "(1) Perfil | (2) Passageiro | (3) Motorista | (4) Avaliacoes | (5) Logout | (6) Sair" << std::endl;
    }

    void TerminalIO::exibirMenuPassageiro() const {
        std::cout << "\n--- Menu Passageiro ---" << std::endl;
        std::cout << "(1) Solicitar Carona | (2) Status das Minhas Solicitacoes | (0) Voltar" << std::endl;
    }

    void TerminalIO::exibirMenuMotorista() const {
        std::cout << "\n--- Menu Motorista ---" << std::endl;
        std::cout << "(1) Oferecer Carona | (2) Gerenciar Caronas | (3) Cadastrar Veiculo | (0) Voltar" << std::endl;
    }

    void TerminalIO::exibirMenuPerfil() const {
        std::cout << "\n--- Menu Perfil ---" << std::endl;
        std::cout << "Escolha a opcao:" << std::endl;
        std::cout << "(1) Editar Perfil | (2) Gerenciar Veiculos (Motorista) | (0) Voltar" << std::endl;
    }

    int TerminalIO::coletarIntInput(const std::string& prompt, int min_val, int max_val) const {
        int valor;
        while (true) {
            std::cout << prompt;
            std::cin >> valor;
            if (std::cin.fail()) {
                std::cout << "Entrada invalida. Digite um numero." << std::endl;
                limparBuffer();
            } else if (valor < min_val || valor > max_val) {
                std::cout << "Numero fora do intervalo permitido (" << min_val << "-" << max_val << ")."
                          << (min_val == 0 ? " Digite 0 para voltar." : "") << std::endl;
                limparBuffer();
            } else {
                limparBuffer();
                return valor;
            }
        }
    }

    std::string TerminalIO::coletarStringInput(const std::string& prompt) const {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }

    Zona TerminalIO::coletarZonaInput(const std::string& prompt) const {
        std::cout << prompt << std::endl;
        Utilitarios util; // Instancia de Utilitarios para usar as funcoes de conversao
        for (const auto& pair : util.getIntParaZona()) {
            std::cout << "(" << pair.first << ") " << util.zonaToString(pair.second) << std::endl;
        }
        int escolha = coletarIntInput("> ", 1, util.getIntParaZona().size());
        return util.getIntParaZona().at(escolha);
    }

    UFMGPosicao TerminalIO::coletarUfmgPosicaoInput(const std::string& prompt) const {
        std::cout << prompt << std::endl;
        Utilitarios util;
        for (const auto& pair : util.getIntParaUfmgPosicao()) {
            std::cout << "(" << pair.first << ") " << util.ufmgPosicaoToString(pair.second) << std::endl;
        }
        int escolha = coletarIntInput("> ", 1, util.getIntParaUfmgPosicao().size());
        return util.getIntParaUfmgPosicao().at(escolha);
    }

    Genero TerminalIO::coletarGeneroInput(const std::string& prompt) const {
        int gen_int = coletarIntInput(prompt, 0, 3);
        return static_cast<Genero>(gen_int);
    }

    char TerminalIO::confirmarAcao(const std::string& prompt) const {
        char resposta;
        std::cout << prompt;
        std::cin >> resposta;
        limparBuffer();
        return resposta;
    }

    void TerminalIO::exibirMensagem(const std::string& msg) const {
        std::cout << msg << std::endl;
    }

    void TerminalIO::exibirErro(const std::string& erro_msg) const {
        std::cerr << "ERRO: " << erro_msg << std::endl;
    }

    void TerminalIO::exibirAviso(const std::string& aviso_msg) const {
        std::cout << "AVISO: " << aviso_msg << std::endl;
    }

    void TerminalIO::limparBuffer() const {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

} // namespace ufmg_carona