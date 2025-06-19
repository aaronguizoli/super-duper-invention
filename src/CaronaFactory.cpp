#include "CaronaFactory.hpp"
#include "Usuario.hpp" // Para Usuario*
// #include <memory> // Removido

namespace ufmg_carona {
    // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
    Carona CaronaFactory::criar_carona(std::string o, std::string d, std::string dt, Usuario* m, bool am, TipoCarona t) {
        // Passa o ponteiro bruto diretamente para o construtor da Carona
        return Carona(o, d, dt, m, am, t);
    }
}