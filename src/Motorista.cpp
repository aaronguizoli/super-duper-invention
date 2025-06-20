#include "Motorista.hpp"
#include <iostream>
#include <algorithm>

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
            _veiculos.push_back(veiculo);
            std::cout << "Veiculo " << veiculo->get_placa() << " adicionado para o motorista " << get_nome() << "." << std::endl;
        }
    }

    // ALTERACAO CRITICA: is_motorista() agora retorna true se o Motorista tem CNH, nao apenas se tem veiculos.
    bool Motorista::is_motorista() const { return !_cnh_numero.empty(); } // Retorna true se a CNH nao estiver vazia

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