#include "GerenciadorCaronas.hpp"
#include "TerminalIO.hpp"
#include "GerenciadorUsuarios.hpp"
#include "GerenciadorVeiculos.hpp"
#include "GerenciadorSolicitacoes.hpp"
#include "Usuario.hpp"
#include "Motorista.hpp"
#include "Veiculo.hpp"
#include "CaronaFactory.hpp"
#include "Excecoes.hpp"
#include "Zona.hpp"
#include "Utilitarios.hpp"
#include "Solicitacao.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <map>
#include <iomanip>

namespace ufmg_carona {

    int GerenciadorCaronas::_proximo_id_carona = 1;

    GerenciadorCaronas::GerenciadorCaronas(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios, GerenciadorVeiculos* ger_veiculos, GerenciadorSolicitacoes* ger_solicitacoes)
        : _terminal_io(terminal_io), _ger_usuarios(ger_usuarios), _ger_veiculos(ger_veiculos), _ger_solicitacoes(ger_solicitacoes) {
    }

    GerenciadorCaronas::~GerenciadorCaronas() {
    }

    void GerenciadorCaronas::setGerenciadorSolicitacoes(GerenciadorSolicitacoes* ger_solicitacoes) {
        _ger_solicitacoes = ger_solicitacoes;
    }

    void GerenciadorCaronas::carregarDados() {
    }

    void GerenciadorCaronas::carregarDadosCaronasPublico(GerenciadorUsuarios* ger_usuarios, GerenciadorVeiculos* ger_veiculos) {
        std::ifstream arquivo_caronas("caronas.txt");
        if (!arquivo_caronas.is_open()) {
            _terminal_io->exibirAviso("Arquivo caronas.txt nao encontrado. Nao ha caronas para carregar.");
            return;
        }

        std::string linha, cpf_motorista, origem_nome, destino_nome, data, apenas_mulheres_str, placa_veiculo_carona,
                    origem_zona_str, destino_zona_str, ufmg_posicao_str, status_str_lido;
        int caronas_carregadas = 0;
        while (std::getline(arquivo_caronas, linha)) {
            std::stringstream ss(linha);

            std::getline(ss, cpf_motorista, ';');
            std::getline(ss, origem_nome, ';');
            std::getline(ss, destino_nome, ';');
            std::getline(ss, origem_zona_str, ';');
            std::getline(ss, destino_zona_str, ';');
            std::getline(ss, ufmg_posicao_str, ';');
            std::getline(ss, data, ';');
            std::getline(ss, apenas_mulheres_str, ';');
            
            std::string resto_linha;
            std::getline(ss, resto_linha);
            
            size_t pos_status = resto_linha.find_last_of(';');
            if (pos_status != std::string::npos) {
                placa_veiculo_carona = resto_linha.substr(0, pos_status);
                status_str_lido = resto_linha.substr(pos_status + 1);
            } else {
                placa_veiculo_carona = resto_linha;
                status_str_lido = "AGUARDANDO";
            }

            bool apenas_mulheres_lida;
            try {
                apenas_mulheres_lida = (std::stoi(apenas_mulheres_str) == 1);
            } catch (const std::invalid_argument& e) {
                _terminal_io->exibirErro("ERRO: Valor invalido para 'apenas_mulheres' em caronas.txt para carona " + origem_nome + "->" + destino_nome + ". Ignorando carona. Erro: " + e.what());
                continue;
            } catch (const std::out_of_range& e) {
                _terminal_io->exibirErro("ERRO: Valor de 'apenas_mulheres' fora do limite para carona " + origem_nome + "->" + destino_nome + " em caronas.txt. Ignorando carona. Erro: " + e.what());
                continue;
            }

            Usuario* motorista_ptr = ger_usuarios->buscarUsuarioPorCpf(cpf_motorista);
            if (motorista_ptr) {
                if (!motorista_ptr->is_motorista()) {
                    _terminal_io->exibirAviso("AVISO: Usuario " + motorista_ptr->get_nome() + " (CPF: " + cpf_motorista + ") listado como motorista em caronas.txt mas nao e um Motorista valido no sistema. Carona nao carregada.");
                    continue;
                }
                Motorista* m_carona_ptr = dynamic_cast<Motorista*>(motorista_ptr);
                Veiculo* veiculo_usado_carona = nullptr;
                if (m_carona_ptr) {
                    veiculo_usado_carona = ger_veiculos->buscarVeiculoPorPlacaMotorista(m_carona_ptr, placa_veiculo_carona);
                } 
                
                if (!veiculo_usado_carona) {
                    _terminal_io->exibirAviso("AVISO: Motorista " + motorista_ptr->get_nome() + " (CPF: " + cpf_motorista + ") para carona em caronas.txt nao tem o veiculo " + placa_veiculo_carona + " cadastrado. Carona nao carregada.");
                    continue;
                }

                Utilitarios util;
                Zona origem_zona_lida = util.stringToZona(origem_zona_str);
                Zona destino_zona_lida = util.stringToZona(destino_zona_str);
                UFMGPosicao ufmg_posicao_lida = util.stringToUfmgPosicao(ufmg_posicao_str);

                Carona nova_carona_carregada = CaronaFactory::criar_carona(origem_nome, destino_nome, origem_zona_lida, destino_zona_lida, ufmg_posicao_lida, data, motorista_ptr, veiculo_usado_carona, apenas_mulheres_lida, TipoCarona::AGENDADA);
                
                if (status_str_lido == "CANCELADA") {
                    nova_carona_carregada.set_status(StatusCarona::CANCELADA);
                } else if (status_str_lido == "FINALIZADA") {
                    nova_carona_carregada.set_status(StatusCarona::FINALIZADA);
                } else if (status_str_lido == "EM_VIAGEM") {
                    nova_carona_carregada.set_status(StatusCarona::EM_VIAGEM);
                } else if (status_str_lido == "LOTADA") {
                    nova_carona_carregada.set_status(StatusCarona::LOTADA);
                } else {
                    nova_carona_carregada.set_status(StatusCarona::AGUARDANDO);
                }

                
                if (nova_carona_carregada.get_id() >= _proximo_id_carona) {
                    _proximo_id_carona = nova_carona_carregada.get_id() + 1;
                }
                
                _caronas.push_back(nova_carona_carregada);
                caronas_carregadas++;
            } else {
                _terminal_io->exibirAviso("AVISO: Motorista com CPF " + cpf_motorista + " para carona " + origem_nome + "->" + destino_nome + " nao encontrado. Carona nao carregada.");
            }
        }
        arquivo_caronas.close();
        Carona::set_proximo_id(_proximo_id_carona);
    }

