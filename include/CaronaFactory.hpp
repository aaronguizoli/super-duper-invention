#ifndef CARONA_FACTORY_HPP
#define CARONA_FACTORY_HPP
#include "Carona.hpp"
// #include <memory> // Removido
#include <string>

namespace ufmg_carona {
    class Usuario; // Declaração antecipada
    class CaronaFactory {
    public:
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        static Carona criar_carona(std::string origem, std::string destino, std::string data, Usuario* motorista, bool apenas_mulheres, TipoCarona tipo);
    };
}
#endif