#ifndef ALUNO_HPP
#define ALUNO_HPP
#include "Usuario.hpp"

namespace ufmg_carona {
    class Aluno : public Usuario {
    private: std::string _curso;
    public: Aluno(std::string nome, std::string cpf, std::string email, std::string senha, Genero genero, std::string curso); std::string get_vinculo() const override;
    };
}
#endif