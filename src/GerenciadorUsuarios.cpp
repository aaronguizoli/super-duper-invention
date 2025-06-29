#include "GerenciadorUsuarios.hpp"
#include "TerminalIO.hpp"
#include "Usuario.hpp"
#include "Motorista.hpp"
#include "Genero.hpp"
#include "Excecoes.hpp" // Para AutenticacaoFalhouException, AppExcecao

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits> // Para numeric_limits
#include <algorithm> // Para std::find

namespace ufmg_carona {

    GerenciadorUsuarios::GerenciadorUsuarios(TerminalIO* terminal_io)
        : _terminal_io(terminal_io) {
        // Os usuários serão carregados pelo Sistema no início
    }

    GerenciadorUsuarios::~GerenciadorUsuarios() {
        for (Usuario* u : _usuarios) {
            delete u;
        }
        _usuarios.clear();
    }

    void GerenciadorUsuarios::carregarDados() {
        // Este método é o interno, que pode ser chamado pelo método público.
    }

    void GerenciadorUsuarios::carregarDadosUsuariosPublico() {
        std::ifstream arquivo_usuarios("usuarios.txt");
        if (arquivo_usuarios.is_open()) {
            std::string linha;
            while (std::getline(arquivo_usuarios, linha)) {
                std::stringstream ss_linha(linha);
                std::string token;
                std::vector<std::string> campos;

                while (std::getline(ss_linha, token, ';')) {
                    campos.push_back(token);
                }

                if (campos.size() < 10) {
                    _terminal_io->exibirAviso("AVISO: Linha com formato invalido no usuarios.txt (menos de 10 campos): " + linha);
                    continue;
                }

                std::string cpf = campos[0];
                std::string nome = campos[1];
                std::string telefone = campos[2];
                std::string data_nascimento = campos[3];
                std::string email = campos[4];
                std::string senha = campos[5];
                std::string gen_str = campos[6];
                std::string vinculo_tipo = campos[7];
                std::string detalhe_vinculo = campos[8];
                std::string is_motorista_str = campos[9];
                
                std::string cnh_numero_lida;
                if (campos.size() >= 11) {
                    cnh_numero_lida = campos[10];
                }

                Genero gen;
                try {
                    gen = static_cast<Genero>(std::stoi(gen_str));
                } catch (const std::invalid_argument& e) {
                    _terminal_io->exibirErro("ERRO: Valor invalido para 'genero' no usuarios.txt para CPF " + cpf + ". Ignorando usuario. Erro: " + e.what());
                    continue;
                } catch (const std::out_of_range& e) {
                    _terminal_io->exibirErro("ERRO: Valor de 'genero' fora do limite para CPF " + cpf + " no usuarios.txt. Ignorando usuario. Erro: " + e.what());
                    continue;
                }

                bool eh_motorista_do_arquivo;
                try {
                    eh_motorista_do_arquivo = (std::stoi(is_motorista_str) == 1);
                } catch (const std::invalid_argument& e) {
                    _terminal_io->exibirErro("ERRO: Valor invalido para 'eh_motorista' no usuarios.txt para CPF " + cpf + ". Ignorando usuario. Erro: " + e.what());
                    continue;
                } catch (const std::out_of_range& e) {
                    _terminal_io->exibirErro("ERRO: Valor de 'eh_motorista' fora do limite para CPF " + cpf + " no usuarios.txt. Ignorando usuario. Erro: " + e.what());
                    continue;
                }

                Usuario* novo_usuario_carregado = nullptr;

                if (eh_motorista_do_arquivo) {
                    if (!cnh_numero_lida.empty() && cnh_numero_lida != "0") {
                        novo_usuario_carregado = new Motorista(nome, cpf, telefone, data_nascimento, email, senha, gen,
                                                             vinculo_tipo, detalhe_vinculo, cnh_numero_lida);
                    } else {
                        _terminal_io->exibirAviso("AVISO: Motorista com CPF " + cpf + " sem CNH valida no usuarios.txt. Carregado como Usuario comum.");
                        novo_usuario_carregado = new Usuario(nome, cpf, telefone, data_nascimento, email, senha, gen,
                                                             vinculo_tipo, detalhe_vinculo);
                    }
                } else {
                    novo_usuario_carregado = new Usuario(nome, cpf, telefone, data_nascimento, email, senha, gen,
                                                         vinculo_tipo, detalhe_vinculo);
                }
                
                if (novo_usuario_carregado) {
                    _usuarios.push_back(novo_usuario_carregado);
                }
            }
            arquivo_usuarios.close();
        } else {
            _terminal_io->exibirAviso("AVISO: Arquivo usuarios.txt nao encontrado. Nao ha usuarios para carregar.");
        }
    }

