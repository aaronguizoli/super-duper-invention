#include "Sistema.hpp"
#include "GerenciadorUsuarios.hpp"
#include "GerenciadorCaronas.hpp"
#include "GerenciadorSolicitacoes.hpp"
#include "GerenciadorAvaliacoes.hpp"
#include "GerenciadorVeiculos.hpp"
#include "TerminalIO.hpp"
#include "Utilitarios.hpp"
#include "Excecoes.hpp"
#include "Usuario.hpp"
#include "Motorista.hpp"
#include "Carona.hpp"
#include "Zona.hpp"
#include "Genero.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include <unistd.h>

namespace ufmg_carona {
    // --- Funções de Validação Auxiliares ---
    // NOTA: Para que a validação funcione, mova estas funções para um local
    // acessível por GerenciadorUsuarios.cpp (pode ser aqui, em um Utilitarios.hpp, ou
    // como métodos privados de GerenciadorUsuarios) e use-as dentro do método
    // GerenciadorUsuarios::cadastrarUsuario.
    bool isTelefoneValido(const std::string& telefone) {
        if (telefone.length() < 10 || telefone.length() > 11) {
            return false;
        }
        for (char const &c : telefone) {
            if (std::isdigit(c) == 0) return false;
        }
        return true;
    }

    bool isEmailValido(const std::string& email) {
        const size_t at_pos = email.find('@');
        const size_t dot_pos = email.rfind('.');
        return (at_pos != std::string::npos && dot_pos != std::string::npos && at_pos < dot_pos);
    }

    bool isSenhaValida(const std::string& senha) {
        return senha.length() >= 6;
    }


    Sistema::Sistema() : _usuario_logado(nullptr) {
        _terminalIO = new TerminalIO();
        _gerenciadorUsuarios = new GerenciadorUsuarios(_terminalIO);
        _gerenciadorVeiculos = new GerenciadorVeiculos(_terminalIO, _gerenciadorUsuarios);
        _gerenciadorCaronas = new GerenciadorCaronas(_terminalIO, _gerenciadorUsuarios, _gerenciadorVeiculos, nullptr);
        _gerenciadorSolicitacoes = new GerenciadorSolicitacoes(_terminalIO, _gerenciadorUsuarios, nullptr);
        _gerenciadorCaronas->setGerenciadorSolicitacoes(_gerenciadorSolicitacoes);
        _gerenciadorSolicitacoes->setGerenciadorCaronas(_gerenciadorCaronas);
        _gerenciadorAvaliacoes = new GerenciadorAvaliacoes(_terminalIO, _gerenciadorUsuarios, _gerenciadorCaronas, _gerenciadorSolicitacoes);
        carregarTodosDados();
    }

    Sistema::~Sistema() {
        if (_terminalIO) _terminalIO->exibirMensagem("\nFinalizando o sistema...");
        salvarTodosDados();
        delete _gerenciadorAvaliacoes;
        delete _gerenciadorSolicitacoes;
        delete _gerenciadorCaronas;
        delete _gerenciadorVeiculos;
        delete _gerenciadorUsuarios;
        delete _terminalIO;
    }

