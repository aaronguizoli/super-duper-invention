#include "Solicitacao.hpp"
#include "Usuario.hpp" // Incluído para o uso de ponteiros inteligentes, se necessário
#include "Carona.hpp"

namespace ufmg_carona {

    Solicitacao::Solicitacao(std::shared_ptr<Usuario> passageiro, Carona* carona)
        : _passageiro(passageiro), _carona_alvo(carona), _status(StatusSolicitacao::PENDENTE) {}

    void Solicitacao::aceitar() {
        _status = StatusSolicitacao::ACEITA;
    }

    void Solicitacao::recusar() {
        _status = StatusSolicitacao::RECUSADA;
    }

    // IMPLEMENTAÇÃO DO MÉTODO QUE FALTAVA
    StatusSolicitacao Solicitacao::get_status() const {
        return _status;
    }

} // namespace ufmg_carona