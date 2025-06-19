#ifndef FUNCIONARIO_HPP
#define FUNCIONARIO_HPP
#include "Usuario.hpp"

namespace ufmg_carona {
    class Funcionario : public Usuario {
    private: std::string _setor;
    public: Funcionario(std::string nome, std::string cpf, std::string email, std::string senha, Genero genero, std::string setor); std::string get_vinculo() const override;
    };
}
#endif