#include "GerenciadorVeiculos.hpp"
#include "TerminalIO.hpp"
#include "GerenciadorUsuarios.hpp"
#include "Motorista.hpp"
#include "Veiculo.hpp"
#include "Excecoes.hpp"
#include "Usuario.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

namespace ufmg_carona {

    GerenciadorVeiculos::GerenciadorVeiculos(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios)
        : _terminal_io(terminal_io), _ger_usuarios(ger_usuarios) {
    }

    GerenciadorVeiculos::~GerenciadorVeiculos() {
        // Os objetos Veiculo são de responsabilidade do Motorista,
        // então não há necessidade de liberá-los aqui.
    }

    void GerenciadorVeiculos::carregarDados(GerenciadorUsuarios* ger_usuarios) {
        // Este método é o interno, que pode ser chamado pelo método público.
        // O parâmetro 'ger_usuarios' é usado pelo método público, mas não diretamente aqui.
        (void)ger_usuarios; // <--- ADICIONADO: Suprime o warning de parâmetro não utilizado
    }

    void GerenciadorVeiculos::carregarDadosVeiculosPublico(GerenciadorUsuarios* ger_usuarios) {
        std::ifstream arquivo_veiculos("veiculos.txt");
        if (arquivo_veiculos.is_open()) {
            std::string linha_veiculo;
            while(std::getline(arquivo_veiculos, linha_veiculo)) {
                std::stringstream ss_veiculo(linha_veiculo);
                std::string cpf_motorista_veic, placa_veic, marca_veic, modelo_veic, cor_veic, lugares_str_veic;
                int lugares_veic;

                std::getline(ss_veiculo, cpf_motorista_veic, ';');
                std::getline(ss_veiculo, placa_veic, ';');
                std::getline(ss_veiculo, marca_veic, ';');
                std::getline(ss_veiculo, modelo_veic, ';');
                std::getline(ss_veiculo, cor_veic, ';');
                std::getline(ss_veiculo, lugares_str_veic);
                
                try {
                    lugares_veic = std::stoi(lugares_str_veic);
                } catch (const std::invalid_argument& e) {
                    _terminal_io->exibirErro("ERRO: Valor invalido para 'lugares' no veiculos.txt para placa " + placa_veic + ". Ignorando veiculo. Erro: " + e.what());
                    continue;
                } catch (const std::out_of_range& e) {
                    _terminal_io->exibirErro("ERRO: Valor de 'lugares' fora do limite para placa " + placa_veic + " no veiculos.txt. Ignorando veiculo. Erro: " + e.what());
                    continue;
                }

                Usuario* motorista_assoc = ger_usuarios->buscarUsuarioPorCpf(cpf_motorista_veic);
                if (motorista_assoc && motorista_assoc->is_motorista()) {
                    Motorista* m_ptr = dynamic_cast<Motorista*>(motorista_assoc);
                    if (m_ptr) {
                        if (m_ptr->buscar_veiculo_por_placa(placa_veic) == nullptr) {
                            m_ptr->adicionar_veiculo(new Veiculo(placa_veic, marca_veic, modelo_veic, cor_veic, lugares_veic));
                        } else {
                            _terminal_io->exibirAviso("AVISO: Veiculo com placa " + placa_veic + " ja existe para o motorista " + cpf_motorista_veic + ". Nao recarregado.");
                        }
                    }
                } else {
                    _terminal_io->exibirAviso("AVISO: Veiculo com placa " + placa_veic + " tem CPF de motorista (" + cpf_motorista_veic + ") nao encontrado ou nao e motorista valido. Veiculo nao carregado.");
                }
            }
            arquivo_veiculos.close();
        } else {
            _terminal_io->exibirAviso("AVISO: Arquivo veiculos.txt nao encontrado. Nao ha veiculos para carregar.");
        }
    }

    void GerenciadorVeiculos::salvarDados(GerenciadorUsuarios* ger_usuarios) {
        // Este método é o interno, que pode ser chamado pelo método público.
        // O parâmetro 'ger_usuarios' é usado pelo método público, mas não diretamente aqui.
        (void)ger_usuarios; // <--- ADICIONADO: Suprime o warning de parâmetro não utilizado
    }

