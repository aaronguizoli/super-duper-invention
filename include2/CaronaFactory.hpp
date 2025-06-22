#ifndef CARONA_FACTORY_HPP
#define CARONA_FACTORY_HPP
#include "Carona.hpp"
#include <string>

namespace ufmg_carona {
    class Usuario;
    class CaronaFactory {
    public:
        static Carona criar_carona(std::string origem, std::string destino, std::string data, Usuario* motorista, bool apenas_mulheres, TipoCarona tipo);
    };
}
#endif