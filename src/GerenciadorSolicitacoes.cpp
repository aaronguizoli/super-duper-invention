#include "GerenciadorSolicitacoes.hpp"
#include "TerminalIO.hpp"
#include "GerenciadorUsuarios.hpp"
#include "GerenciadorCaronas.hpp"
#include "Usuario.hpp"
#include "Motorista.hpp"
#include "Carona.hpp"
#include "Solicitacao.hpp"
#include "Excecoes.hpp"
#include "Utilitarios.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

namespace ufmg_carona {

    GerenciadorSolicitacoes::GerenciadorSolicitacoes(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios, GerenciadorCaronas* ger_caronas)
        : _terminal_io(terminal_io), _ger_usuarios(ger_usuarios), _ger_caronas(ger_caronas) {
    }

    GerenciadorSolicitacoes::~GerenciadorSolicitacoes() {
        for (Solicitacao* s : _solicitacoes) {
            delete s;
        }
        _solicitacoes.clear();
    }

    void GerenciadorSolicitacoes::setGerenciadorCaronas(GerenciadorCaronas* ger_caronas) {
        _ger_caronas = ger_caronas;
    }

    void GerenciadorSolicitacoes::carregarDados() {
    }

    void GerenciadorSolicitacoes::carregarDadosSolicitacoesPublico(GerenciadorUsuarios* ger_usuarios, GerenciadorCaronas* ger_caronas) {
        std::ifstream arquivo_solicitacoes("solicitacoes.txt");
        if (!arquivo_solicitacoes.is_open()) {
            _terminal_io->exibirAviso("Arquivo solicitacoes.txt nao encontrado. Nao ha solicitacoes para carregar.");
            return;
        }

        std::string linha, cpf_passageiro, carona_id_str, status_str,
                    local_embarque_passageiro, local_desembarque_passageiro,
                    local_embarque_motorista_proposto, local_desembarque_motorista_proposto,
                    passageiro_avaliou_str, motorista_avaliou_str;
        
        while (std::getline(arquivo_solicitacoes, linha)) {
            std::stringstream ss(linha);
            std::getline(ss, cpf_passageiro, ';');
            std::getline(ss, carona_id_str, ';');
            std::getline(ss, status_str, ';');

            
            std::getline(ss, local_embarque_passageiro, ';');
            std::getline(ss, local_desembarque_passageiro, ';');
            std::getline(ss, local_embarque_motorista_proposto, ';');
            std::getline(ss, local_desembarque_motorista_proposto, ';');
            std::getline(ss, passageiro_avaliou_str, ';');
            std::getline(ss, motorista_avaliou_str);

            int carona_id;
            try {
                carona_id = std::stoi(carona_id_str);
            } catch (const std::invalid_argument& e) {
                _terminal_io->exibirErro("ERRO: ID de carona invalido em solicitacoes.txt: " + carona_id_str + ". Ignorando solicitacao. Erro: " + e.what());
                continue;
            } catch (const std::out_of_range& e) {
                _terminal_io->exibirErro("ERRO: ID de carona fora do limite em solicitacoes.txt: " + carona_id_str + ". Ignorando solicitacao. Erro: " + e.what());
                continue;
            }

            StatusSolicitacao status_lido;
            if (status_str == "PENDENTE") status_lido = StatusSolicitacao::PENDENTE;
            else if (status_str == "ACEITA") status_lido = StatusSolicitacao::ACEITA;
            else if (status_str == "RECUSADA") status_lido = StatusSolicitacao::RECUSADA;
            else if (status_str == "AGUARDANDO RESPOSTA DO PASSAGEIRO") status_lido = StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO;
            else if (status_str == "RECUSADA_PROPOSTA_MOTORISTA") status_lido = StatusSolicitacao::RECUSADA_PROPOSTA_MOTORISTA;
            else {
                _terminal_io->exibirErro("ERRO: Status de solicitacao invalido em solicitacoes.txt: " + status_str + ". Ignorando solicitacao. ");
                continue;
            }

            Usuario* passageiro_ptr = ger_usuarios->buscarUsuarioPorCpf(cpf_passageiro);
            Carona* carona_alvo_ptr = ger_caronas->buscarCaronaPorId(carona_id);

            if (passageiro_ptr && carona_alvo_ptr) {
                Solicitacao* nova_solicitacao = new Solicitacao(passageiro_ptr, carona_alvo_ptr, local_embarque_passageiro, local_desembarque_passageiro);
                nova_solicitacao->propor_locais_motorista(local_embarque_motorista_proposto, local_desembarque_motorista_proposto);
                
                nova_solicitacao->set_status(status_lido);
                nova_solicitacao->set_passageiro_avaliou_motorista(std::stoi(passageiro_avaliou_str));
                nova_solicitacao->set_motorista_avaliou_passageiro(std::stoi(motorista_avaliou_str));

                if (status_lido == StatusSolicitacao::ACEITA) {
                    if (carona_alvo_ptr->get_vagas_disponiveis() > 0) {
                        carona_alvo_ptr->adicionar_passageiro(passageiro_ptr);
                    } else {
                        _terminal_io->exibirAviso("AVISO: Solicitacao " + std::to_string(carona_id) + " ACEITA mas carona nao tem vagas. Dados inconsistentes, status ajustado para RECUSADA.");
                        nova_solicitacao->set_status(StatusSolicitacao::RECUSADA);
                    }
                }
                
                _solicitacoes.push_back(nova_solicitacao);
                carona_alvo_ptr->adicionar_solicitacao(nova_solicitacao);
            } else {
                _terminal_io->exibirAviso("AVISO: Passageiro (" + cpf_passageiro + ") ou Carona (ID: " + std::to_string(carona_id) + ") nao encontrados para solicitacao. Ignorando.");
            }
        }
        arquivo_solicitacoes.close();
    }

