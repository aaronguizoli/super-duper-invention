#ifndef ALUNO_HPP
#define ALUNO_HPP
#include "Usuario.hpp"

namespace ufmg_carona {
    class Aluno : public Usuario {
    private: std::string _curso;
    public:
        // Construtor atualizado para repassar os novos parâmetros para o construtor base de Usuario
        Aluno(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento, std::string endereco, std::string email, std::string senha, Genero genero, std::string curso, bool deseja_motorista);
        std::string get_vinculo() const override;
    };
}
#endif