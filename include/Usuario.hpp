#ifndef USUARIO_HPP
#define USUARIO_HPP
#include <string>
#include <vector>
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
        std::string _vinculo_tipo;
        std::string _detalhe_vinculo;

    public:
        Usuario(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento, std::string email, std::string senha, Genero genero, std::string vinculo_tipo, std::string detalhe_vinculo);

        virtual ~Usuario();

        std::string get_vinculo() const;
        std::string get_vinculo_raw() const;
        std::string get_detalhe_vinculo() const;

        const std::string& get_email() const;
        const std::string& get_senha() const;
        Genero get_genero() const;


        const std::string& get_cpf() const;
        const std::string& get_nome() const;
        bool verificar_senha(const std::string& senha) const;
        
        virtual bool is_motorista() const;
        
        double get_media_avaliacoes() const;
        void adicionar_avaliacao_recebida(Avaliacao* avaliacao);
        virtual void imprimir_perfil() const;

        const std::string& get_telefone() const;
        const std::string& get_data_nascimento() const;

        void set_email(const std::string& email);
        void set_telefone(const std::string& telefone);
        void set_senha(const std::string& senha);
        void set_genero(Genero genero);
    };
}
#endif