    void GerenciadorSolicitacoes::salvarDados() {
    }

    void GerenciadorSolicitacoes::salvarDadosSolicitacoesPublico() {
        std::ofstream arquivo_solicitacoes("solicitacoes.txt", std::ios::trunc);
        if (!arquivo_solicitacoes.is_open()) {
            _terminal_io->exibirErro("Nao foi possivel abrir o arquivo solicitacoes.txt para salvar dados.");
            return;
        }

        for (const auto& s : _solicitacoes) {
            if (s && s->get_passageiro() && s->get_carona()) {
                arquivo_solicitacoes << s->get_passageiro()->get_cpf() << ";"
                                     << s->get_carona()->get_id() << ";"
                                     << s->get_status_string() << ";"
                                     << s->get_local_embarque_passageiro() << ";"
                                     << s->get_local_desembarque_passageiro() << ";"
                                     << s->get_local_embarque_motorista_proposto() << ";"
                                     << s->get_local_desembarque_motorista_proposto() << ";"
                                     << (s->get_passageiro_avaliou_motorista() ? "1" : "0") << ";"
                                     << (s->get_motorista_avaliou_passageiro() ? "1" : "0")
                                     << std::endl;
            }
        }
        arquivo_solicitacoes.close();
    }

    void GerenciadorSolicitacoes::fluxo_solicitar_carona(Usuario* passageiro) {
        _terminal_io->exibirMensagem("\n--- Solicitar Carona ---");
        if (!_ger_caronas) {
            _terminal_io->exibirErro("Erro interno: Gerenciador de caronas nao inicializado. Nao e possivel solicitar carona.");
            return;
        }
        _ger_caronas->removerCaronasPassadas();

        std::string filtro_origem_str, filtro_destino_str, filtro_data, filtro_hora;
        Zona filtro_origem_zona = Zona::PAMPULHA;
        Zona filtro_destino_zona = Zona::PAMPULHA;
        bool apenas_mulheres_filtro = false;

        char usar_filtro = _terminal_io->confirmarAcao("Deseja usar filtros para buscar caronas? (s/n): ");

        if (usar_filtro == 's' || usar_filtro == 'S') {
            _terminal_io->exibirMensagem("Filtro de Origem (0 para ignorar, ou selecione uma zona): ");
            Utilitarios util;
            int escolha_origem_filtro = _terminal_io->coletarIntInput("(0) Ignorar | " + [this, &util]() {
                std::string s = "";
                for (const auto& pair : util.getIntParaZona()) {
                    s += "(" + std::to_string(pair.first) + ") " + util.zonaToString(pair.second) + " | ";
                }
                return s;
            }(), 0, util.getIntParaZona().size());
            if (escolha_origem_filtro != 0) {
                filtro_origem_zona = util.getIntParaZona().at(escolha_origem_filtro);
                filtro_origem_str = util.zonaToString(filtro_origem_zona);
            }

            _terminal_io->exibirMensagem("Filtro de Destino (0 para ignorar, ou selecione uma zona): ");
            int escolha_destino_filtro = _terminal_io->coletarIntInput("(0) Ignorar | " + [this, &util]() {
                std::string s = "";
                for (const auto& pair : util.getIntParaZona()) {
                    s += "(" + std::to_string(pair.first) + ") " + util.zonaToString(pair.second) + " | ";
                }
                return s;
            }(), 0, util.getIntParaZona().size());
            if (escolha_destino_filtro != 0) {
                filtro_destino_zona = util.getIntParaZona().at(escolha_destino_filtro);
                filtro_destino_str = util.zonaToString(filtro_destino_zona);
            }

            filtro_data = _terminal_io->coletarStringInput("Filtro de Data (DD/MM/AAAA, deixe em branco para ignorar): ");
            filtro_hora = _terminal_io->coletarStringInput("Filtro de Hora (HH:MM, deixe em branco para ignorar): ");
            
            if (passageiro->get_genero() == Genero::FEMININO) {
                apenas_mulheres_filtro = _terminal_io->confirmarAcao("Deseja caronas apenas com motoristas do genero feminino? (s/n): ");
            } else {
                _terminal_io->exibirMensagem("Nota: A opcao de filtrar caronas por genero esta disponivel apenas para usuarias do genero feminino.");
            }
        }

        std::vector<const Carona*> caronas_filtradas;
        const std::vector<Carona>& todas_caronas = _ger_caronas->getTodasCaronas();
        Utilitarios util;

        for (const auto& carona : todas_caronas) {
            if (carona.get_motorista() == passageiro) {
                continue;
            }

            if (!carona.get_motorista()) {
                _terminal_io->exibirAviso("Carona ID " + std::to_string(carona.get_id()) + " tem motorista nulo. Ignorando.");
                continue;
            }
            if (!carona.get_veiculo_usado()) {
                _terminal_io->exibirAviso("Carona ID " + std::to_string(carona.get_id()) + " tem veiculo nulo. Ignorando.");
                continue;
            }

            if (util.isDatetimeInPast(carona.get_data_hora()) || carona.get_status() == StatusCarona::CANCELADA || carona.get_status() == StatusCarona::FINALIZADA) {
                 continue;
            }

            if (carona.get_vagas_disponiveis() <= 0) {
                continue;
            }

            bool atende_filtro_origem = filtro_origem_str.empty() || carona.get_origem_zona() == filtro_origem_zona;
            bool atende_filtro_destino = filtro_destino_str.empty() || carona.get_destino_zona() == filtro_destino_zona;
            bool atende_filtro_data = filtro_data.empty() || carona.get_data_hora().find(filtro_data) != std::string::npos;
            bool atende_filtro_hora = filtro_hora.empty() || carona.get_data_hora().find(filtro_hora) != std::string::npos;

            bool atende_filtro_genero = true;
            if (carona.get_apenas_mulheres() && passageiro->get_genero() != Genero::FEMININO) {
                atende_filtro_genero = false;
            }
            if (apenas_mulheres_filtro) {
                if (!carona.get_apenas_mulheres() && carona.get_motorista()->get_genero() != Genero::FEMININO) {
                    atende_filtro_genero = false;
                }
            }

            if (atende_filtro_origem && atende_filtro_destino && atende_filtro_data && atende_filtro_hora && atende_filtro_genero) {
                caronas_filtradas.push_back(&carona);
            }
        }

        if (todas_caronas.empty() || caronas_filtradas.empty()) {
            _terminal_io->exibirMensagem("Nenhuma carona disponivel no momento ou com os filtros especificados.");
            return;
        }

        _terminal_io->exibirMensagem("\nCaronas disponiveis (filtradas):");
        for (const auto& carona_ptr : caronas_filtradas) {
            if (!carona_ptr) {
                 _terminal_io->exibirAviso("Ponteiro de carona nulo na lista filtrada.");
                 continue;
            }
            carona_ptr->exibir_info();
        }

        int id_carona;
        id_carona = _terminal_io->coletarIntInput("\nDigite o ID da carona que deseja solicitar (0 para voltar): ", 0, std::numeric_limits<int>::max());

        if (id_carona == 0) return;

        Carona* carona_escolhida = _ger_caronas->buscarCaronaPorId(id_carona);
        if (!carona_escolhida) {
            _terminal_io->exibirMensagem("Carona nao encontrada!");
            return;
        }

        if (!podeSolicitarCarona(passageiro, *carona_escolhida)) {
            return;
        }

        std::string local_embarque = _terminal_io->coletarStringInput("Local de embarque desejado: ");
        std::string local_desembarque = _terminal_io->coletarStringInput("Local de desembarque desejado: ");

        Solicitacao* nova_solicitacao = new Solicitacao(passageiro, carona_escolhida, local_embarque, local_desembarque);
        _solicitacoes.push_back(nova_solicitacao);
        carona_escolhida->adicionar_solicitacao(nova_solicitacao);
        salvarDadosSolicitacoesPublico();

        std::string mensagem = "Nova solicitacao de carona de " + passageiro->get_nome() +
                              " para a carona ID: " + std::to_string(id_carona) +
                              ". Locais: Embarque em '" + local_embarque + "' e Desembarque em '" + local_desembarque + "'.";
        if (!carona_escolhida->get_motorista()) {
            _terminal_io->exibirErro("Erro: Motorista da carona escolhida é nulo. Nao foi possivel enviar notificacao.");
        } else {
            _ger_usuarios->enviarNotificacao(carona_escolhida->get_motorista(), mensagem);
        }

        _terminal_io->exibirMensagem("Solicitacao enviada com sucesso!");
    }

