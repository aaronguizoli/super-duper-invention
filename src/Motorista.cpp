#include "Motorista.hpp"
#include <iostream>
#include <algorithm>
#include <map>
#include "Rotina.hpp"

namespace ufmg_carona {
    Motorista::Motorista(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento,
                         std::string email, std::string senha, Genero genero, std::string vinculo_tipo,
                         std::string detalhe_vinculo, std::string cnh_numero)
        : Usuario(nome, cpf, telefone, data_nascimento, email, senha, genero, vinculo_tipo, detalhe_vinculo),
          _veiculos(),
          _cnh_numero(cnh_numero),
          _rotinas() {}

    Motorista::~Motorista() {
        for (Veiculo* v : _veiculos) {
            delete v;
        }
        _veiculos.clear();
    }

    void Motorista::adicionar_veiculo(Veiculo* veiculo) {
        if (veiculo) {
            for (const auto& v_existente : _veiculos) {
                if (v_existente->get_placa() == veiculo->get_placa()) {
                    std::cout << "ERRO: Veiculo com esta placa ja existe para este motorista. Nao adicionado." << std::endl;
                    delete veiculo;
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

    Veiculo* Motorista::buscar_veiculo_por_indice(size_t indice) const {
        if (indice < _veiculos.size()) {
            return _veiculos[indice];
        }
        return nullptr;
    }

    bool Motorista::remover_veiculo(const std::string& placa) {
        auto it = _veiculos.begin();
        while (it != _veiculos.end()) {
            if ((*it)->get_placa() == placa) {
                delete *it;
                it = _veiculos.erase(it);
                return true;
            } else {
                ++it;
            }
        }
        return false;
    }

    bool Motorista::remover_rotina(int indice) {
        if (indice >= 0 && static_cast<size_t>(indice) < _rotinas.size()) {
            _rotinas.erase(_rotinas.begin() + indice);
            return true;
        }
        return false;
    }

    void Motorista::adicionar_rotina(const Rotina& rotina) {
        _rotinas.push_back(rotina);
    }

    const std::vector<Rotina>& Motorista::get_rotinas() const {
        return _rotinas;
    }

    void Motorista::imprimir_perfil() const {
        Usuario::imprimir_perfil();
        std::cout << "--- Informacoes de Motorista ---" << std::endl;
        std::cout << "CNH: " << _cnh_numero << std::endl;
        
        if (_veiculos.empty()) {
            std::cout << "Nenhum veiculo cadastrado." << std::endl;
        } else {
            std::cout << "Veiculos Cadastrados (" << _veiculos.size() << "):" << std::endl;
            for (size_t i = 0; i < _veiculos.size(); ++i) {
                std::cout << "  [" << (i + 1) << "] ";
                if (_veiculos[i]) {
                    _veiculos[i]->exibir_info();
                }
            }
        }
        
        if (_rotinas.empty()) {
            std::cout << "Nenhuma rotina de carona cadastrada." << std::endl;
        } else {
            std::cout << "Rotinas de Carona Cadastradas (" << _rotinas.size() << "):" << std::endl;
            std::map<DiaDaSemana, std::string> dias_da_semana = {
                {DiaDaSemana::DOMINGO, "Domingo"}, {DiaDaSemana::SEGUNDA, "Segunda"},
                {DiaDaSemana::TERCA, "Terca"}, {DiaDaSemana::QUARTA, "Quarta"},
                {DiaDaSemana::QUINTA, "Quinta"}, {DiaDaSemana::SEXTA, "Sexta"},
                {DiaDaSemana::SABADO, "Sabado"}
            };
            for (size_t i = 0; i < _rotinas.size(); ++i) {
                std::cout << "  [" << (i + 1) << "] ";
                const Rotina& rotina = _rotinas[i];
                std::cout << "Dias: ";
                const std::vector<DiaDaSemana>& dias_da_rotina = rotina.get_dias();
                if (dias_da_rotina.empty()) {
                    std::cout << "Nenhum";
                } else if (dias_da_rotina.size() == 7) {
                    std::cout << "Todos";
                } else {
                    for (size_t j = 0; j < dias_da_rotina.size(); ++j) {
                        std::cout << dias_da_semana[dias_da_rotina[j]];
                        if (j < dias_da_rotina.size() - 1) {
                            std::cout << ", ";
                        }
                    }
                }
                std::cout << " | Hora: " << rotina.get_horario_saida()
                          << " | " << rotina.get_local_saida_padrao() << " -> "
                          << rotina.get_destino_final();
                if (!rotina.get_placa_veiculo_usado().empty()) {
                    std::cout << " (Veiculo: " << rotina.get_placa_veiculo_usado() << ")";
                }
                if (rotina.get_apenas_mulheres()) {
                    std::cout << " (Apenas Mulheres)";
                }
                std::cout << std::endl;
            }
        }
        std::cout << "---------------------------------" << std::endl;
    }
}