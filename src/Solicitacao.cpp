#include "Solicitacao.hpp"
#include "Usuario.hpp"
#include "Carona.hpp"
#include <iostream>

namespace ufmg_carona {
    Solicitacao::Solicitacao(std::shared_ptr<Usuario> p, Carona* c) : _passageiro(p), _carona_alvo(c), _status(StatusSolicitacao::PENDENTE) {}
    
    void Solicitacao::aceitar() { _status = StatusSolicitacao::ACEITA; }
    void Solicitacao::recusar() { _status = StatusSolicitacao::RECUSADA; }
    
    std::shared_ptr<Usuario> Solicitacao::get_passageiro() const { return _passageiro; }
    Carona* Solicitacao::get_carona() const { return _carona_alvo; }
    StatusSolicitacao Solicitacao::get_status() const { return _status; }
    
    std::string Solicitacao::get_status_string() const {
        switch (_status) {
            case StatusSolicitacao::PENDENTE: return "PENDENTE";
            case StatusSolicitacao::ACEITA: return "ACEITA";
            case StatusSolicitacao::RECUSADA: return "RECUSADA";
            default: return "DESCONHECIDO";
        }
    }
    
    void Solicitacao::exibir_info() const {
        std::cout << "Carona ID: " << _carona_alvo->get_id() 
                  << " | Status: " << get_status_string() << std::endl;
    }
    
    void Solicitacao::exibir_para_motorista() const {
        std::cout << "Solicitacao de: " << _passageiro->get_nome() 
                  << " | Carona ID: " << _carona_alvo->get_id()
                  << " | Status: " << get_status_string() << std::endl;
    }
}