    void GerenciadorSolicitacoes::fluxo_status_caronas(Usuario* passageiro) {
        _terminal_io->exibirMensagem("\n--- Status das Minhas Solicitacoes ---");

        std::vector<Solicitacao*> minhas_solicitacoes;
        for (const auto& solicitacao : _solicitacoes) {
            if (solicitacao->get_passageiro() == passageiro) {
                minhas_solicitacoes.push_back(solicitacao);
            }
        }

        if (minhas_solicitacoes.empty()) {
            _terminal_io->exibirMensagem("Voce nao fez nenhuma solicitacao de carona ainda.");
            return;
        }

        Utilitarios util;
        for (size_t i = 0; i < minhas_solicitacoes.size(); ++i) {
            Solicitacao* solicitacao = minhas_solicitacoes[i];
            if (!solicitacao->get_carona()) {
                _terminal_io->exibirAviso("AVISO: Solicitacao com carona invalida ignorada ao exibir status.");
                continue;
            }

            if (solicitacao->get_carona()->get_status() == StatusCarona::CANCELADA) {
                continue;
            }

            _terminal_io->exibirMensagem("\n--- Solicitacao [" + std::to_string(i+1) + "] ---");
            _terminal_io->exibirMensagem("Carona: " + util.zonaToString(solicitacao->get_carona()->get_origem_zona()) +
                                       " -> " + util.zonaToString(solicitacao->get_carona()->get_destino_zona()));
            _terminal_io->exibirMensagem("Data: " + solicitacao->get_carona()->get_data_hora());
            _terminal_io->exibirMensagem("Motorista: " + (solicitacao->get_carona()->get_motorista() ? solicitacao->get_carona()->get_motorista()->get_nome() : "N/A"));
            _terminal_io->exibirMensagem("Status: " + solicitacao->get_status_string());
            
            if (solicitacao->get_status() == StatusSolicitacao::ACEITA && !solicitacao->get_local_embarque_motorista_proposto().empty()) {
                _terminal_io->exibirMensagem("  Seu local de embarque: '" + solicitacao->get_local_embarque_motorista_proposto() + "'");
                _terminal_io->exibirMensagem("  Seu local de desembarque: '" + solicitacao->get_local_desembarque_motorista_proposto() + "'");
            } else {
                _terminal_io->exibirMensagem("  Seu local de embarque desejado: '" + solicitacao->get_local_embarque_passageiro() + "'");
                _terminal_io->exibirMensagem("  Seu local de desembarque desejado: '" + solicitacao->get_local_desembarque_passageiro() + "'");
            }

            if (solicitacao->get_status() == StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO) {
                _terminal_io->exibirMensagem("  Motorista propos:\n    Embarque: '" + solicitacao->get_local_embarque_motorista_proposto() + "'\n    Desembarque: '" + solicitacao->get_local_desembarque_motorista_proposto() + "'");
                char resposta_proposta = _terminal_io->confirmarAcao("Aceita a proposta do motorista para esta carona? (s/n): ");

                if (resposta_proposta == 's' || resposta_proposta == 'S') {
                    if (solicitacao->get_carona() && solicitacao->get_carona()->get_vagas_disponiveis() > 0) {
                        solicitacao->aceitar_proposta_motorista();
                        solicitacao->get_carona()->adicionar_passageiro(solicitacao->get_passageiro());
                        _ger_usuarios->enviarNotificacao(solicitacao->get_carona()->get_motorista(),
                                        "O passageiro " + (solicitacao->get_passageiro() ? solicitacao->get_passageiro()->get_nome() : "N/A") + " ACEITOU sua proposta de locais para a carona ID " + std::to_string(solicitacao->get_carona()->get_id()) + "!");
                        _terminal_io->exibirMensagem("Proposta do motorista aceita! Carona confirmada.");
                        cancelarOutrasSolicitacoesPassageiro(solicitacao->get_passageiro(), *solicitacao->get_carona());
                    } else {
                        solicitacao->recusar();
                        _ger_usuarios->enviarNotificacao(solicitacao->get_carona()->get_motorista(),
                                        "O passageiro " + (solicitacao->get_passageiro() ? solicitacao->get_passageiro()->get_nome() : "N/A") + " ACEITOU sua proposta, mas a carona nao tem mais vagas. Solicitacao RECUSADA.");
                        _ger_usuarios->enviarNotificacao(solicitacao->get_passageiro(),
                                        "Sua aceitacao da proposta do motorista nao pode ser concluida: a carona nao possui mais vagas.");
                        _terminal_io->exibirMensagem("Nao foi possivel confirmar a carona: sem vagas disponiveis.");
                    }
                } else {
                    solicitacao->recusar_proposta_motorista();
                    _ger_usuarios->enviarNotificacao(solicitacao->get_carona()->get_motorista(),
                                     "O passageiro " + (solicitacao->get_passageiro() ? solicitacao->get_passageiro()->get_nome() : "N/A") + " RECUSOU sua proposta de locais para a carona ID " + std::to_string(solicitacao->get_carona()->get_id()) + ".");
                    _terminal_io->exibirMensagem("Proposta do motorista recusada pelo passageiro. Solicitacao nao confirmada.");
                }
                salvarDadosSolicitacoesPublico();
            }

            if (solicitacao->get_carona() && util.isDatetimeInPast(solicitacao->get_carona()->get_data_hora())) {
                _terminal_io->exibirMensagem("OBS: Esta carona ja ocorreu ou o horario de partida ja passou.");
            }
        }
    }

