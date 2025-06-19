#ifndef SOLICITACAO_HPP
#define SOLICITACAO_HPP
// #include <memory> // Removido
#include <string>

namespace ufmg_carona {
    class Usuario; // Declaração antecipada
    class Carona; // Declaração antecipada
    enum class StatusSolicitacao { PENDENTE, ACEITA, RECUSADA };

    class Solicitacao {
    private:
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Usuario* _passageiro; // Solicitacao não é proprietária do passageiro
        Carona* _carona_alvo; // Já era ponteiro bruto, Solicitacao não é proprietária da carona
        StatusSolicitacao _status;

    public:
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Solicitacao(Usuario* passageiro, Carona* carona);
        // Não é necessário destrutor customizado, pois Solicitacao não é proprietária dos ponteiros brutos que armazena.
        // Eles são gerenciados pela classe Sistema.
        void aceitar();
        void recusar();

        // Getters
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Usuario* get_passageiro() const;
        Carona* get_carona() const;
        StatusSolicitacao get_status() const;
        std::string get_status_string() const;

        // Métodos de exibição
        void exibir_info() const;
        void exibir_para_motorista() const;
    };
}
#endif