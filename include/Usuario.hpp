#ifndef USUARIO_HPP
#define USUARIO_HPP
#include <string>
#include <vector>
#include <memory>
#include "Veiculo.hpp"
#include "Rotina.hpp"
#include "Notificacao.hpp"
#include "Avaliacao.hpp"
#include "Genero.hpp"

namespace ufmg_carona {
    class Usuario {
    protected:
        std::string _nome, _cpf, _email, _senha;
        std::string _telefone;       // NOVO: Telefone do usuário
        std::string _data_nascimento; // NOVO: Data de nascimento do usuário
        std::string _endereco;       // NOVO: Endereço do usuário
        Genero _genero;
        std::vector<std::shared_ptr<Avaliacao>> _avaliacoes_recebidas;
        std::vector<Notificacao> _notificacoes;
        Veiculo _veiculo;
        bool _possui_veiculo;
        bool _deseja_oferecer_caronas; // NOVO: Flag para indicar se o usuário deseja ser motorista

    public:
        // Construtor atualizado para incluir novas informações
        Usuario(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento, std::string endereco, std::string email, std::string senha, Genero genero, bool deseja_oferecer_caronas);
        virtual ~Usuario() = default;
        virtual std::string get_vinculo() const = 0;
        const std::string& get_cpf() const;
        const std::string& get_nome() const;
        bool verificar_senha(const std::string& senha) const;
        void cadastrar_veiculo(const Veiculo& veiculo);
        bool is_motorista() const;
        const Veiculo& get_veiculo() const;
        double get_media_avaliacoes() const;
        void adicionar_avaliacao_recebida(std::shared_ptr<Avaliacao> avaliacao);
        void imprimir_perfil() const;

        // NOVOS Getters
        const std::string& get_telefone() const;
        const std::string& get_data_nascimento() const;
        const std::string& get_endereco() const;
        bool get_deseja_oferecer_caronas() const;
    };
}
#endif