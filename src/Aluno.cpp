#include "Aluno.hpp"
#include "Usuario.hpp" // Incluir para garantir que Usuario esteja completo

namespace ufmg_carona {
    // Construtor atualizado para repassar os novos parâmetros para Usuario
    // A assinatura do construtor base Usuario foi modificada, esta também precisa.
    Aluno::Aluno(std::string n, std::string c, std::string tel, std::string dt_nasc, std::string end, std::string e, std::string s, Genero g, std::string cur, bool deseja_motorista)
        : Usuario(n, c, tel, dt_nasc, end, e, s, g, deseja_motorista), _curso(cur) {}

    std::string Aluno::get_vinculo() const { return "Aluno do curso de " + _curso; }
}