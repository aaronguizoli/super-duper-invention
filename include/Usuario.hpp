#ifndef USUARIO_HPP
#define USUARIO_HPP
#include <string>
#include <vector>
#include "Veiculo.hpp"
#include "Rotina.hpp"
#include "Notificacao.hpp"
#include "Avaliacao.hpp"
#include "Genero.hpp"

namespace ufmg_carona {
    class Usuario {
    protected:
        std::string _nome, _cpf, _email, _senha;
        std::string _telefone;
        std::string _data_nascimento;
        Genero _genero;
        std::vector<Avaliacao*> _avaliacoes_recebidas;
        std::vector<Notificacao> _notificacoes;
        Veiculo _veiculo;
        bool _possui_veiculo;
        bool _deseja_oferecer_caronas;

    public:
        Usuario(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento, std::string email, std::string senha, Genero genero, bool deseja_oferecer_caronas);

        virtual ~Usuario();

        virtual std::string get_vinculo() const = 0;
        // NOVO: Getters para salvar dados
        const std::string& get_email() const;
        const std::string& get_senha() const;
        Genero get_genero() const;
        virtual std::string get_vinculo_raw() const = 0; // Para obter "aluno" ou "funcionario"
        virtual std::string get_detalhe_vinculo() const = 0; // Para obter "curso" ou "setor"

        const std::string& get_cpf() const;
        const std::string& get_nome() const;
        bool verificar_senha(const std::string& senha) const;
        void cadastrar_veiculo(const Veiculo& veiculo);
        bool is_motorista() const;
        const Veiculo& get_veiculo() const;
        double get_media_avaliacoes() const;
        void adicionar_avaliacao_recebida(Avaliacao* avaliacao);
        void imprimir_perfil() const;

        const std::string& get_telefone() const;
        const std::string& get_data_nascimento() const;
        bool get_deseja_oferecer_caronas() const;
    };
}
#endif