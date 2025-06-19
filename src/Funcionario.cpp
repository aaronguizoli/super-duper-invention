#include "Funcionario.hpp"
namespace ufmg_carona {
    Funcionario::Funcionario(std::string n, std::string c, std::string e, std::string s, Genero g, std::string set) : Usuario(n, c, e, s, g), _setor(set) {}
    std::string Funcionario::get_vinculo() const { return "Funcionario do setor " + _setor; }
}