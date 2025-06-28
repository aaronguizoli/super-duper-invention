#ifndef CARONA_FACTORY_HPP
#define CARONA_FACTORY_HPP
#include "Carona.hpp"
#include "Zona.hpp" 
#include <string>

namespace ufmg_carona {
    class Usuario;
    class Veiculo;

    class CaronaFactory {
    public:
        static Carona criar_carona(std::string origem_nome, std::string destino_nome, Zona origem_zona, Zona destino_zona, UFMGPosicao ufmg_posicao, std::string data, Usuario* motorista, Veiculo* veiculo_usado, bool apenas_mulheres, TipoCarona tipo);
    };
}
#endif