    void GerenciadorCaronas::salvarDados() {
    }

    void GerenciadorCaronas::salvarDadosCaronasPublico() {
        std::ofstream arquivo_caronas("caronas.txt", std::ios::trunc);
        if (!arquivo_caronas.is_open()) {
            _terminal_io->exibirErro("Nao foi possivel abrir o arquivo caronas.txt para salvar dados.");
            return;
        }

        std::map<StatusCarona, std::string> status_carona_para_string = {
            {StatusCarona::AGUARDANDO, "AGUARDANDO"},
            {StatusCarona::LOTADA, "LOTADA"},
            {StatusCarona::EM_VIAGEM, "EM_VIAGEM"},
            {StatusCarona::FINALIZADA, "FINALIZADA"},
            {StatusCarona::CANCELADA, "CANCELADA"}
        };

        for (const auto& carona : _caronas) {
            if (carona.get_motorista() && carona.get_veiculo_usado()) {
                Utilitarios util;
                arquivo_caronas << carona.get_motorista()->get_cpf() << ";"
                                << carona.get_origem() << ";"
                                << carona.get_destino() << ";"
                                << util.zonaToString(carona.get_origem_zona()) << ";"
                                << util.zonaToString(carona.get_destino_zona()) << ";"
                                << util.ufmgPosicaoToString(carona.get_ufmg_posicao()) << ";"
                                << carona.get_data_hora() << ";"
                                << (carona.get_apenas_mulheres() ? "1" : "0") << ";"
                                << carona.get_veiculo_usado()->get_placa() << ";"
                                << status_carona_para_string[carona.get_status()]
                                << std::endl;
            } else {
                 _terminal_io->exibirAviso("AVISO: Carona (ID: " + std::to_string(carona.get_id()) + ") com motorista ou veiculo invalido. Nao salva.");
            }
        }
        arquivo_caronas.close();
    }


