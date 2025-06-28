#include "Solicitacao.hpp"
#include "Usuario.hpp"
#include "Carona.hpp"
#include <iostream>

namespace ufmg_carona {
    Solicitacao::Solicitacao(Usuario* p, Carona* c, std::string local_embarque_p, std::string local_desembarque_p)
        : _passageiro(p),
          _carona_alvo(c),
          _status(StatusSolicitacao::PENDENTE),
          _local_embarque_passageiro(local_embarque_p),
          _local_desembarque_passageiro(local_desembarque_p),
          _local_embarque_motorista_proposto(""),
          _local_desembarque_motorista_proposto("") {}

    void Solicitacao::aceitar() { _status = StatusSolicitacao::ACEITA; }
    void Solicitacao::recusar() { _status = StatusSolicitacao::RECUSADA; }

    void Solicitacao::propor_locais_motorista(std::string local_embarque_m, std::string local_desembarque_m) {
        _local_embarque_motorista_proposto = local_embarque_m;
        _local_desembarque_motorista_proposto = local_desembarque_m;
        _status = StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO;
    }

    void Solicitacao::aceitar_proposta_motorista() {
        _status = StatusSolicitacao::ACEITA;
    }

    void Solicitacao::recusar_proposta_motorista() {
        _status = StatusSolicitacao::RECUSADA_PROPOSTA_MOTORISTA;
    }

    void Solicitacao::set_status(StatusSolicitacao novo_status) {
        _status = novo_status;
    }

    void Solicitacao::set_carona(Carona* new_carona_ptr) { 
        _carona_alvo = new_carona_ptr;
    }

    Usuario* Solicitacao::get_passageiro() const { return _passageiro; }
    Carona* Solicitacao::get_carona() const { return _carona_alvo; }
    StatusSolicitacao Solicitacao::get_status() const { return _status; }

    std::string Solicitacao::get_status_string() const {
        switch (_status) {
            case StatusSolicitacao::PENDENTE: return "PENDENTE";
            case StatusSolicitacao::ACEITA: return "ACEITA";
            case StatusSolicitacao::RECUSADA: return "RECUSADA";
            case StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO: return "AGUARDANDO RESPOSTA DO PASSAGEIRO";
            case StatusSolicitacao::RECUSADA_PROPOSTA_MOTORISTA: return "RECUSADA (PROPOSTA DO MOTORISTA)";
            default: return "DESCONHECIDO";
        }
    }

    const std::string& Solicitacao::get_local_embarque_passageiro() const { return _local_embarque_passageiro; }
    const std::string& Solicitacao::get_local_desembarque_passageiro() const { return _local_desembarque_passageiro; }
    const std::string& Solicitacao::get_local_embarque_motorista_proposto() const { return _local_embarque_motorista_proposto; }
    const std::string& Solicitacao::get_local_desembarque_motorista_proposto() const { return _local_desembarque_motorista_proposto; }


    void Solicitacao::exibir_info() const {
        std::cout << "Carona ID: " << (_carona_alvo ? std::to_string(_carona_alvo->get_id()) : "N/A") 
                  << " | Status: " << get_status_string() << std::endl;
        std::cout << "  Seu embarque: '" << _local_embarque_passageiro << "'" << std::endl;
        std::cout << "  Seu desembarque: '" << _local_desembarque_passageiro << "'" << std::endl;
        if (_status == StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO || _status == StatusSolicitacao::RECUSADA_PROPOSTA_MOTORISTA) {
             std::cout << "  Proposta do motorista: Embarque em '" << _local_embarque_motorista_proposto
                       << "', Desembarque em '" << _local_desembarque_motorista_proposto << "'" << std::endl;
        }
    }

    void Solicitacao::exibir_para_motorista() const {
        std::cout << "Solicitacao de: " << (_passageiro ? _passageiro->get_nome() : "N/A") 
                  << " | Carona ID: " << (_carona_alvo ? std::to_string(_carona_alvo->get_id()) : "N/A") 
                  << " | Status: " << get_status_string() << std::endl;
        std::cout << "  Passageiro propos: Embarque em '" << _local_embarque_passageiro
                  << "', Desembarque em '" << _local_desembarque_passageiro << "'" << std::endl;
        if (_status == StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO || _status == StatusSolicitacao::RECUSADA_PROPOSTA_MOTORISTA) {
             std::cout << "  Sua proposta: Embarque em '" << _local_embarque_motorista_proposto
                       << "', Desembarque em '" << _local_desembarque_motorista_proposto << "'" << std::endl;
        }
    }
}