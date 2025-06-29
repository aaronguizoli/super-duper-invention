#include "Carona.hpp"
#include "Usuario.hpp"
#include "Veiculo.hpp"
#include "Solicitacao.hpp"
#include "Motorista.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace ufmg_carona {
    int Carona::_proximo_id = 1;

    Carona::Carona(std::string origem_nome, std::string destino_nome, Zona origem_zona, Zona destino_zona, UFMGPosicao ufmg_posicao, std::string data, Usuario* motorista, Veiculo* veiculo_usado, bool apenas_mulheres, TipoCarona tipo)
        : _id(gerar_proximo_id()),
          _origem_nome(origem_nome),
          _destino_nome(destino_nome),
          _origem_zona(origem_zona),
          _destino_zona(destino_zona),
          _ufmg_posicao(ufmg_posicao),
          _data_hora_partida(data),
          _motorista(motorista),
          _veiculo_usado(veiculo_usado),
          _passageiros(),
          _solicitacoes_pendentes(),
          _vagas_disponiveis(0),
          _apenas_mulheres(apenas_mulheres),
          _status(StatusCarona::AGUARDANDO),
          _tipo(tipo) {

        if (veiculo_usado) {
            _vagas_disponiveis = veiculo_usado->get_lugares() - 1;
        } else {
            std::cerr << "ERRO: Carona criada sem um veiculo valido. Vagas definidas como 0." << std::endl;
            _vagas_disponiveis = 0;
        }
    }

    int Carona::gerar_proximo_id() { return _proximo_id++; }

    void Carona::set_proximo_id(int id) { _proximo_id = id; }

    int Carona::get_id() const { return _id; }
    Usuario* Carona::get_motorista() const { return _motorista; }
    Veiculo* Carona::get_veiculo_usado() const { return _veiculo_usado; }
    const std::string& Carona::get_origem() const { return _origem_nome; }
    const std::string& Carona::get_destino() const { return _destino_nome; }
    
    Zona Carona::get_origem_zona() const { return _origem_zona; }
    Zona Carona::get_destino_zona() const { return _destino_zona; }
    UFMGPosicao Carona::get_ufmg_posicao() const { return _ufmg_posicao; }

    StatusCarona Carona::get_status() const { return _status; }
    void Carona::set_status(StatusCarona novo_status) { _status = novo_status; }

    const std::string& Carona::get_data_hora() const { return _data_hora_partida; }
    int Carona::get_vagas_disponiveis() const { return _vagas_disponiveis; }
    bool Carona::get_apenas_mulheres() const { return _apenas_mulheres; }

    void Carona::exibir_info() const {
        std::cout << "\n--- Carona ID: " << _id << " ---" << std::endl;
        std::cout << "De: " << _origem_nome << " -> Para: " << _destino_nome << std::endl;
        std::cout << "Data/Hora: " << _data_hora_partida << std::endl;
        
        if (_motorista) {
            std::cout << "Motorista: " << _motorista->get_nome() << " | Avaliacao: "
                      << std::fixed << std::setprecision(1) << _motorista->get_media_avaliacoes() << " estrelas" << std::endl;
            
            if (_veiculo_usado) {
                std::cout << "  Veiculo: " << _veiculo_usado->get_marca() << " " << _veiculo_usado->get_modelo()
                          << ", Cor: " << _veiculo_usado->get_cor() << ", Placa: " << _veiculo_usado->get_placa() << std::endl;
            } else {
                std::cout << "  Veiculo: Nao disponivel" << std::endl;
            }
        } else {
            std::cout << "Motorista: Nao disponivel" << std::endl;
        }
        std::cout << "Vagas restantes: " << _vagas_disponiveis << std::endl;
        if (_apenas_mulheres) {
            std::cout << ">> Viagem exclusiva para mulheres <<" << std::endl;
        }
        std::cout << "Status da Carona: ";
        switch (_status) {
            case StatusCarona::AGUARDANDO: std::cout << "AGUARDANDO"; break;
            case StatusCarona::LOTADA: std::cout << "LOTADA"; break;
            case StatusCarona::EM_VIAGEM: std::cout << "EM VIAGEM"; break;
            case StatusCarona::FINALIZADA: std::cout << "FINALIZADA"; break;
            case StatusCarona::CANCELADA: std::cout << "CANCELADA"; break;
        }
        std::cout << std::endl;
    }

    void Carona::exibir_info_detalhada() const {
        exibir_info();
        std::cout << "Passageiros confirmados: " << _passageiros.size() << std::endl;
        std::cout << "Solicitacoes pendentes: " << _solicitacoes_pendentes.size() << std::endl;
    }

    void Carona::adicionar_solicitacao(Solicitacao* solicitacao) {
        _solicitacoes_pendentes.push_back(solicitacao);
    }

    const std::vector<Solicitacao*>& Carona::get_solicitacoes_pendentes() const {
        return _solicitacoes_pendentes;
    }

    bool Carona::tem_solicitacoes_pendentes() const {
        return !_solicitacoes_pendentes.empty();
    }

    void Carona::adicionar_passageiro(Usuario* passageiro) {
        if (_vagas_disponiveis > 0) {
            _passageiros.push_back(passageiro);
            _vagas_disponiveis--;
        }
    }

    void Carona::remover_passageiro(Usuario* passageiro) {
        auto it = std::find(_passageiros.begin(), _passageiros.end(), passageiro);
        if (it != _passageiros.end()) {
            _passageiros.erase(it);
            _vagas_disponiveis++;
        }
    }
}