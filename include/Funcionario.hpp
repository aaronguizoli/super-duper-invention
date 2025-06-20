#ifndef FUNCIONARIO_HPP
#define FUNCIONARIO_HPP
#include "Usuario.hpp"

namespace ufmg_carona {
    class Funcionario : public Usuario {
    private: std::string _setor;
    public:
        // Construtor atualizado: 'endereco' ja foi removido do construtor base de Usuario
        Funcionario(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento, std::string email, std::string senha, Genero genero, std::string setor, bool deseja_motorista);
        std::string get_vinculo() const override;
        // NOVO: Implementacao dos getters virtuais puros de Usuario
        std::string get_vinculo_raw() const override;
        std::string get_detalhe_vinculo() const override;
    };
}
#endif