#include "Avaliacao.hpp"
#include "Usuario.hpp" // Certifique-se de que Usuario.hpp está incluído para o tipo Usuario*
// #include <memory> // Removido

namespace ufmg_carona {
    // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
    Avaliacao::Avaliacao(int n, std::string c, Usuario* av, Usuario* ad)
        : _nota(n), _comentario(c), _avaliador(av), _avaliado(ad) {}
    int Avaliacao::get_nota() const { return _nota; }
    // Não é necessário destrutor para Avaliacao, pois ela não é proprietária de _avaliador ou _avaliado.
    // Eles são apenas observadores, e a memória é gerenciada por outra classe (Sistema e seus Usuarios).
}