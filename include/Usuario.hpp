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
        Genero _genero;
        std::vector<std::shared_ptr<Avaliacao>> _avaliacoes_recebidas;
        std::vector<Notificacao> _notificacoes;
        Veiculo _veiculo;
        bool _possui_veiculo;
    public:
        Usuario(std::string nome, std::string cpf, std::string email, std::string senha, Genero genero);
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
    };
}
#endif