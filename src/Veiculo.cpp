#include "Veiculo.hpp"
#include <iostream>

namespace ufmg_carona {
    Veiculo::Veiculo(const std::string& placa, const std::string& marca, const std::string& modelo, const std::string& cor, int lugares)
        : _placa(placa), _marca(marca), _modelo(modelo), _cor(cor), _total_de_lugares(lugares) {}
    Veiculo::Veiculo() : _total_de_lugares(0) {}
    const std::string& Veiculo::get_placa() const { return _placa; }
    int Veiculo::get_lugares() const { return _total_de_lugares; }
    void Veiculo::exibir_info() const {
        std::cout << "  Veiculo: " << _marca << " " << _modelo << ", Cor: " << _cor << ", Placa: " << _placa << std::endl;
    }
}