    void GerenciadorSolicitacoes::solicitacoesPendentesMotorista(Motorista* motorista) {
        if (!motorista->is_motorista()) {
            _terminal_io->exibirMensagem("Apenas motoristas podem gerenciar solicitacoes.");
            return;
        }

        std::vector<Solicitacao*> solicitacoes_motorista_pendentes;
        std::vector<Solicitacao*> solicitacoes_motorista_propostas;

        for (const auto& solicitacao : _solicitacoes) {
            if (!solicitacao->get_carona()) {
                _terminal_io->exibirAviso("AVISO: Solicitacao com carona invalida ignorada.");
                continue;
            }

            Utilitarios util;
            if (solicitacao->get_carona()->get_motorista() == motorista &&
                !util.isDatetimeInPast(solicitacao->get_carona()->get_data_hora())) {
                
                if (solicitacao->get_status() == StatusSolicitacao::PENDENTE) {
                    solicitacoes_motorista_pendentes.push_back(solicitacao);
                } else if (solicitacao->get_status() == StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO) {
                    solicitacoes_motorista_propostas.push_back(solicitacao);
                }
            }
        }

        if (solicitacoes_motorista_pendentes.empty() && solicitacoes_motorista_propostas.empty()) {
            _terminal_io->exibirMensagem("Nenhuma solicitacao pendente ou com proposta aguardando resposta.");
            return;
        }

        if (!solicitacoes_motorista_pendentes.empty()) {
            _terminal_io->exibirMensagem("\n--- Solicitacoes PENDENTES (aguardando sua decisao) ---");
            for (size_t i = 0; i < solicitacoes_motorista_pendentes.size(); ++i) {
                _terminal_io->exibirMensagem("[" + std::to_string(i+1) + "] ");
                solicitacoes_motorista_pendentes[i]->exibir_para_motorista();
                _terminal_io->exibirMensagem("    Local de Embarque Passageiro: '" + solicitacoes_motorista_pendentes[i]->get_local_embarque_passageiro() + "'");
                _terminal_io->exibirMensagem("    Local de Desembarque Passageiro: '" + solicitacoes_motorista_pendentes[i]->get_local_desembarque_passageiro() + "'");
            }
        }

        if (!solicitacoes_motorista_propostas.empty()) {
            _terminal_io->exibirMensagem("\n--- Solicitacoes com PROPOSTAS AGUARDANDO RESPOSTA DO PASSAGEIRO ---");
            for (size_t i = 0; i < solicitacoes_motorista_propostas.size(); ++i) {
                _terminal_io->exibirMensagem("[" + std::to_string(solicitacoes_motorista_pendentes.size() + i + 1) + "] ");
                solicitacoes_motorista_propostas[i]->exibir_para_motorista();
                _terminal_io->exibirMensagem("    Sua Proposta de Embarque: '" + solicitacoes_motorista_propostas[i]->get_local_embarque_motorista_proposto() + "'");
                _terminal_io->exibirMensagem("    Sua Proposta de Desembarque: '" + solicitacoes_motorista_propostas[i]->get_local_desembarque_motorista_proposto() + "'");
            }
        }

        int escolha;
        escolha = _terminal_io->coletarIntInput("\nEscolha uma solicitacao para responder (0 para voltar): ", 0, solicitacoes_motorista_pendentes.size() + solicitacoes_motorista_propostas.size());

        if (escolha == 0) {
            return;
        }

        Solicitacao* solicitacao_escolhida = nullptr;
        if (escolha <= static_cast<int>(solicitacoes_motorista_pendentes.size())) {
            solicitacao_escolhida = solicitacoes_motorista_pendentes[escolha - 1];
        } else {
            solicitacao_escolhida = solicitacoes_motorista_propostas[escolha - (solicitacoes_motorista_pendentes.size() + 1)];
        }

        if (solicitacao_escolhida->get_status() == StatusSolicitacao::PENDENTE) {
            char acao_motorista;
            _terminal_io->exibirMensagem("\n--- Responder Solicitacao de " + (solicitacao_escolhida->get_passageiro() ? solicitacao_escolhida->get_passageiro()->get_nome() : "N/A") + " ---");
            _terminal_io->exibirMensagem("Locais desejados pelo passageiro:");
            _terminal_io->exibirMensagem("  Embarque: '" + solicitacao_escolhida->get_local_embarque_passageiro() + "'");
            _terminal_io->exibirMensagem("  Desembarque: '" + solicitacao_escolhida->get_local_desembarque_passageiro() + "'");
            acao_motorista = _terminal_io->confirmarAcao("(A)ceitar | (R)ecusar | (P)ropor novos locais | (0) Voltar: ");

            if (acao_motorista == 'A' || acao_motorista == 'a') {
                if (solicitacao_escolhida->get_carona() && solicitacao_escolhida->get_carona()->get_vagas_disponiveis() > 0) {
                    solicitacao_escolhida->aceitar();
                    solicitacao_escolhida->get_carona()->adicionar_passageiro(solicitacao_escolhida->get_passageiro());
                    _ger_usuarios->enviarNotificacao(solicitacao_escolhida->get_passageiro(),
                                    "Sua solicitacao de carona foi ACEITA! Locais confirmados: Embarque em '" + solicitacao_escolhida->get_local_embarque_passageiro() + "' e Desembarque em '" + solicitacao_escolhida->get_local_desembarque_passageiro() + "'.");
                    _terminal_io->exibirMensagem("Solicitacao aceita!");
                    cancelarOutrasSolicitacoesPassageiro(solicitacao_escolhida->get_passageiro(), *solicitacao_escolhida->get_carona());
                } else {
                    solicitacao_escolhida->recusar();
                    _ger_usuarios->enviarNotificacao(solicitacao_escolhida->get_passageiro(),
                                    "Sua solicitacao de carona foi RECUSADA. Carona sem vagas.");
                    _terminal_io->exibirMensagem("Nao foi possivel aceitar a solicitacao: carona sem vagas.");
                }
            } else if (acao_motorista == 'R' || acao_motorista == 'r') {
                solicitacao_escolhida->recusar();
                _ger_usuarios->enviarNotificacao(solicitacao_escolhida->get_passageiro(),
                                 "Sua solicitacao de carona foi RECUSADA.");
                _terminal_io->exibirMensagem("Solicitacao recusada.");
            } else if (acao_motorista == 'P' || acao_motorista == 'p') {
                std::string nova_origem_motorista = _terminal_io->coletarStringInput("Propor novo local de embarque: ");
                std::string novo_destino_motorista = _terminal_io->coletarStringInput("Propor novo local de desembarque: ");
                solicitacao_escolhida->propor_locais_motorista(nova_origem_motorista, novo_destino_motorista);
                _ger_usuarios->enviarNotificacao(solicitacao_escolhida->get_passageiro(),
                                 "O motorista propos novos locais para sua solicitacao de carona. Por favor, verifique.");
                _terminal_io->exibirMensagem("Proposta de novos locais enviada ao passageiro.");
            } else {
                _terminal_io->exibirMensagem("Acao cancelada.");
            }
        } else if (solicitacao_escolhida->get_status() == StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO) {
            _terminal_io->exibirMensagem("Esta solicitacao ja possui uma proposta sua aguardando resposta do passageiro.");
        }
        salvarDadosSolicitacoesPublico();
    }

