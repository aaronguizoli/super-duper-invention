#include "CaronaFactory.hpp"
#include "Usuario.hpp"
#include "Veiculo.hpp"

namespace ufmg_carona {
    Carona CaronaFactory::criar_carona(std::string o, std::string d, std::string dt, Usuario* m, Veiculo* vu, bool am, TipoCarona t) {
        return Carona(o, d, dt, m, vu, am, t);
    }
}