    void GerenciadorVeiculos::salvarDadosVeiculosPublico(GerenciadorUsuarios* ger_usuarios) {
        std::ofstream arquivo_veiculos("veiculos.txt", std::ios::trunc);
        if (!arquivo_veiculos.is_open()) {
            _terminal_io->exibirErro("Nao foi possivel abrir o arquivo veiculos.txt para salvar dados.");
            return;
        }

        for (const auto& u : ger_usuarios->getUsuarios()) {
            if (u->is_motorista()) {
                Motorista* m_ptr = dynamic_cast<Motorista*>(u);
                if (m_ptr) {
                    for (const auto& veic : m_ptr->get_veiculos()) {
                        if (veic) {
                            arquivo_veiculos << u->get_cpf() << ";"
                                             << veic->get_placa() << ";"
                                             << veic->get_marca() << ";"
                                             << veic->get_modelo() << ";"
                                             << veic->get_cor() << ";"
                                             << veic->get_lugares()
                                             << std::endl;
                        }
                    }
                }
            }
        }
        arquivo_veiculos.close();
    }

    void GerenciadorVeiculos::fluxo_cadastrar_veiculo(Usuario* usuario_logado) {
        if (!usuario_logado->is_motorista()) {
            _terminal_io->exibirMensagem("Voce nao esta registrado como motorista. Nao e possivel cadastrar veiculo.");
            return;
        }
        
        Motorista* motorista_logado = dynamic_cast<Motorista*>(usuario_logado);
        if (!motorista_logado) {
            _terminal_io->exibirErro("ERRO INTERNO: Usuario logado deveria ser motorista mas nao pode ser convertido.");
            return;
        }

        std::string placa, marca, modelo, cor;
        int lugares;

        char cadastrar_outro = 's';
        while (cadastrar_outro == 's' || cadastrar_outro == 'S') {
            _terminal_io->exibirMensagem("\n--- Cadastrar Novo Veiculo ---");
            placa = _terminal_io->coletarStringInput("Placa: ");
            if (motorista_logado->buscar_veiculo_por_placa(placa) != nullptr) {
                _terminal_io->exibirMensagem("Voce ja possui um veiculo com esta placa.");
                cadastrar_outro = _terminal_io->confirmarAcao("Deseja tentar com outra placa? (s/n): ");
                continue;
            }

            marca = _terminal_io->coletarStringInput("Marca: ");
            modelo = _terminal_io->coletarStringInput("Modelo: ");
            cor = _terminal_io->coletarStringInput("Cor: ");
            lugares = _terminal_io->coletarIntInput("Total de lugares (com motorista): ", 2, 99);
            
            motorista_logado->adicionar_veiculo(new Veiculo(placa, marca, modelo, cor, lugares));
            salvarDadosVeiculosPublico(_ger_usuarios);
            _terminal_io->exibirMensagem("Veiculo cadastrado com sucesso!");

            cadastrar_outro = _terminal_io->confirmarAcao("Deseja cadastrar outro veiculo? (s/n): ");
        }
    }

    void GerenciadorVeiculos::fluxo_gerenciar_veiculos(Usuario* usuario_logado) {
        if (!usuario_logado->is_motorista()) {
            _terminal_io->exibirMensagem("Voce nao e motorista para gerenciar veiculos.");
            return;
        }
        Motorista* motorista_logado = dynamic_cast<Motorista*>(usuario_logado);
        if (!motorista_logado) return;

        int comando;
        do {
            _terminal_io->exibirMensagem("\n--- Gerenciar Meus Veiculos ---");
            if (motorista_logado->get_veiculos().empty()) {
                _terminal_io->exibirMensagem("Nenhum veiculo cadastrado.");
            } else {
                _terminal_io->exibirMensagem("Seus veiculos cadastrados:");
                for (size_t i = 0; i < motorista_logado->get_veiculos().size(); ++i) {
                    _terminal_io->exibirMensagem("[" + std::to_string(i + 1) + "] ");
                    if (motorista_logado->get_veiculos()[i]) {
                        motorista_logado->get_veiculos()[i]->exibir_info();
                    }
                }
            }
            
            _terminal_io->exibirMensagem("\n(1) Cadastrar Novo Veiculo | (2) Editar Veiculo | (3) Excluir Veiculo | (0) Voltar");
            comando = _terminal_io->coletarIntInput("> ", 0, 3);

            try {
                if (comando == 1) {
                    fluxo_cadastrar_veiculo(motorista_logado);
                } else if (comando == 2) {
                    fluxo_editar_veiculo_externo(motorista_logado);
                } else if (comando == 3) {
                    fluxo_excluir_veiculo_externo(motorista_logado);
                }
            } catch (const AppExcecao& e) {
                _terminal_io->exibirErro(e.what());
            }
        } while (comando != 0);
    }

