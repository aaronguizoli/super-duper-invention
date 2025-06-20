#include "Funcionario.hpp"
#include "Usuario.hpp"

namespace ufmg_carona {
    Funcionario::Funcionario(std::string n, std::string c, std::string tel, std::string dt_nasc, std::string e, std::string s, Genero g, std::string set, bool deseja_motorista)
        : Usuario(n, c, tel, dt_nasc, e, s, g, deseja_motorista), _setor(set) {}

    std::string Funcionario::get_vinculo() const { return "Funcionario do setor " + _setor; }

    // Implementação dos novos getters virtuais para Funcionario
    std::string Funcionario::get_vinculo_raw() const { return "funcionario"; }
    std::string Funcionario::get_detalhe_vinculo() const { return _setor; }
}