    void Sistema::executar() {
        // --- SOLUÇÃO GLOBAL PARA CAMINHOS DE ARQUIVO ---
        #ifndef PROJECT_SOURCE_DIR
            #define PROJECT_SOURCE_DIR "." 
        #endif
        std::string base_path = std::string(PROJECT_SOURCE_DIR);
        if (chdir(base_path.c_str()) != 0) {
            _terminalIO->exibirErro("ERRO FATAL: Nao foi possivel acessar o diretorio do projeto: " + base_path);
            _terminalIO->exibirErro("Verifique as permissoes da pasta e tente novamente.");
            return; 
        }
        // --- FIM DA SOLUÇÃO ---

        _terminalIO->exibirMensagem("== Sistema de Caronas UFMG iniciado ==");
        std::string comando_str;

        while (true) {
            if (!_usuario_logado) {
                _terminalIO->exibirMenuInicialNaoLogado();
            } else {
                _terminalIO->exibirMenuLogado(_usuario_logado->get_nome());
            }
            
            comando_str = _terminalIO->coletarStringInput("> ");
            if (comando_str.empty()) continue;

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
                        if (comando_int == 1) fluxo_cadastro();
                        else if (comando_int == 2) fluxo_login();
                        else if (comando_int == 0) {
                            _terminalIO->exibirMensagem("Saindo do programa...");
                            break;
                        } else throw ComandoInvalidoException(comando_str);
                    } else if (comando_str == "cadastro") fluxo_cadastro();
                    else if (comando_str == "login") fluxo_login();
                    else if (comando_str == "sair") {
                        _terminalIO->exibirMensagem("Saindo do programa...");
                        break;
                    } else throw ComandoInvalidoException(comando_str);
                } else {
                    processarComandoLogado(comando_str);
                }
            } catch (const ComandoInvalidoException& e) {
                _terminalIO->exibirErro(e.what());
            } catch (const AppExcecao& e) {
                if (std::string(e.what()) == "Sair") break;
                _terminalIO->exibirErro(e.what());
            } catch (const std::bad_alloc& e) {
                _terminalIO->exibirErro("Falha na alocacao de memoria: " + std::string(e.what()));
            } catch (const std::exception& e) {
                _terminalIO->exibirErro("ERRO INESPERADO: " + std::string(e.what()));
            }
        }
    }

    std::tuple<bool, std::string, std::string, std::string, std::string> Sistema::buscarDadosUfmgPorCpf(const std::string& cpf_buscado) {
        const std::string nome_arquivo = "dados_ufmg.txt";
        std::ifstream arquivo_ufmg(nome_arquivo);
        
        if (!arquivo_ufmg.is_open()) {
            _terminalIO->exibirErro("ERRO: Nao foi possivel abrir o arquivo " + nome_arquivo);
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
        if (!_terminalIO) { std::cerr << "ERRO FATAL: _terminalIO e nulo" << std::endl; return; }
        _terminalIO->exibirMensagem("Carregando dados iniciais...");
        if (_gerenciadorUsuarios) _gerenciadorUsuarios->carregarDadosUsuariosPublico();
        if (_gerenciadorVeiculos) _gerenciadorVeiculos->carregarDadosVeiculosPublico(_gerenciadorUsuarios);
        if (_gerenciadorCaronas) _gerenciadorCaronas->carregarDadosCaronasPublico(_gerenciadorUsuarios, _gerenciadorVeiculos);
        if (_gerenciadorSolicitacoes) _gerenciadorSolicitacoes->carregarDadosSolicitacoesPublico(_gerenciadorUsuarios, _gerenciadorCaronas);
        if (_gerenciadorAvaliacoes) _gerenciadorAvaliacoes->carregarDados();
        if (_gerenciadorCaronas) _gerenciadorCaronas->removerCaronasPassadas();
        _terminalIO->exibirMensagem("Dados carregados.");
    }

    void Sistema::salvarTodosDados() {
        if (!_terminalIO) { std::cerr << "ERRO FATAL: _terminalIO e nulo" << std::endl; return; }
        _terminalIO->exibirMensagem("Salvando dados...");
        if (_gerenciadorUsuarios) _gerenciadorUsuarios->salvarDadosUsuariosPublico();
        if (_gerenciadorVeiculos) _gerenciadorVeiculos->salvarDadosVeiculosPublico(_gerenciadorUsuarios);
        if (_gerenciadorCaronas) _gerenciadorCaronas->salvarDadosCaronasPublico();
        if (_gerenciadorSolicitacoes) _gerenciadorSolicitacoes->salvarDadosSolicitacoesPublico();
        if (_gerenciadorAvaliacoes) _gerenciadorAvaliacoes->salvarDados();
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
            if (!_gerenciadorSolicitacoes) { _terminalIO->exibirErro("Erro: Gerenciador de solicitacoes e nulo."); return; }
            _gerenciadorSolicitacoes->fluxo_solicitar_carona(_usuario_logado);
        } else if (comando_int == 3) {
            if (!_usuario_logado->is_motorista()) {
                _terminalIO->exibirMensagem("Voce nao esta registrado como motorista.");
                if (_terminalIO->confirmarAcao("Deseja se tornar um motorista? (s/n): ") == 's') {
                    if (_gerenciadorUsuarios) _gerenciadorUsuarios->fluxo_tornar_motorista(_usuario_logado);
                }
                return;
            }
            Motorista* motorista_logado = dynamic_cast<Motorista*>(_usuario_logado);
            if (motorista_logado) {
                 _terminalIO->exibirMenuMotorista();
                 int sub_comando = _terminalIO->coletarIntInput("> ", 0, 3);
                 if (sub_comando == 1) { if (_gerenciadorCaronas) _gerenciadorCaronas->fluxo_oferecer_carona(_usuario_logado); }
                 else if (sub_comando == 2) { if (_gerenciadorCaronas) _gerenciadorCaronas->fluxo_gerenciar_caronas(motorista_logado); }
                 else if (sub_comando == 3) { if (_gerenciadorVeiculos) _gerenciadorVeiculos->fluxo_cadastrar_veiculo(_usuario_logado); }
                 else if (sub_comando == 0) _terminalIO->exibirMensagem("Voltando ao menu principal...");
                 else throw ComandoInvalidoException(std::to_string(sub_comando));
            } else _terminalIO->exibirErro("Erro interno: Usuario deveria ser motorista mas o cast falhou.");
        } else if (comando_int == 4) {
            if (_gerenciadorAvaliacoes) _gerenciadorAvaliacoes->fluxo_avaliacoes(_usuario_logado);
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
        _terminalIO->exibirMensagem("--- Cadastro ---");
        std::string cpf_digitado = _terminalIO->coletarStringInput("CPF: ");
        if (_gerenciadorUsuarios && _gerenciadorUsuarios->buscarUsuarioPorCpf(cpf_digitado)) {
            throw AppExcecao("CPF ja cadastrado no sistema. Por favor, faca login.");
        }

        auto dados_ufmg = buscarDadosUfmgPorCpf(cpf_digitado);
        if (!std::get<0>(dados_ufmg)) {
            _terminalIO->exibirMensagem("CPF nao encontrado no cadastro da UFMG. O cadastro nao pode ser realizado.");
            return;
        }

        std::string nome_ufmg = std::get<1>(dados_ufmg);
        std::string data_nascimento_ufmg = std::get<2>(dados_ufmg);
        std::string vinculo_ufmg = std::get<3>(dados_ufmg);
        std::string detalhe_ufmg = std::get<4>(dados_ufmg);

        if (nome_ufmg.empty() || data_nascimento_ufmg.empty() || vinculo_ufmg.empty()) {
            _terminalIO->exibirMensagem("As informacoes do seu CPF estao incompletas no cadastro da UFMG. Por favor, procure a secretaria da UFMG para regularizar seu cadastro.");
            return;
        }

        // CORREÇÃO: A responsabilidade de coletar dados adicionais (telefone, email),
        // validar e criar o usuário é delegada ao GerenciadorUsuarios.
        // Isso corrige o erro de compilação.
        if (_gerenciadorUsuarios) {
            // Esta chamada espera que GerenciadorUsuarios::cadastrarUsuario peça os dados
            // adicionais (telefone, email, etc) e faça a validação internamente.
            _gerenciadorUsuarios->cadastrarUsuario(cpf_digitado, nome_ufmg, data_nascimento_ufmg, vinculo_ufmg, detalhe_ufmg);
        } else {
            _terminalIO->exibirErro("ERRO FATAL: Gerenciador de Usuarios nao inicializado.");
        }
    }

    void Sistema::fluxo_login() {
        if (_gerenciadorUsuarios) _usuario_logado = _gerenciadorUsuarios->fluxo_login();
    }

    void Sistema::fluxo_logout() {
        if (_gerenciadorUsuarios) _gerenciadorUsuarios->logoutUsuario(_usuario_logado);
    }

    void Sistema::fluxo_editar_perfil_ou_veiculos() {
        int sub_comando_perfil;
        do {
            _terminalIO->exibirMenuPerfil();
            sub_comando_perfil = _terminalIO->coletarIntInput("> ", 0, 2);
            try {
                if (sub_comando_perfil == 1) {
                    if (_gerenciadorUsuarios) {
                        _gerenciadorUsuarios->fluxo_editar_perfil(_usuario_logado);
                    }
                } else if (sub_comando_perfil == 2) {
                    if (_usuario_logado->is_motorista()) {
                        Motorista* motorista_logado = dynamic_cast<Motorista*>(_usuario_logado);
                        if (motorista_logado) {
                            if (_gerenciadorVeiculos) {
                                _gerenciadorVeiculos->fluxo_gerenciar_veiculos(motorista_logado);
                            }
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
}