    bool GerenciadorSolicitacoes::podeSolicitarCarona(Usuario* passageiro, const Carona& carona) {
        if (passageiro == carona.get_motorista()) {
            _terminal_io->exibirMensagem("Voce nao pode solicitar sua propria carona!");
            return false;
        }

        if (carona.get_vagas_disponiveis() <= 0) {
            _terminal_io->exibirMensagem("Esta carona nao possui vagas disponiveis.");
            return false;
        }
        
        if (carona.get_status() == StatusCarona::CANCELADA || carona.get_status() == StatusCarona::FINALIZADA) {
            _terminal_io->exibirMensagem("Esta carona nao esta mais disponivel para solicitacao.");
            return false;
        }

        for (const auto& solicitacao : _solicitacoes) {
            if (!solicitacao->get_carona()) continue;

            if (solicitacao->get_passageiro() == passageiro &&
                solicitacao->get_carona()->get_id() == carona.get_id()) {
                if (solicitacao->get_status() == StatusSolicitacao::PENDENTE) {
                    _terminal_io->exibirMensagem("Voce ja tem uma solicitacao PENDENTE para esta carona!");
                    return false;
                } else if (solicitacao->get_status() == StatusSolicitacao::ACEITA) {
                    _terminal_io->exibirMensagem("Voce ja foi ACEITO nesta carona!");
                    return false;
                } else if (solicitacao->get_status() == StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO) {
                    _terminal_io->exibirMensagem("Voce ja tem uma proposta do motorista para esta carona aguardando sua resposta!");
                    return false;
                }
            }
        }
        return true;
    }

