#include "Veiculo.hpp"
#include <iostream>
namespace ufmg_carona {
    Veiculo::Veiculo(const std::string& p, const std::string& ma, const std::string& mo, const std::string& c, int l) : _placa(p), _marca(ma), _modelo(mo), _cor(c), _total_de_lugares(l) {}
    Veiculo::Veiculo() : _total_de_lugares(0) {}
    int Veiculo::get_lugares() const { return _total_de_lugares; }
    void Veiculo::exibir_info() const { std::cout << "  Veiculo: " << _marca << " " << _modelo << ", Cor: " << _cor << ", Placa: " << _placa << std::endl; }

    // NOVAS IMPLEMENTACOES DOS GETTERS
    const std::string& Veiculo::get_placa() const { return _placa; }
    const std::string& Veiculo::get_marca() const { return _marca; }
    const std::string& Veiculo::get_modelo() const { return _modelo; }
    const std::string& Veiculo::get_cor() const { return _cor; } // Corrigido: Adicionado "Veiculo::" aqui
}