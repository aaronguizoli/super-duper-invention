#include "Aluno.hpp"

namespace ufmg_carona {
    Aluno::Aluno(std::string nome, std::string cpf, std::string email, std::string senha, Genero genero, std::string curso)
        : Usuario(nome, cpf, email, senha, genero), _curso(curso) {}
    std::string Aluno::get_vinculo() const { return "Aluno do curso de " + _curso; }
}