    void GerenciadorCaronas::fluxo_oferecer_carona(Usuario* usuario_logado) {
        if (!usuario_logado->is_motorista()) {
            throw AppExcecao("Voce nao pode oferecer caronas. Cadastre-se como motorista.");
        }
        
        Motorista* motorista_logado = dynamic_cast<Motorista*>(usuario_logado);
        if (!motorista_logado || motorista_logado->get_veiculos().empty()) {
             throw AppExcecao("Voce precisa cadastrar um veiculo para oferecer caronas.");
        }

        _terminal_io->exibirMensagem("\n--- Oferecer Carona ---");
        _terminal_io->exibirMensagem("Seus veiculos cadastrados:");
        for (size_t i = 0; i < motorista_logado->get_veiculos().size(); ++i) {
            _terminal_io->exibirMensagem("[" + std::to_string(i + 1) + "] ");
            if (motorista_logado->get_veiculos()[i]) {
                motorista_logado->get_veiculos()[i]->exibir_info();
            }
        }
        int indice_veiculo = _terminal_io->coletarIntInput("Digite o numero do veiculo que deseja usar para esta carona: ", 1, motorista_logado->get_veiculos().size());
        Veiculo* veiculo_selecionado = motorista_logado->buscar_veiculo_por_indice(indice_veiculo - 1);
        
        if (!veiculo_selecionado) {
            _terminal_io->exibirMensagem("Selecao de veiculo invalida.");
            return;
        }

        std::string origem_nome_str, destino_nome_str, data;
        Zona origem_zona, destino_zona;
        UFMGPosicao ufmg_posicao_escolhida;
        bool apenas_mulheres;
        Utilitarios util;

        ufmg_posicao_escolhida = _terminal_io->coletarUfmgPosicaoInput("A UFMG sera a origem ou o destino da carona?");

        if (ufmg_posicao_escolhida == UFMGPosicao::ORIGEM) {
            origem_nome_str = "UFMG Pampulha";
            origem_zona = Zona::PAMPULHA;
            destino_zona = _terminal_io->coletarZonaInput("Para qual zona administrativa voce ira?");
            destino_nome_str = util.zonaToString(destino_zona);
        } else {
            destino_nome_str = "UFMG Pampulha";
            destino_zona = Zona::PAMPULHA;
            origem_zona = _terminal_io->coletarZonaInput("De qual zona administrativa voce saira?");
            origem_nome_str = util.zonaToString(origem_zona);
        }

        data = _terminal_io->coletarStringInput("Data e Hora (ex: DD/MM/AAAA HH:MM): ");
        
        if (util.isDatetimeInPast(data)) {
            _terminal_io->exibirErro("ERRO: Nao e possivel oferecer caronas com data e hora no passado.");
            return;
        }

        if (usuario_logado->get_genero() == Genero::FEMININO) {
            apenas_mulheres = _terminal_io->confirmarAcao("Apenas para mulheres? (s/n): ");
        } else {
            apenas_mulheres = false;
            _terminal_io->exibirMensagem("Nota: A opcao 'Apenas para mulheres' esta disponivel apenas para usuarias do genero feminino.");
        }
        
        Carona nova_carona = CaronaFactory::criar_carona(origem_nome_str, destino_nome_str, origem_zona, destino_zona, ufmg_posicao_escolhida, data, usuario_logado, veiculo_selecionado, apenas_mulheres, TipoCarona::AGENDADA);
        _caronas.push_back(nova_carona);
        salvarDadosCaronasPublico();
        _terminal_io->exibirMensagem("Carona (ID: " + std::to_string(nova_carona.get_id()) + ") criada com sucesso usando o veiculo " + veiculo_selecionado->get_placa() + "!");
    }

    Carona* GerenciadorCaronas::buscarCaronaPorId(int id) {
        for (auto& carona : _caronas) {
            if (carona.get_id() == id) {
                return &carona;
            }
        }
        return nullptr;
    }

    void GerenciadorCaronas::removerCaronasPassadas() {
        Utilitarios util;
        std::vector<int> ids_caronas_removidas;
        auto it = _caronas.begin();
        while (it != _caronas.end()) {
            if (util.isDatetimeInPast(it->get_data_hora())) {
                ids_caronas_removidas.push_back(it->get_id());
                it = _caronas.erase(it);
            } else {
                ++it;
            }
        }

        if (!ids_caronas_removidas.empty()) {
            std::string ids_str = "-> Caronas expiradas removidas. IDs: ";
            for (int id : ids_caronas_removidas) {
                ids_str += std::to_string(id) + " ";
            }
            _terminal_io->exibirMensagem(ids_str);
        }
    }
    
