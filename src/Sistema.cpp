#include "Sistema.hpp"
#include "Excecoes.hpp"
#include "CaronaFactory.hpp"
#include "Veiculo.hpp"
#include "Solicitacao.hpp"
#include "Avaliacao.hpp"
#include "Motorista.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <map>
#include <tuple>

namespace ufmg_carona {

    Sistema::Sistema() : _usuario_logado(nullptr) {
        carregar_dados_iniciais();
    }

    Sistema::~Sistema() {
        std::cout << "\nFinalizando o sistema..." << std::endl;
        salvar_dados_usuarios();
        salvar_dados_veiculos();
        
        for (Usuario* u : _usuarios) {
            delete u;
        }
        _usuarios.clear();

        for (Solicitacao* s : _solicitacoes) {
            delete s;
        }
        _solicitacoes.clear();
    }

    void Sistema::executar() {
        std::cout << "== Sistema de Caronas UFMG iniciado ==" << std::endl;
        std::string comando_str;

        while (true) {
            // Exibir o menu inicial com números para o estado não logado
            if (!_usuario_logado) {
                exibir_menu_inicial_nao_logado();
            } else {
                exibir_menu_logado();
            }
            
            std::cout << "> ";
            std::getline(std::cin, comando_str);

            if (comando_str.empty()) {
                continue;
            }

            try {
                int comando_int = -1; // Valor padrão para comando não numérico
                bool is_numeric = true;

                try {
                    comando_int = std::stoi(comando_str);
                } catch (const std::invalid_argument&) {
                    is_numeric = false;
                }

                if (!_usuario_logado) {
                    // Lógica para comandos do menu inicial (não logado)
                    if (is_numeric) {
                        if (comando_int == 1) { // Cadastro
                            fluxo_cadastro();
                        } else if (comando_int == 2) { // Login
                            fluxo_login();
                        } else if (comando_int == 0) { // Sair
                            std::cout << "Saindo do programa..." << std::endl;
                            break; // Sai do loop principal
                        } else {
                            throw ComandoInvalidoException(comando_str);
                        }
                    } else if (comando_str == "cadastro") { // Retrocompatibilidade ou comandos textuais
                        fluxo_cadastro();
                    } else if (comando_str == "login") { // Retrocompatibilidade ou comandos textuais
                        fluxo_login();
                    } else if (comando_str == "sair") { // Retrocompatibilidade ou comandos textuais
                        std::cout << "Saindo do programa..." << std::endl;
                        break; // Sai do loop principal
                    }
                    else {
                        throw ComandoInvalidoException(comando_str);
                    }
                } else {
                    // Lógica para usuário logado
                    // Note: processar_comando_logado já assume entrada numérica
                    processar_comando_logado(comando_str);
                }
            } catch (const ComandoInvalidoException& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            } catch (const AppExcecao& e) {
                if (std::string(e.what()) == "Sair") {
                    // Este break é para a exceção lançada de processar_comando_logado
                    break; 
                }
                std::cerr << "ERRO: " << e.what() << std::endl;
            } catch (const std::bad_alloc& e) {
                std::cerr << "ERRO: Falha na alocacao de memoria: " << e.what() << std::endl;
            } catch (const std::exception& e) { // Captura outras exceções inesperadas
                std::cerr << "ERRO INESPERADO: " << e.what() << std::endl;
            }
        }
    }

