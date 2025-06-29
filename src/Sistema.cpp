#include "Sistema.hpp"
#include "GerenciadorUsuarios.hpp"
#include "GerenciadorCaronas.hpp"
#include "GerenciadorSolicitacoes.hpp"
#include "GerenciadorAvaliacoes.hpp"
#include "GerenciadorVeiculos.hpp"
#include "TerminalIO.hpp"
#include "Utilitarios.hpp" // Para buscarDadosUfmgPorCpf e funcoes de data/hora
#include "Excecoes.hpp" // Para AppExcecao, ComandoInvalidoException, AutenticacaoFalhouException
#include "Usuario.hpp" // Necessário para Usuario*
#include "Motorista.hpp" // Necessário para Motorista*
#include "Carona.hpp" // Para StatusCarona
#include "Zona.hpp" // Para Zona e UFMGPosicao

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits> // Para numeric_limits
#include <algorithm> // Para std::find
#include <chrono> // Para tempo
#include <iomanip> // Para put_time

namespace ufmg_carona {

    Sistema::Sistema() : _usuario_logado(nullptr) {
        _terminalIO = new TerminalIO();
        _gerenciadorUsuarios = new GerenciadorUsuarios(_terminalIO);
        _gerenciadorVeiculos = new GerenciadorVeiculos(_terminalIO, _gerenciadorUsuarios);
        // A ordem de inicialização importa aqui. _gerenciadorSolicitacoes deve ser inicializado antes de _gerenciadorCaronas
        // se _gerenciadorCaronas depender dele no construtor.
        // Vamos inicializar _gerenciadorSolicitacoes e _gerenciadorAvaliacoes primeiro.
        _gerenciadorSolicitacoes = new GerenciadorSolicitacoes(_terminalIO, _gerenciadorUsuarios, _gerenciadorCaronas); // caronas ainda n inicializado, precisa de forward declaration ou mudar ordem
        _gerenciadorCaronas = new GerenciadorCaronas(_terminalIO, _gerenciadorUsuarios, _gerenciadorVeiculos, _gerenciadorSolicitacoes); // <--- CONSTRUTOR ATUALIZADO COM _gerenciadorSolicitacoes
        _gerenciadorAvaliacoes = new GerenciadorAvaliacoes(_terminalIO, _gerenciadorUsuarios, _gerenciadorCaronas, _gerenciadorSolicitacoes);

        carregarTodosDados();
    }

    Sistema::~Sistema() {
        _terminalIO->exibirMensagem("\nFinalizando o sistema...");
        salvarTodosDados();
        
        delete _gerenciadorAvaliacoes;
        delete _gerenciadorSolicitacoes;
        delete _gerenciadorCaronas;
        delete _gerenciadorVeiculos;
        delete _gerenciadorUsuarios;
        delete _terminalIO;
    }

    void Sistema::executar() {
        _terminalIO->exibirMensagem("== Sistema de Caronas UFMG iniciado ==");
        std::string comando_str;

        while (true) {
            if (!_usuario_logado) {
                _terminalIO->exibirMenuInicialNaoLogado();
            } else {
                _terminalIO->exibirMenuLogado(_usuario_logado->get_nome());
            }
            
            comando_str = _terminalIO->coletarStringInput("> ");

            if (comando_str.empty()) {
                continue;
            }

            try {
                int comando_int = -1;
                bool is_numeric = true;

                try {
                    comando_int = std::stoi(comando_str);
                } catch (const std::invalid_argument&) {
                    is_numeric = false;
                }

                if (!_usuario_logado) {
                    if (is_numeric) {
                        if (comando_int == 1) { 
                            fluxo_cadastro();
                        } else if (comando_int == 2) { 
                            fluxo_login();
                        } else if (comando_int == 0) { 
                            _terminalIO->exibirMensagem("Saindo do programa...");
                            break;
                        } else {
                            throw ComandoInvalidoException(comando_str);
                        }
                    } else if (comando_str == "cadastro") {
                        fluxo_cadastro();
                    } else if (comando_str == "login") {
                        fluxo_login();
                    } else if (comando_str == "sair") {
                        _terminalIO->exibirMensagem("Saindo do programa...");
                        break;
                    }
                    else {
                        throw ComandoInvalidoException(comando_str);
                    }
                } else {
                    processarComandoLogado(comando_str); // <--- CORREÇÃO: "processarComandoLogado"
                }
            } catch (const ComandoInvalidoException& e) {
                _terminalIO->exibirErro(e.what());
            } catch (const AppExcecao& e) {
                if (std::string(e.what()) == "Sair") {
                    break; 
                }
                _terminalIO->exibirErro(e.what());
            } catch (const std::bad_alloc& e) {
                _terminalIO->exibirErro("Falha na alocacao de memoria: " + std::string(e.what()));
            } catch (const std::exception& e) {
                _terminalIO->exibirErro("ERRO INESPERADO: " + std::string(e.what()));
            }
        }
    }

