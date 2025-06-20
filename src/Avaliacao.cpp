#include "Avaliacao.hpp"
#include "Usuario.hpp" // Incluido para a definicao completa de Usuario*
#include <iostream>

namespace ufmg_carona {
    Avaliacao::Avaliacao(int n, std::string c, Usuario* av, Usuario* ad)
        : _nota(n), _comentario(c), _avaliador(av), _avaliado(ad) {}
    int Avaliacao::get_nota() const { return _nota; }
}