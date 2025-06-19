#include "Funcionario.hpp"

namespace ufmg_carona {
    // Construtor atualizado para repassar os novos parâmetros para Usuario
    Funcionario::Funcionario(std::string n, std::string c, std::string tel, std::string dt_nasc, std::string end, std::string e, std::string s, Genero g, std::string set, bool deseja_motorista)
        : Usuario(n, c, tel, dt_nasc, end, e, s, g, deseja_motorista), _setor(set) {} // NOVO: Passando tel, dt_nasc, end, deseja_motorista

    std::string Funcionario::get_vinculo() const { return "Funcionario do setor " + _setor; }
}