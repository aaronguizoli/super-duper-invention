#include "CaronaFactory.hpp"
#include "Usuario.hpp"
namespace ufmg_carona {
    Carona CaronaFactory::criar_carona(std::string o, std::string d, std::string dt, std::shared_ptr<Usuario> m, bool am, TipoCarona t) {
        return Carona(o, d, dt, m, am, t);
    }
}