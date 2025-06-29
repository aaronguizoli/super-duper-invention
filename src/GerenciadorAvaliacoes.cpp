#include "GerenciadorAvaliacoes.hpp"
#include "TerminalIO.hpp"
#include "GerenciadorUsuarios.hpp"
#include "GerenciadorCaronas.hpp"
#include "GerenciadorSolicitacoes.hpp"
#include "Usuario.hpp"
#include "Motorista.hpp"
#include "Avaliacao.hpp"
#include "Carona.hpp"
#include "Solicitacao.hpp"
#include "Excecoes.hpp" // Para AppExcecao, se necessário

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm> // Para std::find

namespace ufmg_carona {

    GerenciadorAvaliacoes::GerenciadorAvaliacoes(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios,
                                                 GerenciadorCaronas* ger_caronas, GerenciadorSolicitacoes* ger_solicitacoes)
        : _terminal_io(terminal_io), _ger_usuarios(ger_usuarios), _ger_caronas(ger_caronas),
          _ger_solicitacoes(ger_solicitacoes) {
        // As avaliações serão carregadas pelo Sistema no início, então não carregamos aqui
    }

    GerenciadorAvaliacoes::~GerenciadorAvaliacoes() {
        for (Avaliacao* aval : _avaliacoes_globais) {
            delete aval;
        }
        _avaliacoes_globais.clear();
    }

    // <--- ALTERADO: Métodos carregarDados e salvarDados são agora públicos
    void GerenciadorAvaliacoes::carregarDados() {
        std::ifstream arquivo_avaliacoes("avaliacoes.txt");
        if (!arquivo_avaliacoes.is_open()) {
            _terminal_io->exibirAviso("Arquivo avaliacoes.txt nao encontrado. Nao ha avaliacoes para carregar.");
            return;
        }

        std::string linha, avaliador_cpf, avaliado_cpf, carona_id_str, nota_str, comentario;
        while (std::getline(arquivo_avaliacoes, linha)) {
            std::stringstream ss(linha);
            std::getline(ss, avaliador_cpf, ';');
            std::getline(ss, avaliado_cpf, ';');
            std::getline(ss, carona_id_str, ';');
            std::getline(ss, nota_str, ';');
            std::getline(ss, comentario);

            int carona_id;
            int nota;
            try {
                carona_id = std::stoi(carona_id_str);
                nota = std::stoi(nota_str);
            } catch (const std::invalid_argument&) {
                _terminal_io->exibirErro("ERRO ao carregar avaliacao: ID de carona ou nota invalida.");
                continue;
            }

            Usuario* avaliador_ptr = _ger_usuarios->buscarUsuarioPorCpf(avaliador_cpf);
            Usuario* avaliado_ptr = _ger_usuarios->buscarUsuarioPorCpf(avaliado_cpf);
            Carona* carona_ref_ptr = _ger_caronas->buscarCaronaPorId(carona_id);

            if (avaliador_ptr && avaliado_ptr && carona_ref_ptr) {
                Avaliacao* nova_avaliacao = new Avaliacao(nota, comentario, avaliador_ptr, avaliado_ptr, carona_ref_ptr);
                _avaliacoes_globais.push_back(nova_avaliacao);
                avaliado_ptr->adicionar_avaliacao_recebida(nova_avaliacao);
            } else {
                _terminal_io->exibirAviso("Avaliacao com referencias invalidas (avaliador, avaliado ou carona). Ignorada.");
            }
        }
        arquivo_avaliacoes.close();
    }

    // <--- ALTERADO: Métodos carregarDados e salvarDados são agora públicos
    void GerenciadorAvaliacoes::salvarDados() {
        std::ofstream arquivo_avaliacoes("avaliacoes.txt", std::ios::trunc);
        if (!arquivo_avaliacoes.is_open()) {
            _terminal_io->exibirErro("Nao foi possivel abrir o arquivo avaliacoes.txt para salvar dados.");
            return;
        }

        for (const auto& aval : _avaliacoes_globais) {
            if (aval && aval->get_avaliador() && aval->get_avaliado() && aval->get_carona_referencia()) {
                arquivo_avaliacoes << aval->get_avaliador()->get_cpf() << ";"
                                   << aval->get_avaliado()->get_cpf() << ";"
                                   << aval->get_carona_referencia()->get_id() << ";"
                                   << aval->get_nota() << ";"
                                   << aval->get_comentario()
                                   << std::endl;
            }
        }
        arquivo_avaliacoes.close();
    }

