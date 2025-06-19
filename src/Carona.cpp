#include "Carona.hpp"
#include "Usuario.hpp"
#include "Veiculo.hpp"
#include <iostream>
#include <iomanip>

namespace ufmg_carona {
    int Carona::_proximo_id = 1;

    // A ORDEM NA LISTA DE INICIALIZAÇÃO AGORA BATE COM A ORDEM DE DECLARAÇÃO NO .HPP
    Carona::Carona(std::string origem, std::string destino, std::string data, std::shared_ptr<Usuario> motorista, bool apenas_mulheres, TipoCarona tipo)
        : _id(gerar_proximo_id()), 
          _origem(origem), 
          _destino(destino), 
          _data_hora_partida(data),
          _motorista(motorista), 
          _passageiros(), // Inicializando o vetor vazio
          _vagas_disponiveis(0), // Valor inicial seguro
          _apenas_mulheres(apenas_mulheres), 
          _status(StatusCarona::AGUARDANDO),
          _tipo(tipo) {
        
        if (motorista && motorista->is_motorista()) {
            _vagas_disponiveis = motorista->get_veiculo().get_lugares() - 1;
        }
    }

    int Carona::gerar_proximo_id() { return _proximo_id++; }
    int Carona::get_id() const { return _id; }

    void Carona::exibir_info() const {
        std::cout << "\n--- Carona ID: " << _id << " ---" << std::endl;
        std::cout << "De: " << _origem << " -> Para: " << _destino << std::endl;
        std::cout << "Data/Hora: " << _data_hora_partida << std::endl;
        if (_motorista) {
            std::cout << "Motorista: " << _motorista->get_nome() << " | Avaliacao: " 
                      << std::fixed << std::setprecision(1) << _motorista->get_media_avaliacoes() << " estrelas" << std::endl;
        }
        std::cout << "Vagas restantes: " << _vagas_disponiveis << std::endl;
        if (_apenas_mulheres) {
            std::cout << ">> Viagem exclusiva para mulheres <<" << std::endl;
        }
    }
}