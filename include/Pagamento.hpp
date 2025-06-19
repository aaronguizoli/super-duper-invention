#ifndef PAGAMENTO_HPP
#define PAGAMENTO_HPP
#include "Zona.hpp"

namespace ufmg_carona {
    class Pagamento {
    private:
        double _valor;
        bool _efetuado;
        static const double TAXA_BASE;
        static const double ADICIONAL_POR_ZONA;

    public:
        Pagamento(Zona origem, Zona destino);
        static double calcular_valor(Zona origem, Zona destino);
        double get_valor() const;
    };
}
#endif