    void GerenciadorAvaliacoes::fluxo_avaliacoes(Usuario* usuario_logado) {
        int comando;
        do {
            _terminal_io->exibirMensagem("\n--- Menu Avaliacoes ---");
            _terminal_io->exibirMensagem("(1) Avaliar Caronas (Passageiro) | (2) Avaliar Passageiros (Motorista)");
            _terminal_io->exibirMensagem("(3) Ver Minhas Avaliacoes Recebidas | (4) Ver Avaliacoes que Fiz | (0) Voltar");
            comando = _terminal_io->coletarIntInput("> ", 0, 4);

            try {
                if (comando == 1) {
                    avaliarCaronaPassageiro(usuario_logado);
                } else if (comando == 2) {
                    // Downcast para Motorista é seguro aqui pois a função verificar_motorista é chamada antes.
                    Motorista* motorista_logado = dynamic_cast<Motorista*>(usuario_logado);
                    if (motorista_logado) {
                        avaliarPassageirosMotorista(motorista_logado);
                    } else {
                        _terminal_io->exibirAviso("Voce precisa ser motorista para avaliar passageiros.");
                    }
                } else if (comando == 3) {
                    exibirMinhasAvaliacoesRecebidas(usuario_logado);
                } else if (comando == 4) {
                    exibirAvaliacoesQueFiz(usuario_logado);
                }
            } catch (const AppExcecao& e) {
                _terminal_io->exibirErro(e.what());
            }
        } while (comando != 0);
    }

    void GerenciadorAvaliacoes::avaliarCaronaPassageiro(Usuario* passageiro) {
        _terminal_io->exibirMensagem("\n--- Avaliar Caronas (Como Passageiro) ---");
        std::vector<Solicitacao*> solicitacoes_para_avaliar;

        // O GerenciadorAvaliacoes agora precisa acessar as _solicitacoes globais para verificar se há solicitações para avaliar
        const std::vector<Solicitacao*>& todas_solicitacoes = _ger_solicitacoes->getSolicitacoes();

        for (Solicitacao* s : todas_solicitacoes) {
            if (s->get_passageiro() == passageiro &&
                s->get_status() == StatusSolicitacao::ACEITA &&
                s->get_carona() &&
                s->get_carona()->get_status() == StatusCarona::FINALIZADA &&
                !s->get_passageiro_avaliou_motorista()) {
                solicitacoes_para_avaliar.push_back(s);
            }
        }

        if (solicitacoes_para_avaliar.empty()) {
            _terminal_io->exibirMensagem("Nenhuma carona finalizada para voce avaliar como passageiro.");
            return;
        }

        _terminal_io->exibirMensagem("Caronas pendentes de sua avaliacao:");
        for (size_t i = 0; i < solicitacoes_para_avaliar.size(); ++i) {
            Solicitacao* s = solicitacoes_para_avaliar[i];
            _terminal_io->exibirMensagem("[" + std::to_string(i+1) + "] Carona ID: " + std::to_string(s->get_carona()->get_id()) +
                                       " | Motorista: " + s->get_carona()->get_motorista()->get_nome() +
                                       " | Data: " + s->get_carona()->get_data_hora());
        }

        int escolha = _terminal_io->coletarIntInput("Escolha a carona para avaliar (0 para voltar): ", 0, solicitacoes_para_avaliar.size());
        if (escolha == 0) return;

        Solicitacao* solicitacao_escolhida = solicitacoes_para_avaliar[escolha - 1];
        Carona* carona_alvo = solicitacao_escolhida->get_carona();
        Usuario* motorista_alvo = carona_alvo->get_motorista();

        _terminal_io->exibirMensagem("\nAvaliando carona ID " + std::to_string(carona_alvo->get_id()) + " do motorista " + motorista_alvo->get_nome());
        int nota = _terminal_io->coletarIntInput("Nota (1-5): ", 1, 5);
        std::string comentario = _terminal_io->coletarStringInput("Comentario (ate 100 caracteres): ");
        if (comentario.length() > 100) {
            comentario = comentario.substr(0, 100);
            _terminal_io->exibirAviso("Comentario truncado para 100 caracteres.");
        }

        Avaliacao* nova_avaliacao = new Avaliacao(nota, comentario, passageiro, motorista_alvo, carona_alvo);
        _avaliacoes_globais.push_back(nova_avaliacao);
        motorista_alvo->adicionar_avaliacao_recebida(nova_avaliacao);
        solicitacao_escolhida->set_passageiro_avaliou_motorista(true);

        salvarDados(); // Salva as avaliações
        _ger_solicitacoes->salvarDadosSolicitacoesPublico(); // Salva o status da solicitação
        _ger_usuarios->salvarDadosUsuariosPublico(); // Salva o novo rating do motorista
        _terminal_io->exibirMensagem("Avaliacao registrada com sucesso!");
    }

