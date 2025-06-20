#include "Veiculo.hpp"
#include <iostream>
namespace ufmg_carona {
    Veiculo::Veiculo(const std::string& p, const std::string& ma, const std::string& mo, const std::string& c, int l) : _placa(p), _marca(ma), _modelo(mo), _cor(c), _total_de_lugares(l) {}
    Veiculo::Veiculo() : _total_de_lugares(0) {}
    int Veiculo::get_lugares() const { return _total_de_lugares; }
    void Veiculo::exibir_info() const { std::cout << "  Veiculo: " << _marca << " " << _modelo << ", Cor: " << _cor << ", Placa: " << _placa << std::endl; }
}