#include "Carona.hpp"
#include "Usuario.hpp" // Para Usuario*
#include "Veiculo.hpp"
#include "Solicitacao.hpp" // Para Solicitacao*
#include <iostream>
#include <iomanip>
#include <algorithm> // Para std::find
// #include <memory> // Removido

namespace ufmg_carona {
    int Carona::_proximo_id = 1;

    // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario* no construtor
    Carona::Carona(std::string origem, std::string destino, std::string data, Usuario* motorista, bool apenas_mulheres, TipoCarona tipo)
        : _id(gerar_proximo_id()),
          _origem(origem),
          _destino(destino),
          _data_hora_partida(data),
          _motorista(motorista), // Atribui o ponteiro bruto
          _passageiros(), // Inicializando o vetor vazio
          _solicitacoes_pendentes(), // Inicializando o vetor vazio
          _vagas_disponiveis(0), // Valor inicial seguro
          _apenas_mulheres(apenas_mulheres),
          _status(StatusCarona::AGUARDANDO),
          _tipo(tipo) {

        if (motorista && motorista->is_motorista()) { // Acesso direto ao ponteiro bruto
            _vagas_disponiveis = motorista->get_veiculo().get_lugares() - 1;
        }
    }

    int Carona::gerar_proximo_id() { return _proximo_id++; }

    // Getters
    int Carona::get_id() const { return _id; }
    // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario* no getter
    Usuario* Carona::get_motorista() const { return _motorista; } // Retorna ponteiro bruto
    const std::string& Carona::get_origem() const { return _origem; }
    const std::string& Carona::get_destino() const { return _destino; }
    const std::string& Carona::get_data_hora() const { return _data_hora_partida; }
    int Carona::get_vagas_disponiveis() const { return _vagas_disponiveis; }
    bool Carona::get_apenas_mulheres() const { return _apenas_mulheres; }

    void Carona::exibir_info() const {
        std::cout << "\n--- Carona ID: " << _id << " ---" << std::endl;
        std::cout << "De: " << _origem << " -> Para: " << _destino << std::endl;
        std::cout << "Data/Hora: " << _data_hora_partida << std::endl;
        if (_motorista) {
            std::cout << "Motorista: " << _motorista->get_nome() << " | Avaliacao: " // Acesso direto ao ponteiro
                      << std::fixed << std::setprecision(1) << _motorista->get_media_avaliacoes() << " estrelas" << std::endl; // Acesso direto
        }
        std::cout << "Vagas restantes: " << _vagas_disponiveis << std::endl;
        if (_apenas_mulheres) {
            std::cout << ">> Viagem exclusiva para mulheres <<" << std::endl;
        }
    }

    void Carona::exibir_info_detalhada() const {
        exibir_info();
        std::cout << "Passageiros confirmados: " << _passageiros.size() << std::endl;
        std::cout << "Solicitacoes pendentes: " << _solicitacoes_pendentes.size() << std::endl;
    }

    // Gerenciamento de solicitações
    // ALTERAÇÃO: De std::shared_ptr<Solicitacao> para Solicitacao*
    void Carona::adicionar_solicitacao(Solicitacao* solicitacao) {
        _solicitacoes_pendentes.push_back(solicitacao);
    }

    // ALTERAÇÃO: De const std::vector<std::shared_ptr<Solicitacao>>& para const std::vector<Solicitacao*>&
    const std::vector<Solicitacao*>& Carona::get_solicitacoes_pendentes() const {
        return _solicitacoes_pendentes;
    }

    bool Carona::tem_solicitacoes_pendentes() const {
        return !_solicitacoes_pendentes.empty();
    }

    // Gerenciamento de passageiros
    // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
    void Carona::adicionar_passageiro(Usuario* passageiro) {
        if (_vagas_disponiveis > 0) {
            _passageiros.push_back(passageiro);
            _vagas_disponiveis--;
        }
    }

    // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
    void Carona::remover_passageiro(Usuario* passageiro) {
        // Para ponteiros brutos, a comparação é direta pelo endereço
        auto it = std::find(_passageiros.begin(), _passageiros.end(), passageiro);
        if (it != _passageiros.end()) {
            _passageiros.erase(it);
            _vagas_disponiveis++;
        }
    }
}