    int Sistema::coletar_int_input(const std::string& prompt, int min_val, int max_val) {
        int valor;
        while (true) {
            std::cout << prompt;
            std::cin >> valor;
            if (std::cin.fail()) {
                std::cout << "Entrada invalida. Digite um numero." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (valor < min_val || valor > max_val) {
                std::cout << "Numero fora do intervalo permitido (" << min_val << "-" << max_val << ")."<< (min_val == 0 ? " Digite 0 para voltar." : "") << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return valor;
            }
        }
    }

    // NOVA FUNÇÃO: Menu inicial com números para não logado
    void Sistema::exibir_menu_inicial_nao_logado() {
        std::cout << "\n--- Menu Principal ---" << std::endl;
        std::cout << "(1) Cadastro | (2) Login | (0) Sair" << std::endl;
    }

    // FUNÇÃO AUXILIAR: Extrair lógica de processamento de comando para usuário logado
    void Sistema::processar_comando_logado(const std::string& comando_str) {
        int comando_int;
        try {
            comando_int = std::stoi(comando_str);
        } catch (const std::invalid_argument&) {
            throw ComandoInvalidoException(comando_str); // Se não for número, é comando inválido
        }

        if (comando_int == 1) { // Perfil
            _usuario_logado->imprimir_perfil();
            fluxo_editar_perfil_ou_veiculos();
        } else if (comando_int == 2) { // Passageiro
            fluxo_passageiro_menu();
        } else if (comando_int == 3) { // Motorista
            // Motorista agora sempre direciona para fluxo_motorista_menu, que fará a checagem
            fluxo_motorista_menu();
        } else if (comando_int == 4) { // Logout
            fluxo_logout();
        } else if (comando_int == 5) { // Sair do programa
            std::cout << "Saindo do programa..." << std::endl;
            throw AppExcecao("Sair"); // Sinaliza para executar() sair
        } else {
            throw ComandoInvalidoException(comando_str);
        }
    }

    // A função exibir_menu() original será mantida com corpo vazio,
    // já que as chamadas foram movidas para dentro de executar().
    void Sistema::exibir_menu() {
        // Este método não é mais usado diretamente como um dispatcher.
        // As chamadas foram substituídas por exibir_menu_inicial_nao_logado()
        // e exibir_menu_logado() dentro de 'executar()'.
    }


    // NOVO: Menu para usuario logado
    void Sistema::exibir_menu_logado() {
        std::cout << "\nLogado como " << _usuario_logado->get_nome() << std::endl;
        std::cout << "(1) Perfil | (2) Passageiro | (3) Motorista | (4) Logout | (5) Sair" << std::endl;
    }

    // NOVO: Menu para passageiro
    void Sistema::exibir_menu_passageiro() {
        std::cout << "\n--- Menu Passageiro ---" << std::endl;
        std::cout << "(1) Solicitar Carona | (2) Status das Minhas Solicitacoes | (0) Voltar" << std::endl;
    }

    // NOVO: Menu para motorista
    void Sistema::exibir_menu_motorista() {
        std::cout << "\n--- Menu Motorista ---" << std::endl;
        std::cout << "(1) Oferecer Carona | (2) Gerenciar Solicitacoes | (3) Cadastrar Veiculo | (0) Voltar" << std::endl;
    }

    // NOVO: Fluxo do menu de passageiro
    void Sistema::fluxo_passageiro_menu() {
        int comando;
        do {
            exibir_menu_passageiro();
            comando = coletar_int_input("> ", 0, 2);

            try {
                if (comando == 1) {
                    fluxo_solicitar_carona();
                } else if (comando == 2) {
                    fluxo_status_caronas();
                }
                // Se comando for 0, o loop termina e ele volta para o menu anterior
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            }
        } while (comando != 0); // Continua no loop ate o usuario escolher '0' para voltar
    }

    // NOVO: Fluxo do menu de motorista
    void Sistema::fluxo_motorista_menu() {
        // Se o usuario NAO for motorista, oferece para ele se tornar um
        if (!_usuario_logado->is_motorista()) {
            std::cout << "Voce nao esta registrado como motorista." << std::endl;
            char deseja_se_tornar;
            std::cout << "Deseja se tornar um motorista? (s/n): ";
            std::cin >> deseja_se_tornar;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (deseja_se_tornar == 's' || deseja_se_tornar == 'S') {
                fluxo_tornar_motorista(); // NOVO: Chama o fluxo para se tornar motorista
            }
            return; // Sempre retorna apos tentar se tornar motorista para reexibir o menu principal
        }

        // Se ja for motorista, exibe o menu normal de motorista
        int comando;
        do {
            exibir_menu_motorista();
            comando = coletar_int_input("> ", 0, 3);

            try {
                if (comando == 1) {
                    fluxo_oferecer_carona();
                } else if (comando == 2) {
                    fluxo_gerenciar_solicitacoes();
                } else if (comando == 3) {
                    fluxo_cadastrar_veiculo();
                }
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            }
        } while (comando != 0); // Continua no loop ate o usuario escolher '0' para voltar
    }

    // NOVO: Fluxo para lidar com o sub-menu de Perfil (Editar ou Gerenciar Veiculos)
    void Sistema::fluxo_editar_perfil_ou_veiculos() {
        int sub_comando_perfil;
        do {
            std::cout << "\n--- Menu Perfil ---" << std::endl;
            std::cout << "(1) Editar Perfil | (2) Gerenciar Veiculos (Motorista) | (0) Voltar" << std::endl;
            sub_comando_perfil = coletar_int_input("> ", 0, 2);

            try {
                if (sub_comando_perfil == 1) {
                    fluxo_editar_perfil();
                } else if (sub_comando_perfil == 2) {
                    if (_usuario_logado->is_motorista()) {
                        fluxo_gerenciar_veiculos();
                    } else {
                        std::cout << "Voce nao eh motorista para gerenciar veiculos." << std::endl;
                    }
                }
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            }
        } while (sub_comando_perfil != 0); // Continua no loop ate o usuario escolher '0' para voltar
    }

    // NOVO: Fluxo para tornar um usuario em motorista
    void Sistema::fluxo_tornar_motorista() {
        // Verifica se o usuario ja nao eh um motorista (ja foi checado antes, mas eh uma boa pratica)
        if (_usuario_logado->is_motorista()) {
            std::cout << "Voce ja e um motorista cadastrado." << std::endl;
            return;
        }

        std::string cnh_numero_digitado;
        std::cout << "\n--- Cadastro para Motorista ---" << std::endl;
        std::cout << "Numero da CNH: "; std::getline(std::cin, cnh_numero_digitado);

        // Encontra o indice do usuario atual na lista _usuarios
        auto it = std::find(_usuarios.begin(), _usuarios.end(), _usuario_logado);
        if (it == _usuarios.end()) {
            std::cerr << "ERRO INTERNO: Usuario logado nao encontrado na lista de usuarios." << std::endl;
            return;
        }

        // Obtem os dados do usuario atual para criar o novo motorista
        std::string nome = _usuario_logado->get_nome();
        std::string cpf = _usuario_logado->get_cpf();
        std::string telefone = _usuario_logado->get_telefone();
        std::string data_nascimento = _usuario_logado->get_data_nascimento();
        std::string email = _usuario_logado->get_email();
        std::string senha = _usuario_logado->get_senha();
        Genero genero = _usuario_logado->get_genero();
        std::string vinculo_tipo = _usuario_logado->get_vinculo_raw();
        std::string detalhe_vinculo = _usuario_logado->get_detalhe_vinculo();

        // Cria um novo objeto Motorista
        Motorista* novo_motorista = new Motorista(nome, cpf, telefone, data_nascimento, email, senha, genero,
                                                  vinculo_tipo, detalhe_vinculo, cnh_numero_digitado);
        
        // Libera a memoria do Usuario antigo
        delete _usuario_logado; 

        // Substitui o ponteiro antigo pelo novo Motorista na lista _usuarios
        *it = novo_motorista;
        
        // Atualiza _usuario_logado para apontar para o novo Motorista
        _usuario_logado = novo_motorista;

        salvar_dados_usuarios(); // Salva os dados atualizados do usuario (agora motorista)

        std::cout << "Parabens! Voce agora e um motorista cadastrado." << std::endl;

        // Oferece para cadastrar um veiculo imediatamente
        char cadastrar_veiculo_char;
        std::cout << "Deseja cadastrar um veiculo agora? (s/n): ";
        std::cin >> cadastrar_veiculo_char;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (cadastrar_veiculo_char == 's' || cadastrar_veiculo_char == 'S') {
            fluxo_cadastrar_veiculo();
        }
    }


    void Sistema::fluxo_cadastro() {
        std::string cpf_digitado, telefone_digitado, email_digitado, senha_digitada;
        int gen_int;
        
        std::string nome_ufmg, data_nascimento_ufmg, vinculo_ufmg, detalhe_ufmg;
        
        std::cout << "--- Cadastro ---" << std::endl;
        std::cout << "CPF: "; std::getline(std::cin, cpf_digitado);

        if (buscar_usuario_por_cpf(cpf_digitado)) {
            throw AppExcecao("CPF ja cadastrado no sistema. Por favor, faca login.");
        }

        auto dados_ufmg = buscar_dados_ufmg_por_cpf(cpf_digitado);
        bool cpf_encontrado_ufmg = std::get<0>(dados_ufmg);

        if (!cpf_encontrado_ufmg) {
            std::cout << "CPF nao encontrado no cadastro da UFMG. O cadastro nao pode ser realizado." << std::endl;
            return;
        }

        nome_ufmg = std::get<1>(dados_ufmg);
        data_nascimento_ufmg = std::get<2>(dados_ufmg);
        vinculo_ufmg = std::get<3>(dados_ufmg);
        detalhe_ufmg = std::get<4>(dados_ufmg);

        if (nome_ufmg.empty() || data_nascimento_ufmg.empty() || vinculo_ufmg.empty()) {
            std::cout << "As informacoes do seu CPF estao incompletas no cadastro da UFMG. Por favor, procure a secretaria da UFMG para regularizar seu cadastro." << std::endl;
            return;
        }

        std::cout << "Telefone (apenas numeros): "; std::getline(std::cin, telefone_digitado);
        std::cout << "Email: "; std::getline(std::cin, email_digitado);
        std::cout << "Senha: "; std::getline(std::cin, senha_digitada);
        std::cout << "Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): ";
        gen_int = coletar_int_input("", 0, 3);
        Genero gen_digitado = static_cast<Genero>(gen_int);

        Usuario* novo_usuario = new Usuario(nome_ufmg, cpf_digitado, telefone_digitado, data_nascimento_ufmg,
                                       email_digitado, senha_digitada, gen_digitado, vinculo_ufmg, detalhe_ufmg);

        _usuarios.push_back(novo_usuario);
        salvar_dados_usuarios();
        std::cout << "Cadastro realizado com sucesso!" << std::endl;
    }

    void Sistema::fluxo_login() {
        std::string cpf, senha;
        std::cout << "--- Login ---" << std::endl;
        std::cout << "CPF: "; std::getline(std::cin, cpf);
        std::cout << "Senha: "; std::getline(std::cin, senha);
        Usuario* u = buscar_usuario_por_cpf(cpf);
        if (u && u->verificar_senha(senha)) {
            _usuario_logado = u;
            std::cout << "Login bem-sucedido!" << std::endl;
        } else { throw AutenticacaoFalhouException(); }
    }

    void Sistema::fluxo_logout() {
        _usuario_logado = nullptr;
        std::cout << "Logout efetuado." << std::endl;
    }

    void Sistema::fluxo_editar_perfil() {
        int escolha;
        do { // Adiciona um loop para permitir edicao multipla ou voltar
            std::cout << "\n--- Editar Perfil ---" << std::endl;
            _usuario_logado->imprimir_perfil(); // Mostra o perfil atual
            std::cout << "Escolha o campo para editar:" << std::endl;
            std::cout << "(1) Email | (2) Telefone | (3) Senha | (4) Genero | (0) Voltar" << std::endl;
            
            escolha = coletar_int_input("> ", 0, 4);
            std::string novo_valor;

            switch (escolha) {
                case 1: // Email
                    std::cout << "Novo Email: ";
                    std::getline(std::cin, novo_valor);
                    _usuario_logado->set_email(novo_valor);
                    std::cout << "Email atualizado!" << std::endl;
                    break;
                case 2: // Telefone
                    std::cout << "Novo Telefone (apenas numeros): ";
                    std::getline(std::cin, novo_valor);
                    _usuario_logado->set_telefone(novo_valor);
                    std::cout << "Telefone atualizado!" << std::endl;
                    break;
                case 3: // Senha
                    std::cout << "Nova Senha: ";
                    std::getline(std::cin, novo_valor);
                    _usuario_logado->set_senha(novo_valor);
                    std::cout << "Senha atualizada!" << std::endl;
                    break;
                case 4: { // Genero - Adicionado escopo para 'gen_int'
                    std::cout << "Novo Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): ";
                    int gen_int = coletar_int_input("", 0, 3);
                    _usuario_logado->set_genero(static_cast<Genero>(gen_int));
                    std::cout << "Genero atualizado!" << std::endl;
                    break;
                }
                case 0:
                    std::cout << "Voltando ao menu anterior..." << std::endl;
                    break;
                default:
                    std::cout << "Opcao invalida." << std::endl;
                    break;
            }
            if (escolha != 0) { // Salva apenas se algo foi editado
                salvar_dados_usuarios(); // Salva as alteracoes no arquivo
            }
        } while (escolha != 0); // Continua no loop ate o usuario escolher '0' para voltar
    }

    // NOVO: Fluxo para gerenciar veiculos do motorista
    void Sistema::fluxo_gerenciar_veiculos() {
        if (!_usuario_logado->is_motorista()) {
            std::cout << "Voce nao eh motorista para gerenciar veiculos." << std::endl;
            return;
        }
        Motorista* motorista_logado = dynamic_cast<Motorista*>(_usuario_logado);
        if (!motorista_logado) return;

        int comando;
        do {
            std::cout << "\n--- Gerenciar Meus Veiculos ---" << std::endl;
            if (motorista_logado->get_veiculos().empty()) {
                std::cout << "Nenhum veiculo cadastrado." << std::endl;
            } else {
                std::cout << "Seus veiculos cadastrados:" << std::endl;
                for (size_t i = 0; i < motorista_logado->get_veiculos().size(); ++i) {
                    std::cout << "[" << (i + 1) << "] ";
                    if (motorista_logado->get_veiculos()[i]) {
                        motorista_logado->get_veiculos()[i]->exibir_info();
                    }
                }
            }
            
            std::cout << "\n(1) Cadastrar Novo Veiculo | (2) Editar Veiculo | (3) Excluir Veiculo | (0) Voltar" << std::endl;
            comando = coletar_int_input("> ", 0, 3);

            try {
                if (comando == 1) {
                    fluxo_cadastrar_veiculo();
                } else if (comando == 2) {
                    fluxo_editar_veiculo(motorista_logado);
                } else if (comando == 3) {
                    fluxo_excluir_veiculo(motorista_logado);
                }
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            }
        } while (comando != 0); // Continua no loop ate o usuario escolher '0' para voltar
    }

    // NOVO: Fluxo para editar um veiculo especifico
    void Sistema::fluxo_editar_veiculo(Motorista* motorista) {
        if (motorista->get_veiculos().empty()) {
            std::cout << "Nenhum veiculo para editar." << std::endl;
            return;
        }

        std::string placa_escolhida;
        std::cout << "Digite a placa do veiculo que deseja editar (0 para voltar): ";
        std::getline(std::cin, placa_escolhida);

        if (placa_escolhida == "0") return;

        Veiculo* veiculo_para_editar = motorista->buscar_veiculo_por_placa(placa_escolhida);
        if (!veiculo_para_editar) {
            std::cout << "Veiculo com a placa '" << placa_escolhida << "' nao encontrado." << std::endl;
            return;
        }

        int escolha;
        do { // Loop para permitir editar varios campos ou voltar
            std::cout << "\n--- Editando Veiculo: " << placa_escolhida << " ---" << std::endl;
            veiculo_para_editar->exibir_info();
            std::cout << "Escolha o campo para editar:" << std::endl;
            std::cout << "(1) Marca | (2) Modelo | (3) Cor | (4) Lugares | (0) Voltar" << std::endl;
            escolha = coletar_int_input("> ", 0, 4);
            std::string novo_str_valor;
            int novo_int_valor;

            switch (escolha) {
                case 1:
                    std::cout << "Nova Marca: "; std::getline(std::cin, novo_str_valor);
                    veiculo_para_editar->set_marca(novo_str_valor);
                    break;
                case 2:
                    std::cout << "Novo Modelo: "; std::getline(std::cin, novo_str_valor);
                    veiculo_para_editar->set_modelo(novo_str_valor);
                    break;
                case 3:
                    std::cout << "Nova Cor: "; std::getline(std::cin, novo_str_valor);
                    veiculo_para_editar->set_cor(novo_str_valor);
                    break;
                case 4:
                    std::cout << "Total de Lugares (com motorista): ";
                    novo_int_valor = coletar_int_input("", 2, 99); // Limites razoaveis para lugares
                    veiculo_para_editar->set_lugares(novo_int_valor);
                    break;
                case 0:
                    std::cout << "Voltando..." << std::endl;
                    break;
                default:
                    std::cout << "Opcao invalida." << std::endl;
                    break;
            }
            if (escolha != 0) { // Salva apenas se algo foi editado
                salvar_dados_veiculos(); // Salva as alteracoes
                std::cout << "Veiculo atualizado com sucesso!" << std::endl;
            }
        } while (escolha != 0); // Continua no loop ate o usuario escolher '0' para voltar
    }

    // NOVO: Fluxo para excluir um veiculo
    void Sistema::fluxo_excluir_veiculo(Motorista* motorista) {
        if (motorista->get_veiculos().empty()) {
            std::cout << "Nenhum veiculo para excluir." << std::endl;
            return;
        }

        std::string placa_escolhida;
        std::cout << "Digite a placa do veiculo que deseja excluir (0 para voltar): ";
        std::getline(std::cin, placa_escolhida);

        if (placa_escolhida == "0") return;

        char confirmar;
        std::cout << "Tem certeza que deseja excluir o veiculo " << placa_escolhida << "? (s/n): ";
        std::cin >> confirmar;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (confirmar == 's' || confirmar == 'S') {
            if (motorista->remover_veiculo(placa_escolhida)) {
                salvar_dados_veiculos(); // Salva as alteracoes
                std::cout << "Veiculo " << placa_escolhida << " excluido com sucesso!" << std::endl;
            } else {
                std::cout << "Veiculo com a placa '" << placa_escolhida << "' nao encontrado." << std::endl;
            }
        } else {
            std::cout << "Exclusao cancelada." << std::endl;
        }
    }


    void Sistema::fluxo_cadastrar_veiculo() {
        if (!_usuario_logado->is_motorista()) {
            std::cout << "Voce nao esta registrado como motorista. Nao e possivel cadastrar veiculo." << std::endl;
            return;
        }
        
        Motorista* motorista_logado = dynamic_cast<Motorista*>(_usuario_logado);
        if (!motorista_logado) {
            std::cerr << "ERRO INTERNO: Usuario logado deveria ser motorista mas nao pode ser convertido." << std::endl;
            return;
        }

        std::string placa, marca, modelo, cor;
        int lugares;

        char cadastrar_outro = 's';
        while (cadastrar_outro == 's' || cadastrar_outro == 'S') {
            std::cout << "\n--- Cadastrar Novo Veiculo ---" << std::endl;
            std::cout << "Placa: "; std::getline(std::cin, placa);
            if (motorista_logado->buscar_veiculo_por_placa(placa) != nullptr) {
                std::cout << "Voce ja possui um veiculo com esta placa." << std::endl;
                std::cout << "Deseja tentar com outra placa? (s/n): ";
                char tentar_outra;
                std::cin >> tentar_outra;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (!(tentar_outra == 's' || tentar_outra == 'S')) {
                    cadastrar_outro = 'n'; // Sai do loop
                }
                continue; // Volta para o inicio do loop
            }

            std::cout << "Marca: "; std::getline(std::cin, marca);
            std::cout << "Modelo: "; std::getline(std::cin, modelo);
            std::cout << "Cor: "; std::getline(std::cin, cor);
            std::cout << "Total de lugares (com motorista): ";
            lugares = coletar_int_input("", 2, 99); // Limites razoaveis para lugares
            
            motorista_logado->adicionar_veiculo(new Veiculo(placa, marca, modelo, cor, lugares));
            salvar_dados_veiculos();
            std::cout << "Veiculo cadastrado com sucesso!" << std::endl;

            std::cout << "Deseja cadastrar outro veiculo? (s/n): ";
            std::cin >> cadastrar_outro;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    void Sistema::fluxo_oferecer_carona() {
        if (!_usuario_logado->is_motorista()) {
            throw AppExcecao("Voce nao pode oferecer caronas. Cadastre-se como motorista.");
        }
        
        Motorista* motorista_logado = dynamic_cast<Motorista*>(_usuario_logado);
        if (!motorista_logado || motorista_logado->get_veiculos().empty()) {
             throw AppExcecao("Voce precisa cadastrar um veiculo para oferecer caronas.");
        }

        std::cout << "\n--- Oferecer Carona ---" << std::endl;
        std::cout << "Seus veiculos cadastrados:" << std::endl;
        for (size_t i = 0; i < motorista_logado->get_veiculos().size(); ++i) {
            std::cout << "[" << (i + 1) << "] ";
            if (motorista_logado->get_veiculos()[i]) {
                motorista_logado->get_veiculos()[i]->exibir_info();
            }
        }
        std::string placa_escolhida;
        std::cout << "Digite a placa do veiculo que deseja usar para esta carona (0 para voltar): ";
        std::getline(std::cin, placa_escolhida);
        
        if (placa_escolhida == "0") return; // Volta se o usuario digitar 0

        Veiculo* veiculo_selecionado = motorista_logado->buscar_veiculo_por_placa(placa_escolhida);
        if (!veiculo_selecionado) {
            std::cout << "Veiculo com a placa informada nao encontrado em seus cadastros." << std::endl;
            return;
        }

        std::string origem, destino, data;
        char apenas_mulheres_char;
        std::cout << "Origem: "; std::getline(std::cin, origem);
        std::cout << "Destino: "; std::getline(std::cin, destino);
        std::cout << "Data e Hora (ex: 18/06/2025 08:00): "; std::getline(std::cin, data);
        std::cout << "Apenas para mulheres? (s/n): "; std::cin >> apenas_mulheres_char;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        bool apenas_mulheres = (apenas_mulheres_char == 's' || apenas_mulheres_char == 'S');
        
        Carona nova_carona = CaronaFactory::criar_carona(origem, destino, data, _usuario_logado, veiculo_selecionado, apenas_mulheres, TipoCarona::AGENDADA);
        _caronas.push_back(nova_carona);
        std::cout << "Carona (ID: " << nova_carona.get_id() << ") criada com sucesso usando o veiculo " << veiculo_selecionado->get_placa() << "!" << std::endl;
    }

    // ALTERADO: fluxo_solicitar_carona agora inclui busca e filtros e NAO MOSTRA SOLICITACOES JA FEITAS
    void Sistema::fluxo_solicitar_carona() {
        std::cout << "\n--- Solicitar Carona ---" << std::endl;
        if (_caronas.empty()) {
            std::cout << "Nenhuma carona disponivel no momento." << std::endl;
            return;
        }

        std::string filtro_origem, filtro_destino, filtro_data, filtro_hora;
        int filtro_genero_int = -1; // -1 = sem filtro de genero

        char usar_filtro;
        std::cout << "Deseja usar filtros para buscar caronas? (s/n): ";
        std::cin >> usar_filtro;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (usar_filtro == 's' || usar_filtro == 'S') {
            std::cout << "Filtro de Origem (deixe em branco para ignorar): "; std::getline(std::cin, filtro_origem);
            std::cout << "Filtro de Destino (deixe em branco para ignorar): "; std::getline(std::cin, filtro_destino);
            std::cout << "Filtro de Data (DD/MM/AAAA, deixe em branco para ignorar): "; std::getline(std::cin, filtro_data);
            std::cout << "Filtro de Hora (HH:MM, deixe em branco para ignorar): "; std::getline(std::cin, filtro_hora);
            std::cout << "Filtro de Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar, -1:Sem Filtro): ";
            filtro_genero_int = coletar_int_input("", -1, 3);
        }

        std::vector<Carona*> caronas_filtradas;
        for (auto& carona : _caronas) {
            // Verifica se o usuario logado ja solicitou esta carona
            bool ja_solicitou = false;
            for (const auto& solicitacao : _solicitacoes) {
                if (solicitacao->get_passageiro() == _usuario_logado &&
                    solicitacao->get_carona()->get_id() == carona.get_id()) {
                    ja_solicitou = true;
                    break;
                }
            }
            if (ja_solicitou) {
                continue; // Pula para a proxima carona se o usuario ja solicitou esta
            }

            // Continua com os filtros existentes
            bool atende_filtro_origem = filtro_origem.empty() || carona.get_origem().find(filtro_origem) != std::string::npos;
            bool atende_filtro_destino = filtro_destino.empty() || carona.get_destino().find(filtro_destino) != std::string::npos;
            
            // Simples comparacao de data e hora
            bool atende_filtro_data = filtro_data.empty() || carona.get_data_hora().find(filtro_data) != std::string::npos;
            bool atende_filtro_hora = filtro_hora.empty() || carona.get_data_hora().find(filtro_hora) != std::string::npos;

            bool atende_filtro_genero = true;
            if (filtro_genero_int != -1) {
                // Se a carona é apenas para mulheres e o filtro é Feminino (1), atende
                // Se a carona não é apenas para mulheres, atende qualquer filtro de gênero
                if (carona.get_apenas_mulheres() && static_cast<Genero>(filtro_genero_int) != Genero::FEMININO) {
                    atende_filtro_genero = false;
                }
            }


            if (carona.get_vagas_disponiveis() > 0 &&
                atende_filtro_origem && atende_filtro_destino &&
                atende_filtro_data && atende_filtro_hora &&
                atende_filtro_genero) {
                caronas_filtradas.push_back(&carona);
            }
        }

        if (caronas_filtradas.empty()) {
            std::cout << "Nenhuma carona encontrada com os filtros especificados ou voce ja solicitou todas as caronas disponiveis." << std::endl;
            return;
        }

        std::cout << "\nCaronas disponiveis (filtradas):" << std::endl;
        for (const auto& carona_ptr : caronas_filtradas) {
            carona_ptr->exibir_info();
        }

        int id_carona;
        std::cout << "\nDigite o ID da carona que deseja solicitar (0 para voltar ao menu Passageiro): ";
        id_carona = coletar_int_input("", 0, std::numeric_limits<int>::max());

        if (id_carona == 0) return; // Volta para o menu de passageiro

        Carona* carona_escolhida = buscar_carona_por_id(id_carona);
        if (!carona_escolhida) {
            std::cout << "Carona nao encontrada!" << std::endl;
            return;
        }

        if (!pode_solicitar_carona(_usuario_logado, *carona_escolhida)) {
            return;
        }

        Solicitacao* nova_solicitacao = new Solicitacao(_usuario_logado, carona_escolhida);
        _solicitacoes.push_back(nova_solicitacao);
        carona_escolhida->adicionar_solicitacao(nova_solicitacao);

        std::string mensagem = "Nova solicitacao de carona de " + _usuario_logado->get_nome() +
                              " para a carona ID: " + std::to_string(id_carona);
        enviar_notificacao(carona_escolhida->get_motorista(), mensagem);

        std::cout << "Solicitacao enviada com sucesso!" << std::endl;
    }

    void Sistema::fluxo_gerenciar_solicitacoes() {
        if (!_usuario_logado->is_motorista()) {
            std::cout << "Apenas motoristas podem gerenciar solicitacoes." << std::endl;
            return;
        }

        std::cout << "\n--- Gerenciar Solicitacoes ---" << std::endl;

        std::vector<Solicitacao*> solicitacoes_motorista;
        for (const auto& solicitacao : _solicitacoes) {
            if (solicitacao->get_carona() && solicitacao->get_carona()->get_motorista() == _usuario_logado &&
                solicitacao->get_status() == StatusSolicitacao::PENDENTE) {
                solicitacoes_motorista.push_back(solicitacao);
            }
        }

        if (solicitacoes_motorista.empty()) {
            std::cout << "Nenhuma solicitacao pendente." << std::endl;
            return;
        }

        std::cout << "Solicitacoes pendentes:" << std::endl;
        for (size_t i = 0; i < solicitacoes_motorista.size(); ++i) {
            std::cout << "[" << (i+1) << "] ";
            solicitacoes_motorista[i]->exibir_para_motorista();
        }

        int escolha;
        std::cout << "\nEscolha uma solicitacao para responder (0 para voltar): ";
        escolha = coletar_int_input("> ", 0, solicitacoes_motorista.size());

        if (escolha == 0) {
            return;
        }

        Solicitacao* solicitacao_escolhida = solicitacoes_motorista[escolha - 1];

        char resposta;
        std::cout << "Aceitar solicitacao? (s/n): ";
        std::cin >> resposta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (resposta == 's' || resposta == 'S') {
            // Verifica novamente as vagas antes de aceitar
            if (solicitacao_escolhida->get_carona()->get_vagas_disponiveis() > 0) {
                solicitacao_escolhida->aceitar();
                solicitacao_escolhida->get_carona()->adicionar_passageiro(solicitacao_escolhida->get_passageiro());
                enviar_notificacao(solicitacao_escolhida->get_passageiro(),
                                "Sua solicitacao de carona foi ACEITA!");
                std::cout << "Solicitacao aceita!" << std::endl;
            } else {
                solicitacao_escolhida->recusar(); // Recusa se nao houver vagas
                enviar_notificacao(solicitacao_escolhida->get_passageiro(),
                                "Sua solicitacao de carona foi RECUSADA. Carona sem vagas.");
                std::cout << "Nao foi possivel aceitar a solicitacao: carona sem vagas." << std::endl;
            }
        } else {
            solicitacao_escolhida->recusar();
            enviar_notificacao(solicitacao_escolhida->get_passageiro(),
                             "Sua solicitacao de carona foi RECUSADA.");
            std::cout << "Solicitacao recusada." << std::endl;
        }
    }

    void Sistema::fluxo_status_caronas() {
        std::cout << "\n--- Status das Minhas Solicitacoes ---" << std::endl;

        std::vector<Solicitacao*> minhas_solicitacoes;
        for (const auto& solicitacao : _solicitacoes) {
            if (solicitacao->get_passageiro() == _usuario_logado) {
                minhas_solicitacoes.push_back(solicitacao);
            }
        }

        if (minhas_solicitacoes.empty()) {
            std::cout << "Voce nao fez nenhuma solicitacao de carona ainda." << std::endl;
            return;
        }

        for (const auto& solicitacao : minhas_solicitacoes) {
            std::cout << "\n--- Solicitacao ---" << std::endl;
            std::cout << "Carona: " << solicitacao->get_carona()->get_origem()
                      << " -> " << solicitacao->get_carona()->get_destino() << std::endl;
            std::cout << "Data: " << solicitacao->get_carona()->get_data_hora() << std::endl;
            std::cout << "Motorista: " << solicitacao->get_carona()->get_motorista()->get_nome() << std::endl;
            std::cout << "Status: " << solicitacao->get_status_string() << std::endl;
        }
    }

    Usuario* Sistema::buscar_usuario_por_cpf(const std::string& cpf) {
        for (const auto& u : _usuarios) {
            if (u->get_cpf() == cpf) return u;
        }
        return nullptr;
    }
    
    // NOVO: Busca um veiculo por placa dentro dos veiculos de um motorista especifico
    Veiculo* Sistema::buscar_veiculo_por_placa_motorista(Motorista* motorista, const std::string& placa) {
        if (!motorista) return nullptr;
        for (Veiculo* v : motorista->get_veiculos()) {
            if (v && v->get_placa() == placa) { // Adicionado 'v &&' para seguranca
                return v;
            }
        }
        return nullptr;
    }

    std::tuple<bool, std::string, std::string, std::string, std::string> Sistema::buscar_dados_ufmg_por_cpf(const std::string& cpf_buscado) {
        std::ifstream arquivo_ufmg("dados_ufmg.txt");
        if (!arquivo_ufmg.is_open()) {
            std::cerr << "ERRO: Nao foi possivel abrir o arquivo dados_ufmg.txt." << std::endl;
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

    void Sistema::carregar_dados_iniciais() {
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
                    std::cerr << "AVISO: Linha com formato invalido no usuarios.txt (menos de 10 campos): " << linha << std::endl;
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
                if (campos.size() >= 11) { // Mudado para >= para evitar segfault se houver mais campos por engano
                    cnh_numero_lida = campos[10];
                }

                Genero gen;
                try {
                    gen = static_cast<Genero>(std::stoi(gen_str));
                } catch (const std::invalid_argument& e) {
                    std::cerr << "ERRO: Valor invalido para 'genero' no usuarios.txt para CPF " << cpf << ". Ignorando usuario. Erro: " << e.what() << std::endl;
                    continue;
                } catch (const std::out_of_range& e) {
                    std::cerr << "ERRO: Valor de 'genero' fora do limite para CPF " << cpf << " no usuarios.txt. Ignorando usuario. Erro: " << e.what() << std::endl;
                    continue;
                }


                bool eh_motorista_do_arquivo;
                try {
                    eh_motorista_do_arquivo = (std::stoi(is_motorista_str) == 1);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "ERRO: Valor invalido para 'eh_motorista' no usuarios.txt para CPF " << cpf << ". Ignorando usuario. Erro: " << e.what() << std::endl;
                    continue;
                } catch (const std::out_of_range& e) {
                    std::cerr << "ERRO: Valor de 'eh_motorista' fora do limite para CPF " << cpf << " no usuarios.txt. Ignorando usuario. Erro: " << e.what() << std::endl;
                    continue;
                }

                Usuario* novo_usuario_carregado = nullptr;

                if (eh_motorista_do_arquivo) {
                    // Cuidado: Motorista só é válido se tiver CNH lida
                    if (!cnh_numero_lida.empty() && cnh_numero_lida != "0") { // Verifica se a CNH não está vazia ou "0"
                        novo_usuario_carregado = new Motorista(nome, cpf, telefone, data_nascimento, email, senha, gen,
                                                             vinculo_tipo, detalhe_vinculo, cnh_numero_lida);
                    } else {
                        std::cerr << "AVISO: Motorista com CPF " << cpf << " sem CNH valida no usuarios.txt. Carregado como Usuario comum." << std::endl;
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
            std::cout << "-> " << _usuarios.size() << " usuarios carregados do usuarios.txt." << std::endl;
        } else {
            std::cout << "-> Arquivo usuarios.txt nao encontrado. Nao ha usuarios para carregar." << std::endl;
        }

        // Carregar veiculos do veiculos.txt
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
                    std::cerr << "ERRO: Valor invalido para 'lugares' no veiculos.txt para placa " << placa_veic << ". Ignorando veiculo. Erro: " << e.what() << std::endl;
                    continue;
                } catch (const std::out_of_range& e) {
                    std::cerr << "ERRO: Valor de 'lugares' fora do limite para placa " << placa_veic << " no veiculos.txt. Ignorando veiculo. Erro: " << e.what() << std::endl;
                    continue;
                }

                Usuario* motorista_assoc = buscar_usuario_por_cpf(cpf_motorista_veic);
                if (motorista_assoc && motorista_assoc->is_motorista()) {
                    Motorista* m_ptr = dynamic_cast<Motorista*>(motorista_assoc);
                    if (m_ptr) {
                        // Antes de adicionar, verificar se o veículo já existe para o motorista
                        if (m_ptr->buscar_veiculo_por_placa(placa_veic) == nullptr) {
                            m_ptr->adicionar_veiculo(new Veiculo(placa_veic, marca_veic, modelo_veic, cor_veic, lugares_veic));
                        } else {
                            std::cerr << "AVISO: Veiculo com placa " << placa_veic << " ja existe para o motorista " << cpf_motorista_veic << ". Nao recarregado." << std::endl;
                        }
                    }
                } else {
                    std::cerr << "AVISO: Veiculo com placa " << placa_veic << " tem CPF de motorista (" << cpf_motorista_veic << ") nao encontrado ou nao e motorista valido. Veiculo nao carregado." << std::endl;
                }
            }
            arquivo_veiculos.close();
            std::cout << "-> Veiculos carregados do veiculos.txt." << std::endl;
        } else {
            std::cout << "-> Arquivo veiculos.txt nao encontrado. Nao ha veiculos para carregar." << std::endl;
        }

        std::ifstream arquivo_caronas("caronas.txt");
        if (arquivo_caronas.is_open()) {
            std::string linha, cpf_motorista, origem, destino, data, apenas_mulheres_str, placa_veiculo_carona;
            while (std::getline(arquivo_caronas, linha)) {
                std::stringstream ss(linha);
                std::getline(ss, cpf_motorista, ';');
                std::getline(ss, origem, ';');
                std::getline(ss, destino, ';');
                std::getline(ss, data, ';');
                std::getline(ss, apenas_mulheres_str, ';');
                std::getline(ss, placa_veiculo_carona); // Novo campo

                bool apenas_mulheres_lida;
                try {
                    apenas_mulheres_lida = (std::stoi(apenas_mulheres_str) == 1);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "ERRO: Valor invalido para 'apenas_mulheres' em caronas.txt para carona " << origem << "->" << destino << ". Ignorando carona. Erro: " << e.what() << std::endl;
                    continue;
                } catch (const std::out_of_range& e) {
                    std::cerr << "ERRO: Valor de 'apenas_mulheres' fora do limite para carona " << origem << "->" << destino << " em caronas.txt. Ignorando carona. Erro: " << e.what() << std::endl;
                    continue;
                }

                Usuario* motorista_ptr = buscar_usuario_por_cpf(cpf_motorista);
                if (motorista_ptr) {
                    if (!motorista_ptr->is_motorista()) {
                        std::cerr << "AVISO: Usuario " << motorista_ptr->get_nome() << " (CPF: " << cpf_motorista << ") listado como motorista em caronas.txt mas nao e um Motorista valido no sistema. Carona nao carregada." << std::endl;
                        continue;
                    }
                    Motorista* m_carona_ptr = dynamic_cast<Motorista*>(motorista_ptr);
                    Veiculo* veiculo_usado_carona = nullptr;
                    if (m_carona_ptr) {
                        veiculo_usado_carona = m_carona_ptr->buscar_veiculo_por_placa(placa_veiculo_carona);
                    } 
                    
                    if (!veiculo_usado_carona) {
                        std::cerr << "AVISO: Motorista " << motorista_ptr->get_nome() << " (CPF: " << cpf_motorista << ") para carona em caronas.txt nao tem o veiculo " << placa_veiculo_carona << " cadastrado. Carona nao carregada." << std::endl;
                        continue;
                    }

                    _caronas.push_back(CaronaFactory::criar_carona(origem, destino, data, motorista_ptr, veiculo_usado_carona, apenas_mulheres_lida, TipoCarona::AGENDADA));
                } else {
                    std::cerr << "AVISO: Motorista com CPF " << cpf_motorista << " para carona " << origem << "->" << destino << " nao encontrado. Carona nao carregada." << std::endl;
                }
            }
            arquivo_caronas.close();
            std::cout << "-> " << _caronas.size() << " caronas carregadas." << std::endl;
        } else {
            std::cout << "-> Arquivo caronas.txt nao encontrado. Nao ha caronas para carregar." << std::endl;
        }
    }

    void Sistema::salvar_dados_usuarios() {
        std::ofstream arquivo_usuarios("usuarios.txt", std::ios::trunc);
        if (!arquivo_usuarios.is_open()) {
            std::cerr << "ERRO: Nao foi possivel abrir o arquivo usuarios.txt para salvar dados." << std::endl;
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
                    arquivo_usuarios << "1;" // Flag: eh motorista
                                     << m_ptr->get_cnh_numero(); // CNH do motorista (ultimo campo da line se eh motorista)
                } else {
                    arquivo_usuarios << "0"; // Nao eh motorista (ou erro)
                    std::cerr << "AVISO: Usuario " << u->get_cpf() << " indicou ser motorista mas cast falhou. Nao salvando CNH." << std::endl;
                }
            } else {
                arquivo_usuarios << "0"; // Flag: nao eh motorista
            }
            arquivo_usuarios << std::endl;
        }
        arquivo_usuarios.close();
        std::cout << "-> " << _usuarios.size() << " usuarios salvos em usuarios.txt." << std::endl;
    }

    void Sistema::salvar_dados_veiculos() {
        std::ofstream arquivo_veiculos("veiculos.txt", std::ios::trunc);
        if (!arquivo_veiculos.is_open()) {
            std::cerr << "ERRO: Nao foi possivel abrir o arquivo veiculos.txt para salvar dados." << std::endl;
            return;
        }

        for (const auto& u : _usuarios) {
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
        std::cout << "-> Veiculos salvos em veiculos.txt." << std::endl;
    }

    Carona* Sistema::buscar_carona_por_id(int id) {
        for (auto& carona : _caronas) {
            if (carona.get_id() == id) {
                return &carona;
            }
        }
        return nullptr;
    }

    void Sistema::enviar_notificacao(Usuario* usuario, const std::string& mensagem) {
        if (usuario) {
            // AQUI VOCE PODE ADICIONAR A NOTIFICACAO REAL AO OBJETO USUARIO
            // usuario->adicionar_notificacao(Notificacao(mensagem));
            std::cout << "[NOTIFICACAO para " << usuario->get_nome() << "]: " << mensagem << std::endl;
        }
    }

    bool Sistema::pode_solicitar_carona(Usuario* passageiro, const Carona& carona) {
        if (passageiro == carona.get_motorista()) {
            std::cout << "Voce nao pode solicitar sua propria carona!" << std::endl;
            return false;
        }

        if (carona.get_vagas_disponiveis() <= 0) {
            std::cout << "Esta carona nao possui vagas disponiveis." << std::endl;
            return false;
        }

        // Verifica se ja existe uma solicitacao PENDENTE ou ACEITA para esta carona
        for (const auto& solicitacao : _solicitacoes) {
            if (solicitacao->get_passageiro() == passageiro &&
                solicitacao->get_carona()->get_id() == carona.get_id()) {
                if (solicitacao->get_status() == StatusSolicitacao::PENDENTE) {
                    std::cout << "Voce ja tem uma solicitacao PENDENTE para esta carona!" << std::endl;
                    return false;
                } else if (solicitacao->get_status() == StatusSolicitacao::ACEITA) {
                    std::cout << "Voce ja foi ACEITO nesta carona!" << std::endl;
                    return false;
                }
            }
        }
        return true;
    }
}