    // A função buscarDadosUfmgPorCpf permanece em Sistema e acessa _terminalIO
    std::tuple<bool, std::string, std::string, std::string, std::string> Sistema::buscarDadosUfmgPorCpf(const std::string& cpf_buscado) {
        std::ifstream arquivo_ufmg("dados_ufmg.txt");
        if (!arquivo_ufmg.is_open()) {
            _terminalIO->exibirErro("Nao foi possivel abrir o arquivo dados_ufmg.txt.");
            return std::make_tuple(false, "", "", "", "");
        }

        std::string linha;
        while (std::getline(arquivo_ufmg, linha)) {
            std::stringstream ss(linha);
            std::string nome_lido, cpf_lido, data_nascimento_lida, vinculo_lido, detalhe_lido;

            std::getline(ss, nome_lido, ';');
            std::getline(ss, cpf_lido, ';');
            std::getline(ss, data_nascimento_lida, ';');
            std::getline(ss, vinculo_lido, ';');
            std::getline(ss, detalhe_lido);

            if (cpf_lido == cpf_buscado) {
                arquivo_ufmg.close();
                return std::make_tuple(true, nome_lido, data_nascimento_lida, vinculo_lido, detalhe_lido);
            }
        }
        arquivo_ufmg.close();
        return std::make_tuple(false, "", "", "", "");
    }


    void Sistema::carregarTodosDados() {
        _terminalIO->exibirMensagem("Carregando dados iniciais...");
        _gerenciadorUsuarios->carregarDadosUsuariosPublico();
        _gerenciadorVeiculos->carregarDadosVeiculosPublico(_gerenciadorUsuarios);
        _gerenciadorCaronas->carregarDadosCaronasPublico(_gerenciadorUsuarios, _gerenciadorVeiculos);
        _gerenciadorSolicitacoes->carregarDadosSolicitacoesPublico(_gerenciadorUsuarios, _gerenciadorCaronas);
        _gerenciadorAvaliacoes->carregarDados();
        _gerenciadorCaronas->removerCaronasPassadas(); // Limpeza inicial de caronas passadas
        _terminalIO->exibirMensagem("Dados carregados.");
    }

    void Sistema::salvarTodosDados() {
        _terminalIO->exibirMensagem("Salvando dados...");
        _gerenciadorUsuarios->salvarDadosUsuariosPublico();
        _gerenciadorVeiculos->salvarDadosVeiculosPublico(_gerenciadorUsuarios);
        _gerenciadorCaronas->salvarDadosCaronasPublico();
        _gerenciadorSolicitacoes->salvarDadosSolicitacoesPublico();
        _gerenciadorAvaliacoes->salvarDados();
        _terminalIO->exibirMensagem("Dados salvos.");
    }

    void Sistema::processarComandoLogado(const std::string& comando_str) {
        int comando_int;
        try {
            comando_int = std::stoi(comando_str);
        } catch (const std::invalid_argument&) {
            throw ComandoInvalidoException(comando_str);
        }

        if (comando_int == 1) { 
            fluxo_editar_perfil_ou_veiculos();
        } else if (comando_int == 2) { 
            _gerenciadorSolicitacoes->fluxo_solicitar_carona(_usuario_logado);
        } else if (comando_int == 3) { 
            // Verifica se é motorista ou oferece para se tornar um
            if (!_usuario_logado->is_motorista()) {
                _terminalIO->exibirMensagem("Voce nao esta registrado como motorista.");
                char deseja_se_tornar = _terminalIO->confirmarAcao("Deseja se tornar um motorista? (s/n): ");
                if (deseja_se_tornar == 's' || deseja_se_tornar == 'S') {
                    _gerenciadorUsuarios->fluxo_tornar_motorista(_usuario_logado);
                }
                return;
            }
            Motorista* motorista_logado = dynamic_cast<Motorista*>(_usuario_logado);
            if (motorista_logado) {
                 _terminalIO->exibirMenuMotorista(); // Exibe o menu do motorista
                 int sub_comando = _terminalIO->coletarIntInput("> ", 0, 3);
                 if (sub_comando == 1) {
                     _gerenciadorCaronas->fluxo_oferecer_carona(_usuario_logado);
                 } else if (sub_comando == 2) {
                     _gerenciadorCaronas->fluxo_gerenciar_caronas(motorista_logado);
                 } else if (sub_comando == 3) {
                     _gerenciadorVeiculos->fluxo_cadastrar_veiculo(_usuario_logado);
                 } else if (sub_comando == 0) {
                     _terminalIO->exibirMensagem("Voltando ao menu principal...");
                 } else {
                     throw ComandoInvalidoException(std::to_string(sub_comando));
                 }
            } else {
                _terminalIO->exibirErro("Erro interno: Usuario deveria ser motorista mas o cast falhou.");
            }
        } else if (comando_int == 4) {
            _gerenciadorAvaliacoes->fluxo_avaliacoes(_usuario_logado);
        } else if (comando_int == 5) { 
            fluxo_logout();
        } else if (comando_int == 6) { 
            _terminalIO->exibirMensagem("Saindo do programa...");
            throw AppExcecao("Sair");
        } else {
            throw ComandoInvalidoException(comando_str);
        }
    }