    void GerenciadorCaronas::fluxo_gerenciar_caronas(Motorista* motorista_logado) {
        if (!motorista_logado->is_motorista()) {
            _terminal_io->exibirMensagem("Voce nao e motorista. Nao e possivel gerenciar caronas.");
            return;
        }
        
        int comando;
        do {
            _terminal_io->exibirMensagem("\n--- Gerenciar Caronas ---");
            _terminal_io->exibirMensagem("(1) Solicitacoes Pendentes | (2) Minhas Caronas | (0) Voltar");
            comando = _terminal_io->coletarIntInput("> ", 0, 2);

            try {
                if (comando == 1) {
                    
                    
                    if (_ger_solicitacoes) {
                        _ger_solicitacoes->solicitacoesPendentesMotorista(motorista_logado);
                    } else {
                        _terminal_io->exibirErro("Erro interno: Gerenciador de solicitacoes nao inicializado.");
                    }
                } else if (comando == 2) {
                    fluxo_minhas_caronas(motorista_logado);
                }
            } catch (const AppExcecao& e) {
                _terminal_io->exibirErro(e.what());
            }
        } while (comando != 0);
    }

    void GerenciadorCaronas::fluxo_minhas_caronas(Motorista* motorista_logado) {
        _terminal_io->exibirMensagem("\n--- Minhas Caronas Ofertadas ---");
        removerCaronasPassadas();

        std::vector<Carona*> caronas_do_motorista;
        for (auto& carona : _caronas) {
            if (carona.get_motorista() == motorista_logado &&
                carona.get_status() != StatusCarona::CANCELADA &&
                carona.get_status() != StatusCarona::FINALIZADA) {
                caronas_do_motorista.push_back(&carona);
            }
        }

        if (caronas_do_motorista.empty()) {
            _terminal_io->exibirMensagem("Voce nao possui caronas ofertadas no momento.");
        } else {
            _terminal_io->exibirMensagem("Suas caronas ativas:");
            Utilitarios util;
            for (size_t i = 0; i < caronas_do_motorista.size(); ++i) {
                Carona* carona = caronas_do_motorista[i];
                _terminal_io->exibirMensagem("\n[" + std::to_string(i + 1) + "] Carona ID: " + std::to_string(carona->get_id()));
                _terminal_io->exibirMensagem(" | De: " + carona->get_origem() + " Para: " + carona->get_destino());
                _terminal_io->exibirMensagem(" | Data/Hora: " + carona->get_data_hora());
                _terminal_io->exibirMensagem(" | Vagas: " + std::to_string(carona->get_vagas_disponiveis()));
                std::string status_str;
                switch (carona->get_status()) {
                    case StatusCarona::AGUARDANDO: status_str = "Aguardando Passageiros"; break;
                    case StatusCarona::LOTADA: status_str = "Lotada"; break;
                    case StatusCarona::EM_VIAGEM: status_str = "Em Viagem"; break;
                    case StatusCarona::FINALIZADA: status_str = "Finalizada"; break;
                    case StatusCarona::CANCELADA: status_str = "Cancelada"; break;
                }
                _terminal_io->exibirMensagem(" | Status: " + status_str);

                std::vector<Solicitacao*> passageiros_confirmados_solicitacoes;
                if (_ger_solicitacoes) {
                    const std::vector<Solicitacao*>& todas_solicitacoes = _ger_solicitacoes->getSolicitacoes();
                    for (const auto& solicitacao : todas_solicitacoes) {
                        if (solicitacao->get_carona() && solicitacao->get_carona() == carona && solicitacao->get_status() == StatusSolicitacao::ACEITA) {
                            passageiros_confirmados_solicitacoes.push_back(solicitacao);
                        }
                    }
                } else {
                    _terminal_io->exibirErro("Erro interno: Gerenciador de solicitacoes nao inicializado para listar passageiros.");
                }

                if (!passageiros_confirmados_solicitacoes.empty()) {
                    _terminal_io->exibirMensagem("  Passageiros confirmados (" + std::to_string(passageiros_confirmados_solicitacoes.size()) + "):");
                    for (Solicitacao* sol : passageiros_confirmados_solicitacoes) {
                        Usuario* passageiro = sol->get_passageiro();
                        _terminal_io->exibirMensagem("    - " + (passageiro ? passageiro->get_nome() : "N/A") +
                                                     " (Tel: " + (passageiro ? passageiro->get_telefone() : "N/A") +
                                                     ", Email: " + (passageiro ? passageiro->get_email() : "N/A") + ")");
                        if (sol->get_motorista_avaliou_passageiro()) {
                            _terminal_io->exibirMensagem(" [Voce ja avaliou]");
                        } else {
                            _terminal_io->exibirMensagem(" [Pendente de sua avaliacao]");
                        }
                    }
                } else {
                    _terminal_io->exibirMensagem("  Nenhum passageiro confirmado ainda.");
                }
            }
        }

        int escolha_carona_idx = _terminal_io->coletarIntInput("\nDigite o numero da carona para gerenciar (0 para voltar): ", 0, caronas_do_motorista.size());
        if (escolha_carona_idx == 0) return;

        Carona* carona_escolhida = caronas_do_motorista[escolha_carona_idx - 1];

        char acao_carona;
        _terminal_io->exibirMensagem("\nGerenciar Carona ID: " + std::to_string(carona_escolhida->get_id()));
        _terminal_io->exibirMensagem("(F)inalizar Carona | (C)ancelar Carona | (0) Voltar: ");
        acao_carona = _terminal_io->confirmarAcao("");

        if (acao_carona == 'F' || acao_carona == 'f') {
            char confirmar_finalizacao = _terminal_io->confirmarAcao("Tem certeza que deseja FINALIZAR esta carona? (s/n): ");
            if (confirmar_finalizacao == 's' || confirmar_finalizacao == 'S') {
                finalizarCarona(carona_escolhida);
                _terminal_io->exibirMensagem("Carona ID " + std::to_string(carona_escolhida->get_id()) + " finalizada com sucesso.");
            } else {
                _terminal_io->exibirMensagem("Finalizacao de carona abortada.");
            }
        }
        else if (acao_carona == 'C' || acao_carona == 'c') {
            char confirmar_cancelamento = _terminal_io->confirmarAcao("Tem certeza que deseja CANCELAR esta carona para TODOS os passageiros? (s/n): ");
            if (confirmar_cancelamento == 's' || confirmar_cancelamento == 'S') {
                cancelarCarona(carona_escolhida);
                _terminal_io->exibirMensagem("Carona ID " + std::to_string(carona_escolhida->get_id()) + " cancelada com sucesso.");
            } else {
                _terminal_io->exibirMensagem("Cancelamento de carona abortado.");
            }
        } else {
            _terminal_io->exibirMensagem("Voltando...");
        }
    }