    void GerenciadorUsuarios::salvarDados() {
        // Este método é o interno, que pode ser chamado pelo método público.
    }

    void GerenciadorUsuarios::salvarDadosUsuariosPublico() {
        std::ofstream arquivo_usuarios("usuarios.txt", std::ios::trunc);
        if (!arquivo_usuarios.is_open()) {
            _terminal_io->exibirErro("Nao foi possivel abrir o arquivo usuarios.txt para salvar dados.");
            return;
        }

        for (const auto& u : _usuarios) {
            arquivo_usuarios << u->get_cpf() << ";"
                             << u->get_nome() << ";"
                             << u->get_telefone() << ";"
                             << u->get_data_nascimento() << ";"
                             << u->get_email() << ";"
                             << u->get_senha() << ";"
                             << static_cast<int>(u->get_genero()) << ";"
                             << u->get_vinculo_raw() << ";"
                             << u->get_detalhe_vinculo() << ";";
            
            if (u->is_motorista()) {
                Motorista* m_ptr = dynamic_cast<Motorista*>(u);
                if (m_ptr) {
                    arquivo_usuarios << "1;"
                                     << m_ptr->get_cnh_numero();
                } else {
                    arquivo_usuarios << "0";
                    _terminal_io->exibirAviso("AVISO: Usuario " + u->get_cpf() + " indicou ser motorista mas cast falhou. Nao salvando CNH.");
                }
            } else {
                arquivo_usuarios << "0";
            }
            arquivo_usuarios << std::endl;
        }
        arquivo_usuarios.close();
    }

    // <--- NOVA ASSINATURA: Recebe dados UFMG já processados
    void GerenciadorUsuarios::cadastrarUsuario(const std::string& cpf_digitado, const std::string& nome_ufmg, const std::string& data_nascimento_ufmg,
                                               const std::string& vinculo_ufmg, const std::string& detalhe_ufmg) {
        std::string telefone_digitado, email_digitado, senha_digitada;
        
        if (buscarUsuarioPorCpf(cpf_digitado)) {
            throw AppExcecao("CPF ja cadastrado no sistema. Por favor, faca login.");
        }

        // A verificação e obtenção de dados da UFMG foi movida para Sistema::fluxo_cadastro
        // O GerenciadorUsuarios agora apenas cria o usuário com os dados fornecidos.
        
        _terminal_io->exibirMensagem("--- Cadastro ---"); // Mensagem de contexto
        _terminal_io->exibirMensagem("Dados UFMG confirmados para " + nome_ufmg + "."); // Confirmação visual

        telefone_digitado = _terminal_io->coletarStringInput("Telefone (apenas numeros): ");
        email_digitado = _terminal_io->coletarStringInput("Email: ");
        senha_digitada = _terminal_io->coletarStringInput("Senha: ");
        Genero gen_digitado = _terminal_io->coletarGeneroInput("Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): ");

        Usuario* novo_usuario = new Usuario(nome_ufmg, cpf_digitado, telefone_digitado, data_nascimento_ufmg,
                                       email_digitado, senha_digitada, gen_digitado, vinculo_ufmg, detalhe_ufmg);

        _usuarios.push_back(novo_usuario);
        salvarDadosUsuariosPublico();
        _terminal_io->exibirMensagem("Cadastro realizado com sucesso!");
    }


    // <--- REMOVIDO: fluxo_cadastro foi movido para Sistema.cpp
    // void GerenciadorUsuarios::fluxo_cadastro(std::tuple<bool, std::string, std::string, std::string, std::string> (*buscar_dados_ufmg_func)(const std::string&)) { /* ... */ }

    Usuario* GerenciadorUsuarios::fluxo_login() {
        std::string cpf, senha;
        _terminal_io->exibirMensagem("--- Login ---");
        cpf = _terminal_io->coletarStringInput("CPF: ");
        senha = _terminal_io->coletarStringInput("Senha: ");
        Usuario* u = buscarUsuarioPorCpf(cpf);
        
        if (!u) {
            throw AutenticacaoFalhouException();
        }
        
        if (u->verificar_senha(senha)) {
            _terminal_io->exibirMensagem("Login bem-sucedido!");
            
            for (auto& notif : u->get_notificacoes_mutavel()) {
                if (!notif.is_lida()) {
                    _terminal_io->exibirMensagem("[NOVA NOTIFICACAO]: " + notif.get_mensagem());
                    notif.marcar_como_lida();
                }
            }
            return u;
        } else {
            throw AutenticacaoFalhouException();
        }
    }

