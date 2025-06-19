#include "Solicitacao.hpp"
namespace ufmg_carona {
    Solicitacao::Solicitacao(std::shared_ptr<Usuario> p, Carona* c) : _passageiro(p), _carona_alvo(c), _status(StatusSolicitacao::PENDENTE) {}
    void Solicitacao::aceitar() { _status = StatusSolicitacao::ACEITA; }
    void Solicitacao::recusar() { _status = StatusSolicitacao::RECUSADA; }
}