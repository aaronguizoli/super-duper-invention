#include "Aluno.hpp"
#include "Usuario.hpp"

namespace ufmg_carona {
    Aluno::Aluno(std::string n, std::string c, std::string tel, std::string dt_nasc, std::string e, std::string s, Genero g, std::string cur, bool deseja_motorista)
        : Usuario(n, c, tel, dt_nasc, e, s, g, deseja_motorista), _curso(cur) {}

    std::string Aluno::get_vinculo() const { return "Aluno do curso de " + _curso; }

    // Implementação dos novos getters virtuais para Aluno
    std::string Aluno::get_vinculo_raw() const { return "aluno"; }
    std::string Aluno::get_detalhe_vinculo() const { return _curso; }
}