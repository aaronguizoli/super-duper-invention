#include "Avaliacao.hpp"
#include "Usuario.hpp"

namespace ufmg_carona {
    Avaliacao::Avaliacao(int nota, std::string comentario, std::shared_ptr<Usuario> avaliador, std::shared_ptr<Usuario> avaliado)
        : _nota(nota), _comentario(comentario), _avaliador(avaliador), _avaliado(avaliado) {}
    int Avaliacao::get_nota() const { return _nota; }
}