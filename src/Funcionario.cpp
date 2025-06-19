#include "Funcionario.hpp"
#include "Usuario.hpp" // Incluir para garantir que Usuario esteja completo

namespace ufmg_carona {
    // Construtor atualizado para repassar os novos parâmetros para Usuario
    // A assinatura do construtor base Usuario foi modificada, esta também precisa.
    Funcionario::Funcionario(std::string n, std::string c, std::string tel, std::string dt_nasc, std::string end, std::string e, std::string s, Genero g, std::string set, bool deseja_motorista)
        : Usuario(n, c, tel, dt_nasc, end, e, s, g, deseja_motorista), _setor(set) {}

    std::string Funcionario::get_vinculo() const { return "Funcionario do setor " + _setor; }
}