    void GerenciadorVeiculos::fluxo_editar_veiculo_externo(Motorista* motorista) {
        if (motorista->get_veiculos().empty()) {
            _terminal_io->exibirMensagem("Nenhum veiculo para editar.");
            return;
        }

        std::string placa_escolhida;
        placa_escolhida = _terminal_io->coletarStringInput("Digite a placa do veiculo que deseja editar (0 para voltar): ");

        if (placa_escolhida == "0") return;

        Veiculo* veiculo_para_editar = motorista->buscar_veiculo_por_placa(placa_escolhida);
        if (!veiculo_para_editar) {
            _terminal_io->exibirMensagem("Veiculo com a placa '" + placa_escolhida + "' nao encontrado.");
            return;
        }

        int escolha;
        do {
            _terminal_io->exibirMensagem("\n--- Editando Veiculo: " + placa_escolhida + " ---");
            veiculo_para_editar->exibir_info();
            _terminal_io->exibirMensagem("Escolha o campo para editar:");
            _terminal_io->exibirMensagem("(1) Marca | (2) Modelo | (3) Cor | (4) Lugares | (0) Voltar");
            escolha = _terminal_io->coletarIntInput("> ", 0, 4);
            std::string novo_str_valor;
            int novo_int_valor;

            switch (escolha) {
                case 1:
                    novo_str_valor = _terminal_io->coletarStringInput("Nova Marca: ");
                    veiculo_para_editar->set_marca(novo_str_valor);
                    break;
                case 2:
                    novo_str_valor = _terminal_io->coletarStringInput("Novo Modelo: ");
                    veiculo_para_editar->set_modelo(novo_str_valor);
                    break;
                case 3:
                    novo_str_valor = _terminal_io->coletarStringInput("Nova Cor: ");
                    veiculo_para_editar->set_cor(novo_str_valor);
                    break;
                case 4:
                    novo_int_valor = _terminal_io->coletarIntInput("Total de Lugares (com motorista): ", 2, 99);
                    veiculo_para_editar->set_lugares(novo_int_valor);
                    break;
                case 0:
                    _terminal_io->exibirMensagem("Voltando...");
                    break;
                default:
                    _terminal_io->exibirMensagem("Opcao invalida.");
                    break;
            }
            if (escolha != 0) {
                salvarDadosVeiculosPublico(_ger_usuarios);
                _terminal_io->exibirMensagem("Veiculo atualizado com sucesso!");
            }
        } while (escolha != 0);
    }

    void GerenciadorVeiculos::fluxo_excluir_veiculo_externo(Motorista* motorista) {
        if (motorista->get_veiculos().empty()) {
            _terminal_io->exibirMensagem("Nenhum veiculo para excluir.");
            return;
        }

        std::string placa_escolhida;
        placa_escolhida = _terminal_io->coletarStringInput("Digite a placa do veiculo que deseja excluir (0 para voltar): ");

        if (placa_escolhida == "0") return;

        char confirmar = _terminal_io->confirmarAcao("Tem certeza que deseja excluir o veiculo " + placa_escolhida + "? (s/n): ");

        if (confirmar == 's' || confirmar == 'S') {
            if (motorista->remover_veiculo(placa_escolhida)) {
                salvarDadosVeiculosPublico(_ger_usuarios);
                _terminal_io->exibirMensagem("Veiculo " + placa_escolhida + " excluido com sucesso!");
            } else {
                _terminal_io->exibirMensagem("Veiculo com a placa '" + placa_escolhida + "' nao encontrado.");
            }
        } else {
            _terminal_io->exibirMensagem("Exclusao cancelada.");
        }
    }

    Veiculo* GerenciadorVeiculos::buscarVeiculoPorPlacaMotorista(Motorista* motorista, const std::string& placa) const {
        if (!motorista) return nullptr;
        for (Veiculo* v : motorista->get_veiculos()) {
            if (v && v->get_placa() == placa) {
                return v;
            }
        }
        return nullptr;
    }

} // namespace ufmg_carona