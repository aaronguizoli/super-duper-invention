#include "Avaliacao.hpp"
#include "Usuario.hpp"
namespace ufmg_carona {
    Avaliacao::Avaliacao(int n, std::string c, std::shared_ptr<Usuario> av, std::shared_ptr<Usuario> ad) : _nota(n), _comentario(c), _avaliador(av), _avaliado(ad) {}
    int Avaliacao::get_nota() const { return _nota; }
}