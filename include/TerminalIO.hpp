#ifndef TERMINAL_IO_HPP
#define TERMINAL_IO_HPP

#include <string>
#include <vector>

namespace ufmg_carona {
    enum class Zona;
    enum class UFMGPosicao;
    enum class Genero;

    class TerminalIO {
    public:
        void exibirMenuInicialNaoLogado() const;
        void exibirMenuLogado(const std::string& nome_usuario) const;
        void exibirMenuPassageiro() const;
        void exibirMenuMotorista() const;
        void exibirMenuPerfil() const;

        int coletarIntInput(const std::string& prompt, int min_val, int max_val) const;
        std::string coletarStringInput(const std::string& prompt) const;
        Zona coletarZonaInput(const std::string& prompt) const;
        UFMGPosicao coletarUfmgPosicaoInput(const std::string& prompt) const;
        Genero coletarGeneroInput(const std::string& prompt) const;
        char confirmarAcao(const std::string& prompt) const;

        void exibirMensagem(const std::string& msg) const;
        void exibirErro(const std::string& erro_msg) const;
        void exibirAviso(const std::string& aviso_msg) const;
        void limparBuffer() const;
    };
}
#endif