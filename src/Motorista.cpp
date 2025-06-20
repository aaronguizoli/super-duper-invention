#include "Motorista.hpp"
#include <iostream>
#include <algorithm> // Para std::remove_if e std::for_each

namespace ufmg_carona {
    // Construtor do Motorista - Ordem da lista de inicializacao ajustada
    Motorista::Motorista(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento,
                         std::string email, std::string senha, Genero genero, std::string vinculo_tipo,
                         std::string detalhe_vinculo, std::string cnh_numero)
        : Usuario(nome, cpf, telefone, data_nascimento, email, senha, genero, vinculo_tipo, detalhe_vinculo),
          _veiculos(),
          _cnh_numero(cnh_numero) {}

    // Destrutor para liberar a memoria dos objetos Veiculo*
    Motorista::~Motorista() {
        for (Veiculo* v : _veiculos) {
            delete v;
        }
        _veiculos.clear();
    }

    void Motorista::adicionar_veiculo(Veiculo* veiculo) {
        if (veiculo) {
            // Verifica se o veiculo ja existe pela placa para evitar duplicatas e vazamentos
            for (const auto& v_existente : _veiculos) {
                if (v_existente->get_placa() == veiculo->get_placa()) {
                    std::cout << "ERRO: Veiculo com esta placa ja existe para este motorista. Nao adicionado." << std::endl;
                    delete veiculo; // Evita vazamento de memória se o veículo já existe
                    return;
                }
            }
            _veiculos.push_back(veiculo);
            std::cout << "Veiculo " << veiculo->get_placa() << " adicionado para o motorista " << get_nome() << "." << std::endl;
        }
    }

    bool Motorista::is_motorista() const { return !_cnh_numero.empty(); }

    const std::string& Motorista::get_cnh_numero() const { return _cnh_numero; }

    const std::vector<Veiculo*>& Motorista::get_veiculos() const { return _veiculos; }

    Veiculo* Motorista::buscar_veiculo_por_placa(const std::string& placa) const {
        for (Veiculo* v : _veiculos) {
            if (v && v->get_placa() == placa) {
                return v;
            }
        }
        return nullptr;
    }

    // Corrigido: Metodo para remover um veiculo
    bool Motorista::remover_veiculo(const std::string& placa) {
        // Encontra o veículo a ser removido e apaga ele
        auto it = _veiculos.begin();
        while (it != _veiculos.end()) {
            if ((*it)->get_placa() == placa) {
                delete *it; // Libera a memória do objeto Veiculo antes de remover o ponteiro
                it = _veiculos.erase(it); // Remove o ponteiro do vetor e avança o iterador
                return true; // Veículo encontrado e removido
            } else {
                ++it;
            }
        }
        return false; // Veículo não encontrado
    }

    void Motorista::imprimir_perfil() const {
        Usuario::imprimir_perfil();
        std::cout << "--- Informacoes de Motorista ---" << std::endl;
        std::cout << "CNH: " << _cnh_numero << std::endl;
        
        if (_veiculos.empty()) { // Verifica se NAO possui veiculos para exibicao
            std::cout << "Nenhum veiculo cadastrado." << std::endl;
        } else {
            std::cout << "Veiculos Cadastrados (" << _veiculos.size() << "):" << std::endl;
            for (const auto& veiculo_ptr : _veiculos) {
                if (veiculo_ptr) {
                    veiculo_ptr->exibir_info();
                }
            }
        }
        std::cout << "---------------------------------" << std::endl;
    }
}