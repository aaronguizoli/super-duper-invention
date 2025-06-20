#ifndef SOLICITACAO_HPP
#define SOLICITACAO_HPP
#include <string>

namespace ufmg_carona {
    class Usuario;
    class Carona;
    enum class StatusSolicitacao { PENDENTE, ACEITA, RECUSADA };

    class Solicitacao {
    private:
        Usuario* _passageiro;
        Carona* _carona_alvo;
        StatusSolicitacao _status;

    public:
        Solicitacao(Usuario* passageiro, Carona* carona);
        void aceitar();
        void recusar();

        Usuario* get_passageiro() const;
        Carona* get_carona() const;
        StatusSolicitacao get_status() const;
        std::string get_status_string() const;

        void exibir_info() const;
        void exibir_para_motorista() const;
    };
}
#endif