    void GerenciadorCaronas::finalizarCarona(Carona* carona_para_finalizar) {
        if (!carona_para_finalizar) return;

        carona_para_finalizar->set_status(StatusCarona::FINALIZADA);

        if (_ger_solicitacoes) {
            const std::vector<Solicitacao*>& todas_solicitacoes = _ger_solicitacoes->getSolicitacoes();
            for (Solicitacao* s : todas_solicitacoes) {
                if (s->get_carona() && s->get_carona() == carona_para_finalizar && s->get_status() == StatusSolicitacao::ACEITA) {
                    _ger_usuarios->enviarNotificacao(s->get_passageiro(), "A carona ID " + std::to_string(carona_para_finalizar->get_id()) +
                                     " de " + (carona_para_finalizar->get_motorista() ? carona_para_finalizar->get_motorista()->get_nome() : "Motorista Desconhecido") + " foi FINALIZADA. Voce ja pode avalia-la!");
                }
            }
            _ger_solicitacoes->salvarDadosSolicitacoesPublico();
        } else {
            _terminal_io->exibirErro("Erro interno: Gerenciador de solicitacoes nao inicializado para finalizar carona.");
        }
        salvarDadosCaronasPublico();
    }

    void GerenciadorCaronas::cancelarCarona(Carona* carona_para_cancelar) {
        if (!carona_para_cancelar) return;

        carona_para_cancelar->set_status(StatusCarona::CANCELADA);

        if (_ger_solicitacoes) {
            const std::vector<Solicitacao*>& todas_solicitacoes = _ger_solicitacoes->getSolicitacoes();
            for (Solicitacao* s : todas_solicitacoes) {
                if (s->get_carona() && s->get_carona() == carona_para_cancelar) {
                    if (s->get_status() == StatusSolicitacao::ACEITA ||
                        s->get_status() == StatusSolicitacao::PENDENTE ||
                        s->get_status() == StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO) {
                        
                        s->set_status(StatusSolicitacao::RECUSADA);
                        _ger_usuarios->enviarNotificacao(s->get_passageiro(), "Sua solicitacao de carona para a carona ID " + (carona_para_cancelar ? std::to_string(carona_para_cancelar->get_id()) : "N/A") +
                                         " de " + (carona_para_cancelar && carona_para_cancelar->get_motorista() ? carona_para_cancelar->get_motorista()->get_nome() : "Motorista Desconhecido") + " foi CANCELADA. Por favor, busque outra carona.");
                        s->set_carona(nullptr);
                    }
                }
            }
            _ger_solicitacoes->salvarDadosSolicitacoesPublico();
        } else {
            _terminal_io->exibirErro("Erro interno: Gerenciador de solicitacoes nao inicializado para cancelar carona.");
        }
        salvarDadosCaronasPublico();
    }

    const std::vector<Carona>& GerenciadorCaronas::getTodasCaronas() const {
        return _caronas;
    }

}