#include "Pagamento.hpp"
#include <cmath> // Para std::abs

namespace ufmg_carona {
    // Definindo as constantes estáticas
    const double Pagamento::TAXA_BASE = 2.00;
    const double Pagamento::ADICIONAL_POR_ZONA = 1.50;

    // O construtor agora inicializa todos os membros
    Pagamento::Pagamento(Zona origem, Zona destino) : _efetuado(false) {
        this->_valor = calcular_valor(origem, destino);
    }

    double Pagamento::calcular_valor(Zona origem, Zona destino) {
        int z1 = static_cast<int>(origem);
        int z2 = static_cast<int>(destino);
        int diferenca_zonas = std::abs(z1 - z2);

        if (diferenca_zonas == 0) {
            return TAXA_BASE;
        }
        
        return TAXA_BASE + (diferenca_zonas * ADICIONAL_POR_ZONA);
    }

    // IMPLEMENTAÇÃO DOS MÉTODOS GETTERS E SETTERS
    double Pagamento::get_valor() const {
        return this->_valor;
    }

    bool Pagamento::is_efetuado() const {
        return this->_efetuado;
    }

    void Pagamento::efetuar_pagamento() {
        this->_efetuado = true;
    }
}