    void GerenciadorUsuarios::logoutUsuario(Usuario*& usuario_logado) {
        usuario_logado = nullptr;
        _terminal_io->exibirMensagem("Logout efetuado.");
    }

    void GerenciadorUsuarios::fluxo_editar_perfil(Usuario* usuario) {
        int escolha;
        do {
            _terminal_io->exibirMensagem("\n--- Editar Perfil ---");
            usuario->imprimir_perfil(); // Método de Usuario
            _terminal_io->exibirMensagem("Escolha o campo para editar:");
            _terminal_io->exibirMensagem("(1) Email | (2) Telefone | (3) Senha | (4) Genero | (0) Voltar");
            
            escolha = _terminal_io->coletarIntInput("> ", 0, 4);
            std::string novo_valor;

            switch (escolha) {
                case 1: 
                    novo_valor = _terminal_io->coletarStringInput("Novo Email: ");
                    usuario->set_email(novo_valor);
                    _terminal_io->exibirMensagem("Email atualizado!");
                    break;
                case 2: 
                    novo_valor = _terminal_io->coletarStringInput("Novo Telefone (apenas numeros): ");
                    usuario->set_telefone(novo_valor);
                    _terminal_io->exibirMensagem("Telefone atualizado!");
                    break;
                case 3: 
                    novo_valor = _terminal_io->coletarStringInput("Nova Senha: ");
                    usuario->set_senha(novo_valor);
                    _terminal_io->exibirMensagem("Senha atualizada!");
                    break;
                case 4: { 
                    Genero gen_novo = _terminal_io->coletarGeneroInput("Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): ");
                    usuario->set_genero(gen_novo);
                    _terminal_io->exibirMensagem("Genero atualizado!");
                    break;
                }
                case 0:
                    _terminal_io->exibirMensagem("Voltando ao menu anterior...");
                    break;
                default:
                    _terminal_io->exibirMensagem("Opcao invalida.");
                    break;
            }
            if (escolha != 0) {
                salvarDadosUsuariosPublico(); // Salva as alterações
            }
        } while (escolha != 0);
    }

    void GerenciadorUsuarios::fluxo_tornar_motorista(Usuario*& usuario) {
        if (usuario->is_motorista()) {
            _terminal_io->exibirMensagem("Voce ja e um motorista cadastrado.");
            return;
        }

        std::string cnh_numero_digitado;
        _terminal_io->exibirMensagem("\n--- Cadastro para Motorista ---");
        cnh_numero_digitado = _terminal_io->coletarStringInput("Numero da CNH: ");

        // Encontra o usuário na lista e o substitui por um Motorista
        for (size_t i = 0; i < _usuarios.size(); ++i) {
            if (_usuarios[i] == usuario) {
                std::string nome = usuario->get_nome();
                std::string cpf = usuario->get_cpf();
                std::string telefone = usuario->get_telefone();
                std::string data_nascimento = usuario->get_data_nascimento();
                std::string email = usuario->get_email();
                std::string senha = usuario->get_senha();
                Genero genero = usuario->get_genero();
                std::string vinculo_tipo = usuario->get_vinculo_raw();
                std::string detalhe_vinculo = usuario->get_detalhe_vinculo();

                Motorista* novo_motorista = new Motorista(nome, cpf, telefone, data_nascimento, email, senha, genero,
                                                          vinculo_tipo, detalhe_vinculo, cnh_numero_digitado);
                
                delete usuario; // Libera a memória do objeto Usuario antigo
                _usuarios[i] = novo_motorista; // Substitui na lista
                usuario = novo_motorista; // Atualiza o ponteiro do usuario logado

                salvarDadosUsuariosPublico();
                _terminal_io->exibirMensagem("Parabens! Voce agora e um motorista cadastrado.");
                return;
            }
        }
        _terminal_io->exibirErro("ERRO INTERNO: Usuario logado nao encontrado na lista de usuarios.");
    }

    Usuario* GerenciadorUsuarios::buscarUsuarioPorCpf(const std::string& cpf) const {
        for (const auto& u : _usuarios) {
            if (u->get_cpf() == cpf) return u;
        }
        return nullptr;
    }

    const std::vector<Usuario*>& GerenciadorUsuarios::getUsuarios() const {
        return _usuarios;
    }

    void GerenciadorUsuarios::enviarNotificacao(Usuario* usuario, const std::string& mensagem) {
        if (usuario) {
            Notificacao nova_notificacao(mensagem);
            usuario->adicionar_notificacao(nova_notificacao);
            _terminal_io->exibirMensagem("[NOTIFICACAO para " + usuario->get_nome() + "]: " + mensagem);
        }
    }

} // namespace ufmg_carona