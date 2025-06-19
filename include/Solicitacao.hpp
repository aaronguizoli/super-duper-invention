#ifndef SOLICITACAO_HPP
#define SOLICITACAO_HPP
#include <memory>

namespace ufmg_carona {
    class Usuario;
    class Carona;
    enum class StatusSolicitacao { PENDENTE, ACEITA, RECUSADA };
    class Solicitacao {
    private: std::shared_ptr<Usuario> _passageiro; Carona* _carona_alvo; StatusSolicitacao _status;
    public: Solicitacao(std::shared_ptr<Usuario> passageiro, Carona* carona); void aceitar(); void recusar();
    };
}
#endif