#include "CaronaFactory.hpp"
#include "Usuario.hpp"
#include "Veiculo.hpp"
#include "Zona.hpp" // Adicionado para usar Zona

namespace ufmg_carona {
    // Implementacao do construtor atualizado
    Carona CaronaFactory::criar_carona(std::string o_nome, std::string d_nome, Zona o_zona, Zona d_zona, UFMGPosicao ufmg_pos, std::string dt, Usuario* m, Veiculo* vu, bool am, TipoCarona t) {
        return Carona(o_nome, d_nome, o_zona, d_zona, ufmg_pos, dt, m, vu, am, t);
    }
}