    void Sistema::fluxo_cadastro() {
        std::string cpf_digitado;
        std::string nome_ufmg, data_nascimento_ufmg, vinculo_ufmg, detalhe_ufmg;

        _terminalIO->exibirMensagem("--- Cadastro ---");
        cpf_digitado = _terminalIO->coletarStringInput("CPF: "); // <--- CORREÇÃO: "_terminal_io"

        // Verifica se o CPF já está cadastrado no sistema antes de buscar na UFMG
        if (_gerenciadorUsuarios->buscarUsuarioPorCpf(cpf_digitado)) {
            throw AppExcecao("CPF ja cadastrado no sistema. Por favor, faca login.");
        }

        auto dados_ufmg = buscarDadosUfmgPorCpf(cpf_digitado); // Chama diretamente
        bool cpf_encontrado_ufmg = std::get<0>(dados_ufmg);

        if (!cpf_encontrado_ufmg) {
            _terminalIO->exibirMensagem("CPF nao encontrado no cadastro da UFMG. O cadastro nao pode ser realizado.");
            return;
        }

        nome_ufmg = std::get<1>(dados_ufmg);
        data_nascimento_ufmg = std::get<2>(dados_ufmg);
        vinculo_ufmg = std::get<3>(dados_ufmg);
        detalhe_ufmg = std::get<4>(dados_ufmg);

        if (nome_ufmg.empty() || data_nascimento_ufmg.empty() || vinculo_ufmg.empty()) {
            _terminalIO->exibirMensagem("As informacoes do seu CPF estao incompletas no cadastro da UFMG. Por favor, procure a secretaria da UFMG para regularizar seu cadastro.");
            return;
        }

        // Passa os dados brutos da UFMG para o GerenciadorUsuarios
        _gerenciadorUsuarios->cadastrarUsuario(cpf_digitado, nome_ufmg, data_nascimento_ufmg,
                                               vinculo_ufmg, detalhe_ufmg);
    }

    void Sistema::fluxo_login() {
        _usuario_logado = _gerenciadorUsuarios->fluxo_login();
    }

    void Sistema::fluxo_logout() {
        _gerenciadorUsuarios->logoutUsuario(_usuario_logado);
    }

    void Sistema::fluxo_editar_perfil_ou_veiculos() {
        int sub_comando_perfil;
        do {
            _terminalIO->exibirMenuPerfil(); // Exibe o menu de perfil (novo)
            sub_comando_perfil = _terminalIO->coletarIntInput("> ", 0, 2);

            try {
                if (sub_comando_perfil == 1) {
                    _gerenciadorUsuarios->fluxo_editar_perfil(_usuario_logado);
                } else if (sub_comando_perfil == 2) {
                    if (_usuario_logado->is_motorista()) {
                        Motorista* motorista_logado = dynamic_cast<Motorista*>(_usuario_logado);
                        if (motorista_logado) {
                            _gerenciadorVeiculos->fluxo_gerenciar_veiculos(motorista_logado);
                        } else {
                            _terminalIO->exibirErro("Erro interno: Usuario deveria ser motorista mas o cast falhou.");
                        }
                    } else {
                        _terminalIO->exibirMensagem("Voce nao e motorista para gerenciar veiculos.");
                    }
                }
            } catch (const AppExcecao& e) {
                _terminalIO->exibirErro(e.what());
            }
        } while (sub_comando_perfil != 0);
    }

} // namespace ufmg_carona