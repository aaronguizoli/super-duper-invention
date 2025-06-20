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

    // Construtor da Carona ATUALIZADO para receber o veiculo_usado
    Carona::Carona(std::string origem, std::string destino, std::string data, Usuario* motorista, Veiculo* veiculo_usado, bool apenas_mulheres, TipoCarona tipo)
        : _id(gerar_proximo_id()),
          _origem(origem),
          _destino(destino),
          _data_hora_partida(data),
          _motorista(motorista),
          _veiculo_usado(veiculo_usado), // Inicializa o novo atributo
          _passageiros(),
          _solicitacoes_pendentes(),
          _vagas_disponiveis(0), // Valor inicial seguro
          _apenas_mulheres(apenas_mulheres),
          _status(StatusCarona::AGUARDANDO),
          _tipo(tipo) {

        // As vagas disponiveis sao diretamente do veiculo_usado
        if (veiculo_usado) {
            _vagas_disponiveis = veiculo_usado->get_lugares() - 1;
        } else {
            std::cerr << "ERRO: Carona criada sem um veiculo valido. Vagas definidas como 0." << std::endl;
            _vagas_disponiveis = 0;
        }
    }

    int Carona::gerar_proximo_id() { return _proximo_id++; }

    int Carona::get_id() const { return _id; }
    Usuario* Carona::get_motorista() const { return _motorista; }
    Veiculo* Carona::get_veiculo_usado() const { return _veiculo_usado; }
    const std::string& Carona::get_origem() const { return _origem; }
    const std::string& Carona::get_destino() const { return _destino; }
    const std::string& Carona::get_data_hora() const { return _data_hora_partida; }
    int Carona::get_vagas_disponiveis() const { return _vagas_disponiveis; }
    bool Carona::get_apenas_mulheres() const { return _apenas_mulheres; }

    void Carona::exibir_info() const {
        std::cout << "\n--- Carona ID: " << _id << " ---" << std::endl;
        std::cout << "De: " << _origem << " -> Para: " << _destino << std::endl;
        std::cout << "Data/Hora: " << _data_hora_partida << std::endl;
        
        // ADICIONADO: Verificacao de nullptr para _motorista
        if (_motorista) {
            std::cout << "Motorista: " << _motorista->get_nome() << " | Avaliacao: "
                      << std::fixed << std::setprecision(1) << _motorista->get_media_avaliacoes() << " estrelas" << std::endl;
            
            // ADICIONADO: Verificacao de nullptr para _veiculo_usado
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
    }

    void Carona::exibir_info_detalhada() const {
        exibir_info(); // Já contém as verificações
        std::cout << "Passageiros confirmados: " << _passageiros.size() << std::endl;
        std::cout << "Solicitacoes pendentes: " << _solicitacoes_pendentes.size() << std::endl;
        // Poderiamos iterar e exibir os passageiros se desejado
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