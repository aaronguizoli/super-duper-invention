#ifndef USUARIO_HPP
#define USUARIO_HPP
#include <string>
#include <vector>
// #include <memory> // Removido, pois não usaremos smart pointers
#include "Veiculo.hpp"
#include "Rotina.hpp"
#include "Notificacao.hpp"
#include "Avaliacao.hpp" // Avaliacao agora será ponteiro bruto
#include "Genero.hpp"

namespace ufmg_carona {
    class Usuario {
    protected:
        std::string _nome, _cpf, _email, _senha;
        std::string _telefone;       // Telefone do usuário
        std::string _data_nascimento; // Data de nascimento do usuário
        std::string _endereco;       // Endereço do usuário
        Genero _genero;
        // ALTERAÇÃO: De std::vector<std::shared_ptr<Avaliacao>> para std::vector<Avaliacao*>
        std::vector<Avaliacao*> _avaliacoes_recebidas; // Agora armazena ponteiros brutos
        std::vector<Notificacao> _notificacoes; // Mantém Notificacao como objeto direto
        Veiculo _veiculo;
        bool _possui_veiculo;
        bool _deseja_oferecer_caronas; // Flag para indicar se o usuário deseja ser motorista

    public:
        // Construtor atualizado para incluir novas informações
        Usuario(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento, std::string endereco, std::string email, std::string senha, Genero genero, bool deseja_oferecer_caronas);

        // NOVO: Destrutor virtual para liberar a memória das Avaliações.
        // É CRUCIAL que Usuario seja responsável por deletar as Avaliações que ele armazena.
        virtual ~Usuario();

        virtual std::string get_vinculo() const = 0;
        const std::string& get_cpf() const;
        const std::string& get_nome() const;
        bool verificar_senha(const std::string& senha) const;
        void cadastrar_veiculo(const Veiculo& veiculo);
        bool is_motorista() const;
        const Veiculo& get_veiculo() const;
        double get_media_avaliacoes() const;
        // ALTERAÇÃO: De std::shared_ptr<Avaliacao> para Avaliacao*
        void adicionar_avaliacao_recebida(Avaliacao* avaliacao); // Agora recebe ponteiro bruto
        void imprimir_perfil() const;

        // Novos Getters (não precisam de alteração)
        const std::string& get_telefone() const;
        const std::string& get_data_nascimento() const;
        const std::string& get_endereco() const;
        bool get_deseja_oferecer_caronas() const;
    };
}
#endif