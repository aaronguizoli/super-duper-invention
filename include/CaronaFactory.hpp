#ifndef CARONA_FACTORY_HPP
#define CARONA_FACTORY_HPP
#include "Carona.hpp"
#include <string>

namespace ufmg_carona {
    class Usuario;
    class Veiculo; // Declaracao antecipada para o parametro veiculo_usado

    class CaronaFactory {
    public:
        // Construtor ATUALIZADO: Recebe Usuario* e Veiculo* para criar a carona
        static Carona criar_carona(std::string origem, std::string destino, std::string data, Usuario* motorista, Veiculo* veiculo_usado, bool apenas_mulheres, TipoCarona tipo);
    };
}
#endif