#ifndef TERMINAL_IO_HPP
#define TERMINAL_IO_HPP

#include <string>
#include <vector> // Para coletarZonaInput e exibirMenus

namespace ufmg_carona {
    // Forward declarations de enums que serão usados
    enum class Zona;
    enum class UFMGPosicao;
    enum class Genero;

    class TerminalIO {
    public:
        // Métodos para exibição de menus
        void exibirMenuInicialNaoLogado() const;
        void exibirMenuLogado(const std::string& nome_usuario) const;
        void exibirMenuPassageiro() const;
        void exibirMenuMotorista() const;
        void exibirMenuPerfil() const; // Novo menu para perfil/veículos

        // Métodos para coleta de input
        int coletarIntInput(const std::string& prompt, int min_val, int max_val) const;
        std::string coletarStringInput(const std::string& prompt) const;
        Zona coletarZonaInput(const std::string& prompt) const;
        UFMGPosicao coletarUfmgPosicaoInput(const std::string& prompt) const;
        Genero coletarGeneroInput(const std::string& prompt) const; // Novo
        char confirmarAcao(const std::string& prompt) const; // Novo

        // Métodos para exibição de mensagens
        void exibirMensagem(const std::string& msg) const;
        void exibirErro(const std::string& erro_msg) const;
        void exibirAviso(const std::string& aviso_msg) const;
        void limparBuffer() const; // Adicionado para conveniência
    };
} // namespace ufmg_carona

#endif // TERMINAL_IO_HPP