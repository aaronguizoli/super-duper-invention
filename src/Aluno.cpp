#include "Aluno.hpp"
namespace ufmg_carona {
    Aluno::Aluno(std::string n, std::string c, std::string e, std::string s, Genero g, std::string cur) : Usuario(n, c, e, s, g), _curso(cur) {}
    std::string Aluno::get_vinculo() const { return "Aluno do curso de " + _curso; }
}