    void GerenciadorAvaliacoes::avaliarPassageirosMotorista(Motorista* motorista) {
        if (!motorista->is_motorista()) {
            _terminal_io->exibirAviso("Voce precisa ser motorista para avaliar passageiros.");
            return;
        }
        _terminal_io->exibirMensagem("\n--- Avaliar Passageiros (Como Motorista) ---");
        std::vector<Solicitacao*> solicitacoes_para_avaliar;

        // O GerenciadorAvaliacoes agora precisa acessar as _solicitacoes globais para verificar se há solicitações para avaliar
        const std::vector<Solicitacao*>& todas_solicitacoes = _ger_solicitacoes->getSolicitacoes();

        for (Solicitacao* s : todas_solicitacoes) {
            if (s->get_carona() && s->get_carona()->get_motorista() == motorista &&
                s->get_status() == StatusSolicitacao::ACEITA &&
                s->get_carona()->get_status() == StatusCarona::FINALIZADA &&
                !s->get_motorista_avaliou_passageiro()) {
                solicitacoes_para_avaliar.push_back(s);
            }
        }

        if (solicitacoes_para_avaliar.empty()) {
            _terminal_io->exibirMensagem("Nenhum passageiro para voce avaliar em caronas finalizadas.");
            return;
        }

        _terminal_io->exibirMensagem("Passageiros pendentes de sua avaliacao:");
        for (size_t i = 0; i < solicitacoes_para_avaliar.size(); ++i) {
            Solicitacao* s = solicitacoes_para_avaliar[i];
            _terminal_io->exibirMensagem("[" + std::to_string(i+1) + "] Passageiro: " + s->get_passageiro()->get_nome() +
                                       " | Carona ID: " + std::to_string(s->get_carona()->get_id()) +
                                       " | Data: " + s->get_carona()->get_data_hora());
        }

        int escolha = _terminal_io->coletarIntInput("Escolha o passageiro para avaliar (0 para voltar): ", 0, solicitacoes_para_avaliar.size());
        if (escolha == 0) return;

        Solicitacao* solicitacao_escolhida = solicitacoes_para_avaliar[escolha - 1];
        Carona* carona_alvo = solicitacao_escolhida->get_carona();
        Usuario* passageiro_alvo = solicitacao_escolhida->get_passageiro();

        _terminal_io->exibirMensagem("\nAvaliando passageiro " + passageiro_alvo->get_nome() + " da carona ID " + std::to_string(carona_alvo->get_id()));
        int nota = _terminal_io->coletarIntInput("Nota (1-5): ", 1, 5);
        std::string comentario = _terminal_io->coletarStringInput("Comentario (ate 100 caracteres): ");
        if (comentario.length() > 100) {
            comentario = comentario.substr(0, 100);
            _terminal_io->exibirAviso("Comentario truncado para 100 caracteres.");
        }

        Avaliacao* nova_avaliacao = new Avaliacao(nota, comentario, motorista, passageiro_alvo, carona_alvo);
        _avaliacoes_globais.push_back(nova_avaliacao);
        passageiro_alvo->adicionar_avaliacao_recebida(nova_avaliacao);
        solicitacao_escolhida->set_motorista_avaliou_passageiro(true);

        salvarDados(); // Salva as avaliações
        _ger_solicitacoes->salvarDadosSolicitacoesPublico(); // Salva o status da solicitação
        _ger_usuarios->salvarDadosUsuariosPublico(); // Salva o novo rating do passageiro
        _terminal_io->exibirMensagem("Avaliacao registrada com sucesso!");
    }

    void GerenciadorAvaliacoes::exibirMinhasAvaliacoesRecebidas(Usuario* usuario) const {
        _terminal_io->exibirMensagem("\n--- Minhas Avaliacoes Recebidas ---");
        usuario->imprimir_perfil(); // Isso está em Usuario, então permanece lá.
        
        bool encontrou_avaliacoes = false;
        for (const auto& aval : _avaliacoes_globais) {
            if (aval->get_avaliado() == usuario) {
                encontrou_avaliacoes = true;
                _terminal_io->exibirMensagem("  - De: " + aval->get_avaliador()->get_nome() +
                                           " | Carona ID: " + std::to_string(aval->get_carona_referencia()->get_id()) +
                                           " | Nota: " + std::to_string(aval->get_nota()));
                if (!aval->get_comentario().empty()) {
                    _terminal_io->exibirMensagem("    Comentario: \"" + aval->get_comentario() + "\"");
                }
            }
        }
        if (!encontrou_avaliacoes) {
            _terminal_io->exibirMensagem("Voce ainda nao recebeu avaliacoes.");
        }
    }

    void GerenciadorAvaliacoes::exibirAvaliacoesQueFiz(Usuario* usuario) const {
        _terminal_io->exibirMensagem("\n--- Avaliacoes que Fiz ---");
        bool encontrou_avaliacoes = false;
        for (const auto& aval : _avaliacoes_globais) {
            if (aval->get_avaliador() == usuario) {
                encontrou_avaliacoes = true;
                _terminal_io->exibirMensagem("  - Para: " + aval->get_avaliado()->get_nome() +
                                           " | Carona ID: " + std::to_string(aval->get_carona_referencia()->get_id()) +
                                           " | Nota: " + std::to_string(aval->get_nota()));
                if (!aval->get_comentario().empty()) {
                    _terminal_io->exibirMensagem("    Comentario: \"" + aval->get_comentario() + "\"");
                }
            }
        }
        if (!encontrou_avaliacoes) {
            _terminal_io->exibirMensagem("Voce ainda nao fez nenhuma avaliacao.");
        }
    }

} // namespace ufmg_carona