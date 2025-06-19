#ifndef EXCECOES_HPP
#define EXCECOES_HPP
#include <stdexcept>
#include <string>

namespace ufmg_carona {
    class AppExcecao : public std::runtime_error {
    public:
        explicit AppExcecao(const std::string& message) : std::runtime_error(message) {}
    };
    class AutenticacaoFalhouException : public AppExcecao {
    public:
        AutenticacaoFalhouException() : AppExcecao("CPF ou senha invalidos.") {}
    };
    class CaronaLotadaException : public AppExcecao {
    public:
        CaronaLotadaException() : AppExcecao("A carona selecionada nao possui mais vagas disponiveis.") {}
    };
    class ComandoInvalidoException : public AppExcecao {
    public:
        ComandoInvalidoException(const std::string& comando) : AppExcecao("Comando '" + comando + "' nao reconhecido.") {}
    };
}
#endif