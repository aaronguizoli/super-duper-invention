#include "Pagamento.hpp"
#include <cmath>

namespace ufmg_carona {
    const double Pagamento::TAXA_BASE = 2.00;
    const double Pagamento::ADICIONAL_POR_ZONA = 1.50;

    Pagamento::Pagamento(Zona origem, Zona destino) : _efetuado(false) {
        this->_valor = calcular_valor(origem, destino);
    }
    double Pagamento::calcular_valor(Zona origem, Zona destino) {
        int z1 = static_cast<int>(origem);
        int z2 = static_cast<int>(destino);
        int diferenca_zonas = std::abs(z1 - z2);
        if (diferenca_zonas == 0) return TAXA_BASE;
        return TAXA_BASE + (diferenca_zonas * ADICIONAL_POR_ZONA);
    }
    double Pagamento::get_valor() const { return _valor; }
}