    void GerenciadorSolicitacoes::cancelarOutrasSolicitacoesPassageiro(Usuario* passageiro, const Carona& carona_aceita) {
        _terminal_io->exibirMensagem("-> Verificando e cancelando outras solicitacoes do passageiro...");
        Utilitarios util;
        
        for (Solicitacao* s : _solicitacoes) {
            if (s->get_passageiro() == passageiro && 
                s->get_carona() &&
                s->get_carona()->get_id() != carona_aceita.get_id() &&
                (s->get_status() == StatusSolicitacao::PENDENTE || s->get_status() == StatusSolicitacao::AGUARDANDO_RESPOSTA_PASSAGEIRO) &&
                s->get_carona()->get_destino_zona() == carona_aceita.get_destino_zona() &&
                s->get_carona()->get_data_hora().substr(0, 10) == carona_aceita.get_data_hora().substr(0, 10)) {
                
                s->recusar();
                _ger_usuarios->enviarNotificacao(s->get_passageiro(), "Sua solicitacao para carona " + (s->get_carona() ? std::to_string(s->get_carona()->get_id()) : "N/A") +
                                 " foi automaticamente cancelada pois outra carona para o mesmo destino/dia foi aceita.");
                _terminal_io->exibirMensagem("   - Solicitacao ID " + (s->get_carona() ? std::to_string(s->get_carona()->get_id()) : "N/A") + " para " + (s->get_carona() ? util.zonaToString(s->get_carona()->get_destino_zona()) : "N/A") + " em " + (s->get_carona() ? s->get_carona()->get_data_hora().substr(0,10) : "N/A") + " foi cancelada.");
            }
        }
        salvarDadosSolicitacoesPublico();
    }

    const std::vector<Solicitacao*>& GerenciadorSolicitacoes::getSolicitacoes() const {
        return _solicitacoes;
    }
}