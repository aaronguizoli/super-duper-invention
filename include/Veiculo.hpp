#ifndef VEICULO_HPP
#define VEICULO_HPP
#include <string>
namespace ufmg_carona {
    class Veiculo {
    private: std::string _placa, _marca, _modelo, _cor; int _total_de_lugares;
    public:
        Veiculo(const std::string& placa, const std::string& marca, const std::string& modelo, const std::string& cor, int lugares);
        Veiculo(); int get_lugares() const; void exibir_info() const;
    };
}
#endif