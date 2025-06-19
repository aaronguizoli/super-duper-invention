#include "CaronaFactory.hpp"
#include "Usuario.hpp"

namespace ufmg_carona {
    // Implementação final e completa
    Carona CaronaFactory::criar_carona(std::string origem, std::string destino, std::string data, std::shared_ptr<Usuario> motorista, bool apenas_mulheres, TipoCarona tipo) {
        return Carona(origem, destino, data, motorista, apenas_mulheres, tipo);
    }
}