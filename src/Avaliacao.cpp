#include "Avaliacao.hpp"
#include "Usuario.hpp" 
#include "Carona.hpp"
#include <iostream>

namespace ufmg_carona {
    Avaliacao::Avaliacao(int n, std::string c, Usuario* av, Usuario* ad, Carona* cr)
        : _nota(n), _comentario(c), _avaliador(av), _avaliado(ad), _carona_referencia(cr) {}
    int Avaliacao::get_nota() const { return _nota; }
    const std::string& Avaliacao::get_comentario() const { return _comentario; }
    Usuario* Avaliacao::get_avaliador() const { return _avaliador; }
    Usuario* Avaliacao::get_avaliado() const { return _avaliado; }
    Carona* Avaliacao::get_carona_referencia() const { return _carona_referencia; }
}