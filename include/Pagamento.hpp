#ifndef PAGAMENTO_HPP
#define PAGAMENTO_HPP

#include "Zona.hpp"

namespace ufmg_carona {
    /**
     * @class Pagamento
     * @brief Gerencia o cálculo e o estado de um pagamento, simulando uma taxa variável.
     */
    class Pagamento {
    private:
        // ATRIBUTOS ADICIONADOS AQUI
        double _valor;
        bool _efetuado;
        static const double TAXA_BASE;
        static const double ADICIONAL_POR_ZONA;

    public:
        Pagamento(Zona origem, Zona destino);
        
        static double calcular_valor(Zona origem, Zona destino);
        
        // MÉTODOS GETTERS ADICIONADOS AQUI
        double get_valor() const;
        bool is_efetuado() const;

        // MÉTODO SETTER ADICIONADO
        void efetuar_pagamento();
    };
}

#endif // PAGAMENTO_HPP