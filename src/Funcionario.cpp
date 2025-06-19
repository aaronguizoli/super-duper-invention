#include "Funcionario.hpp"

namespace ufmg_carona {
    Funcionario::Funcionario(std::string nome, std::string cpf, std::string email, std::string senha, Genero genero, std::string setor)
        : Usuario(nome, cpf, email, senha, genero), _setor(setor) {}
    std::string Funcionario::get_vinculo() const { return "Funcionario do setor " + _setor; }
}