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
#include <chrono>
#include <iomanip>
#include <ctime>

namespace ufmg_carona {

    Sistema::Sistema() : _usuario_logado(nullptr) {
        carregar_dados_iniciais();
        remover_caronas_passadas();
        gerar_caronas_de_rotinas();
    }

    Sistema::~Sistema() {
        std::cout << "\nFinalizando o sistema..." << std::endl;
        salvar_dados_usuarios();
        salvar_dados_veiculos();
        salvar_dados_rotinas(); // Adicionado a chamada correta
        
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
                int comando_int = -1;
                bool is_numeric = true;

                try {
                    comando_int = std::stoi(comando_str);
                } catch (const std::invalid_argument&) {
                    is_numeric = false;
                }

                if (!_usuario_logado) {
                    if (is_numeric) {
                        if (comando_int == 1) { // Cadastro
                            fluxo_cadastro();
                        } else if (comando_int == 2) { // Login
                            fluxo_login();
                        } else if (comando_int == 0) { // Sair
                            std::cout << "Saindo do programa..." << std::endl;
                            break;
                        } else {
                            throw ComandoInvalidoException(comando_str);
                        }
                    } else if (comando_str == "cadastro") {
                        fluxo_cadastro();
                    } else if (comando_str == "login") {
                        fluxo_login();
                    } else if (comando_str == "sair") {
                        std::cout << "Saindo do programa..." << std::endl;
                        break;
                    }
                    else {
                        throw ComandoInvalidoException(comando_str);
                    }
                } else {
                    processar_comando_logado(comando_str);
                }
            } catch (const ComandoInvalidoException& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            } catch (const AppExcecao& e) {
                if (std::string(e.what()) == "Sair") {
                    break; 
                }
                std::cerr << "ERRO: " << e.what() << std::endl;
            } catch (const std::bad_alloc& e) {
                std::cerr << "ERRO: Falha na alocacao de memoria: " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "ERRO INESPERADO: " << e.what() << std::endl;
            }
        }
    }

    std::tm Sistema::parse_datetime_string(const std::string& dt_str) const {
        std::tm tm_struct = {};
        std::stringstream ss(dt_str);
        ss >> std::get_time(&tm_struct, "%d/%m/%Y %H:%M");
        if (ss.fail()) {
            std::cerr << "ERRO ao parsear data/hora: " << dt_str << std::endl;
        }
        return tm_struct;
    }

    std::string Sistema::get_current_datetime_string() const {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* local_tm = std::localtime(&now_c);
        std::stringstream ss;
        ss << std::put_time(local_tm, "%d/%m/%Y %H:%M");
        return ss.str();
    }

    bool Sistema::is_datetime_in_past(const std::string& dt_str) const {
        std::tm carona_tm = parse_datetime_string(dt_str);
        std::time_t carona_time = std::mktime(&carona_tm);

        auto now = std::chrono::system_clock::now();
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);

        if (carona_time == -1) {
             std::cerr << "AVISO: Data/hora invalida para comparacao: " << dt_str << std::endl;
             return true;
        }

        return std::difftime(current_time, carona_time) > 0;
    }

    void Sistema::remover_caronas_passadas() {
        auto new_end = std::remove_if(_caronas.begin(), _caronas.end(),
                                     [this](const Carona& carona) {
                                         return is_datetime_in_past(carona.get_data_hora());
                                     });
        if (new_end != _caronas.end()) {
            _caronas.erase(new_end, _caronas.end());
            std::cout << "-> Caronas expiradas removidas." << std::endl;
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
                std::cout << "Numero fora do intervalo permitido (" << min_val << "-" << max_val << ")."
                          << (min_val == 0 ? " Digite 0 para voltar." : "") << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return valor;
            }
        }
    }

    std::vector<DiaDaSemana> Sistema::coletar_dias_da_semana(const std::string& prompt) {
        std::vector<DiaDaSemana> dias_selecionados;
        std::string input_str;
        std::map<int, DiaDaSemana> int_to_dia = {
            {0, DiaDaSemana::DOMINGO}, {1, DiaDaSemana::SEGUNDA},
            {2, DiaDaSemana::TERCA}, {3, DiaDaSemana::QUARTA},
            {4, DiaDaSemana::QUINTA}, {5, DiaDaSemana::SEXTA},
            {6, DiaDaSemana::SABADO}
        };

        std::cout << prompt << " (0:Dom, 1:Seg, ..., 6:Sab. Digite 'todos' para todos os dias, ou numeros separados por virgula, ex: 1,3,5): ";
        std::getline(std::cin, input_str);

        if (input_str == "todos") {
            for (int i = 0; i <= 6; ++i) {
                dias_selecionados.push_back(int_to_dia[i]);
            }
        } else {
            std::stringstream ss(input_str);
            std::string segment;
            while(std::getline(ss, segment, ',')) {
                segment.erase(0, segment.find_first_not_of(" \t\n\r\f\v"));
                segment.erase(segment.find_last_not_of(" \t\n\r\f\v") + 1);

                if (segment.empty()) continue;

                try {
                    int dia_int = std::stoi(segment);
                    if (dia_int >= 0 && dia_int <= 6) {
                        dias_selecionados.push_back(int_to_dia[dia_int]);
                    } else {
                        std::cerr << "AVISO: Numero de dia invalido ignorado: " << dia_int << std::endl;
                    }
                } catch (const std::invalid_argument&) {
                    std::cerr << "AVISO: Entrada nao numerica ignorada: '" << segment << "'" << std::endl;
                } catch (const std::out_of_range&) {
                    std::cerr << "AVISO: Numero muito grande para dia da semana ignorado: '" << segment << "'" << std::endl;
                }
            }
            std::sort(dias_selecionados.begin(), dias_selecionados.end());
            dias_selecionados.erase(std::unique(dias_selecionados.begin(), dias_selecionados.end()), dias_selecionados.end());
        }
        return dias_selecionados;
    }

    void Sistema::exibir_menu_inicial_nao_logado() {
        std::cout << "\n--- Menu Principal ---" << std::endl;
        std::cout << "(1) Cadastro | (2) Login | (0) Sair" << std::endl;
    }

    void Sistema::processar_comando_logado(const std::string& comando_str) {
        int comando_int;
        try {
            comando_int = std::stoi(comando_str);
        } catch (const std::invalid_argument&) {
            throw ComandoInvalidoException(comando_str);
        }

        if (comando_int == 1) { // Perfil
            _usuario_logado->imprimir_perfil();
            fluxo_editar_perfil_ou_veiculos();
        } else if (comando_int == 2) { // Passageiro
            fluxo_passageiro_menu();
        } else if (comando_int == 3) { // Motorista
            fluxo_motorista_menu();
        } else if (comando_int == 4) { // Logout
            fluxo_logout();
        } else if (comando_int == 5) { // Sair do programa
            std::cout << "Saindo do programa..." << std::endl;
            throw AppExcecao("Sair");
        } else {
            throw ComandoInvalidoException(comando_str);
        }
    }

    void Sistema::exibir_menu() {
        // Este metodo nao e mais usado diretamente como um dispatcher.
        // As chamadas foram substituidas por exibir_menu_inicial_nao_logado()
        // e exibir_menu_logado() dentro de 'executar()'.
    }

    void Sistema::exibir_menu_logado() {
        std::cout << "\nLogado como " << _usuario_logado->get_nome() << std::endl;
        std::cout << "(1) Perfil | (2) Passageiro | (3) Motorista | (4) Logout | (5) Sair" << std::endl;
    }

    void Sistema::exibir_menu_passageiro() {
        std::cout << "\n--- Menu Passageiro ---" << std::endl;
        std::cout << "(1) Solicitar Carona | (2) Status das Minhas Solicitacoes | (0) Voltar" << std::endl;
    }

    void Sistema::exibir_menu_motorista() {
        std::cout << "\n--- Menu Motorista ---" << std::endl;
        std::cout << "(1) Oferecer Carona | (2) Gerenciar Solicitacoes | (3) Cadastrar Veiculo | (4) Gerenciar Rotinas | (0) Voltar" << std::endl;
    }

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
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            }
        } while (comando != 0);
    }

    void Sistema::fluxo_motorista_menu() {
        if (!_usuario_logado->is_motorista()) {
            std::cout << "Voce nao esta registrado como motorista." << std::endl;
            char deseja_se_tornar;
            std::cout << "Deseja se tornar um motorista? (s/n): ";
            std::cin >> deseja_se_tornar;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (deseja_se_tornar == 's' || deseja_se_tornar == 'S') {
                fluxo_tornar_motorista();
            }
            return;
        }

        int comando;
        do {
            exibir_menu_motorista();
            comando = coletar_int_input("> ", 0, 4);

            try {
                if (comando == 1) {
                    fluxo_oferecer_carona();
                } else if (comando == 2) {
                    fluxo_gerenciar_solicitacoes();
                } else if (comando == 3) {
                    fluxo_cadastrar_veiculo();
                } else if (comando == 4) {
                    fluxo_gerenciar_rotinas();
                }
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            }
        } while (comando != 0);
    }

    void Sistema::fluxo_editar_perfil_ou_veiculos() {
        int sub_comando_perfil;
        do {
            std::cout << "\n--- Menu Perfil ---" << std::endl;
            _usuario_logado->imprimir_perfil();
            std::cout << "Escolha o campo para editar:" << std::endl;
            std::cout << "(1) Editar Perfil | (2) Gerenciar Veiculos (Motorista) | (0) Voltar" << std::endl;
            sub_comando_perfil = coletar_int_input("> ", 0, 2);

            try {
                if (sub_comando_perfil == 1) {
                    fluxo_editar_perfil();
                } else if (sub_comando_perfil == 2) {
                    if (_usuario_logado->is_motorista()) {
                        fluxo_gerenciar_veiculos();
                    } else {
                        std::cout << "Voce nao e motorista para gerenciar veiculos." << std::endl;
                    }
                }
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            }
        } while (sub_comando_perfil != 0);
    }

    void Sistema::fluxo_tornar_motorista() {
        if (_usuario_logado->is_motorista()) {
            std::cout << "Voce ja e um motorista cadastrado." << std::endl;
            return;
        }

        std::string cnh_numero_digitado;
        std::cout << "\n--- Cadastro para Motorista ---" << std::endl;
        std::cout << "Numero da CNH: "; std::getline(std::cin, cnh_numero_digitado);

        auto it = std::find(_usuarios.begin(), _usuarios.end(), _usuario_logado);
        if (it == _usuarios.end()) {
            std::cerr << "ERRO INTERNO: Usuario logado nao encontrado na lista de usuarios." << std::endl;
            return;
        }

        std::string nome = _usuario_logado->get_nome();
        std::string cpf = _usuario_logado->get_cpf();
        std::string telefone = _usuario_logado->get_telefone();
        std::string data_nascimento = _usuario_logado->get_data_nascimento();
        std::string email = _usuario_logado->get_email();
        std::string senha = _usuario_logado->get_senha();
        Genero genero = _usuario_logado->get_genero();
        std::string vinculo_tipo = _usuario_logado->get_vinculo_raw();
        std::string detalhe_vinculo = _usuario_logado->get_detalhe_vinculo();

        Motorista* novo_motorista = new Motorista(nome, cpf, telefone, data_nascimento, email, senha, genero,
                                                  vinculo_tipo, detalhe_vinculo, cnh_numero_digitado);
        
        delete _usuario_logado; 

        *it = novo_motorista;
        
        _usuario_logado = novo_motorista;

        salvar_dados_usuarios();

        std::cout << "Parabens! Voce agora e um motorista cadastrado." << std::endl;

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
        do {
            std::cout << "\n--- Editar Perfil ---" << std::endl;
            _usuario_logado->imprimir_perfil();
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
                case 4: { // Genero
                    int gen_int = coletar_int_input("Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): ", 0, 3);
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
            if (escolha != 0) {
                salvar_dados_usuarios();
            }
        } while (escolha != 0);
    }

    void Sistema::fluxo_gerenciar_veiculos() {
        if (!_usuario_logado->is_motorista()) {
            std::cout << "Voce nao e motorista para gerenciar veiculos." << std::endl;
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
        } while (comando != 0);
    }

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
        do {
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
                    novo_int_valor = coletar_int_input("", 2, 99);
                    veiculo_para_editar->set_lugares(novo_int_valor);
                    break;
                case 0:
                    std::cout << "Voltando..." << std::endl;
                    break;
                default:
                    std::cout << "Opcao invalida." << std::endl;
                    break;
            }
            if (escolha != 0) {
                salvar_dados_veiculos();
                std::cout << "Veiculo atualizado com sucesso!" << std::endl;
            }
        } while (escolha != 0);
    }

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
                salvar_dados_veiculos();
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
                    cadastrar_outro = 'n';
                }
                continue;
            }

            std::cout << "Marca: "; std::getline(std::cin, marca);
            std::cout << "Modelo: "; std::getline(std::cin, modelo);
            std::cout << "Cor: "; std::getline(std::cin, cor);
            std::cout << "Total de lugares (com motorista): ";
            lugares = coletar_int_input("", 2, 99);
            
            motorista_logado->adicionar_veiculo(new Veiculo(placa, marca, modelo, cor, lugares));
            salvar_dados_veiculos();
            std::cout << "Veiculo cadastrado com sucesso!" << std::endl;

            std::cout << "Deseja cadastrar outro veiculo? (s/n): ";
            std::cin >> cadastrar_outro;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    void Sistema::fluxo_gerenciar_rotinas() {
        if (!_usuario_logado->is_motorista()) {
            std::cout << "Voce nao e motorista para gerenciar rotinas." << std::endl;
            return;
        }
        Motorista* motorista_logado = dynamic_cast<Motorista*>(_usuario_logado);
        if (!motorista_logado) return;

        int comando;
        do {
            std::cout << "\n--- Gerenciar Minhas Rotinas ---" << std::endl;
            fluxo_visualizar_rotinas(motorista_logado);

            std::cout << "\n(1) Adicionar Nova Rotina | (2) Excluir Rotina | (0) Voltar" << std::endl;
            comando = coletar_int_input("> ", 0, 2);

            try {
                if (comando == 1) {
                    fluxo_adicionar_rotina(motorista_logado);
                } else if (comando == 2) {
                    fluxo_excluir_rotina(motorista_logado);
                }
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            }
        } while (comando != 0);
    }

    void Sistema::fluxo_adicionar_rotina(Motorista* motorista) {
        std::string origem, destino, horario;
        std::vector<DiaDaSemana> dias_rotina;
        char apenas_mulheres_char;
        bool apenas_mulheres_rotina_bool;

        std::cout << "\n--- Adicionar Nova Rotina ---" << std::endl;
        
        dias_rotina = coletar_dias_da_semana("Selecione os dias da semana para a rotina");

        std::cout << "Horario de Saida (HH:MM): "; std::getline(std::cin, horario);
        std::cout << "Local de Saida Padrao: "; std::getline(std::cin, origem);
        std::cout << "Destino Final: "; std::getline(std::cin, destino);

        std::string data_hora_teste = get_current_datetime_string().substr(0, 10) + " " + horario;
        if (is_datetime_in_past(data_hora_teste) && !dias_rotina.empty()) {
            bool is_future_day_selected = false;
            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm* local_tm = std::localtime(&now_c);
            int current_wday = local_tm->tm_wday;

            for(DiaDaSemana dia : dias_rotina) {
                if (static_cast<int>(dia) > current_wday || 
                    (static_cast<int>(dia) == current_wday && !is_datetime_in_past(data_hora_teste))) {
                    is_future_day_selected = true;
                    break;
                }
            }
            if (!is_future_day_selected) {
                 std::cout << "ERRO: O horario (" << horario << ") para a rotina no(s) dia(s) selecionado(s) ja passou." << std::endl;
                 return;
            }
        }

        if (motorista->get_veiculos().empty()) {
            std::cout << "Voce nao possui veiculos cadastrados. Cadastre um veiculo antes de criar rotinas." << std::endl;
            return;
        }
        std::cout << "Seus veiculos cadastrados:" << std::endl;
        for (size_t i = 0; i < motorista->get_veiculos().size(); ++i) {
            std::cout << "[" << (i + 1) << "] ";
            motorista->get_veiculos()[i]->exibir_info();
        }
        int indice_veiculo = coletar_int_input("Digite o numero do veiculo a ser usado nesta rotina: ", 1, motorista->get_veiculos().size());
        Veiculo* veiculo_para_rotina = motorista->buscar_veiculo_por_indice(indice_veiculo - 1);
        
        if (!veiculo_para_rotina) {
            std::cout << "Selecao de veiculo invalida." << std::endl;
            return;
        }
        std::string placa_veiculo = veiculo_para_rotina->get_placa();

        if (_usuario_logado->get_genero() == Genero::FEMININO) {
            std::cout << "Apenas para mulheres? (s/n): ";
            std::cin >> apenas_mulheres_char;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            apenas_mulheres_rotina_bool = (apenas_mulheres_char == 's' || apenas_mulheres_char == 'S');
        } else {
            apenas_mulheres_rotina_bool = false;
            std::cout << "Nota: A opcao 'Apenas para mulheres' esta disponivel apenas para usuarias do genero feminino." << std::endl;
        }

        Rotina nova_rotina(dias_rotina, horario, origem, destino, placa_veiculo, apenas_mulheres_rotina_bool);
        motorista->adicionar_rotina(nova_rotina);
        salvar_dados_rotinas();
        std::cout << "Rotina adicionada com sucesso!" << std::endl;
    }

    void Sistema::fluxo_visualizar_rotinas(Motorista* motorista) {
        const auto& rotinas = motorista->get_rotinas();
        if (rotinas.empty()) {
            std::cout << "Nenhuma rotina cadastrada." << std::endl;
            return;
        }

        std::cout << "\n--- Suas Rotinas Cadastradas ---" << std::endl;
        std::map<DiaDaSemana, std::string> dias_da_semana_map = {
            {DiaDaSemana::DOMINGO, "Dom"}, {DiaDaSemana::SEGUNDA, "Seg"},
            {DiaDaSemana::TERCA, "Ter"}, {DiaDaSemana::QUARTA, "Qua"},
            {DiaDaSemana::QUINTA, "Qui"}, {DiaDaSemana::SEXTA, "Sex"},
            {DiaDaSemana::SABADO, "Sab"}
        };

        for (size_t i = 0; i < rotinas.size(); ++i) {
            const Rotina& rotina = rotinas[i];
            std::cout << "[" << (i + 1) << "] ";
            
            std::cout << "Dias: ";
            const std::vector<DiaDaSemana>& dias_da_rotina = rotina.get_dias();
            if (dias_da_rotina.empty()) {
                std::cout << "Nenhum";
            } else if (dias_da_rotina.size() == 7) {
                std::cout << "Todos";
            } else {
                for (size_t j = 0; j < dias_da_rotina.size(); ++j) {
                    std::cout << dias_da_semana_map[dias_da_rotina[j]];
                    if (j < dias_da_rotina.size() - 1) {
                        std::cout << ",";
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

    void Sistema::fluxo_excluir_rotina(Motorista* motorista) {
        if (motorista->get_rotinas().empty()) {
            std::cout << "Nenhuma rotina para excluir." << std::endl;
            return;
        }

        fluxo_visualizar_rotinas(motorista);

        int indice_escolhido = coletar_int_input("Digite o numero da rotina para excluir (0 para voltar): ", 0, motorista->get_rotinas().size());

        if (indice_escolhido == 0) {
            std::cout << "Exclusao cancelada." << std::endl;
            return;
        }

        char confirmar;
        std::cout << "Tem certeza que deseja excluir esta rotina? (s/n): ";
        std::cin >> confirmar;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (confirmar == 's' || confirmar == 'S') {
            if (motorista->remover_rotina(indice_escolhido - 1)) {
                salvar_dados_rotinas();
                std::cout << "Rotina excluida com sucesso!" << std::endl;
            } else {
                std::cout << "Erro ao excluir rotina. Indice invalido." << std::endl;
            }
        } else {
            std::cout << "Exclusao cancelada." << std::endl;
        }
    }

    void Sistema::gerar_caronas_de_rotinas() {
        std::cout << "\nVerificando rotinas para criar caronas..." << std::endl;

        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* local_tm = std::localtime(&now_c);
        int dia_semana_atual_int = local_tm->tm_wday;
        DiaDaSemana dia_semana_atual = static_cast<DiaDaSemana>(dia_semana_atual_int);

        std::stringstream ss_data_atual;
        ss_data_atual << std::put_time(local_tm, "%d/%m/%Y");
        std::string data_atual_str = ss_data_atual.str();

        for (Usuario* u : _usuarios) {
            if (u->is_motorista()) {
                Motorista* motorista_ptr = dynamic_cast<Motorista*>(u);
                if (motorista_ptr) {
                    for (const auto& rotina : motorista_ptr->get_rotinas()) {
                        if (rotina.aplica_se_ao_dia(dia_semana_atual)) {
                            std::string data_hora_completa = data_atual_str + " " + rotina.get_horario_saida();

                            if (is_datetime_in_past(data_hora_completa)) {
                                std::cout << "  AVISO: Rotina de " << motorista_ptr->get_nome() << " para " << data_hora_completa << " ignorada. Horario ja passou para hoje." << std::endl;
                                continue;
                            }

                            Veiculo* veiculo_rotina = motorista_ptr->buscar_veiculo_por_placa(rotina.get_placa_veiculo_usado());
                            
                            if (!veiculo_rotina) {
                                std::cerr << "AVISO: Rotina de motorista " << motorista_ptr->get_nome() << " tem veiculo "
                                          << rotina.get_placa_veiculo_usado() << " nao encontrado. Carona nao gerada." << std::endl;
                                continue;
                            }

                            bool carona_ja_criada = false;
                            for (const auto& carona : _caronas) {
                                if (carona.get_motorista() == motorista_ptr &&
                                    carona.get_origem() == rotina.get_local_saida_padrao() &&
                                    carona.get_destino() == rotina.get_destino_final() &&
                                    carona.get_data_hora() == data_hora_completa &&
                                    carona.get_veiculo_usado() == veiculo_rotina) {
                                    carona_ja_criada = true;
                                    break;
                                }
                            }

                            if (!carona_ja_criada) {
                                bool apenas_mulheres_rotina = rotina.get_apenas_mulheres(); 

                                Carona nova_carona = CaronaFactory::criar_carona(
                                    rotina.get_local_saida_padrao(),
                                    rotina.get_destino_final(),
                                    data_hora_completa,
                                    motorista_ptr,
                                    veiculo_rotina,
                                    apenas_mulheres_rotina,
                                    TipoCarona::AGENDADA
                                );
                                _caronas.push_back(nova_carona);
                                std::cout << "  Carona (ID: " << nova_carona.get_id() << ") gerada automaticamente de rotina para "
                                          << motorista_ptr->get_nome() << " (" << data_hora_completa << ") usando veiculo "
                                          << rotina.get_placa_veiculo_usado() << "." << std::endl;
                            }
                        }
                    }
                }
            }
        }
        std::cout << "Verificacao de rotinas concluida." << std::endl;
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
        int indice_veiculo = coletar_int_input("Digite o numero do veiculo que deseja usar para esta carona: ", 1, motorista_logado->get_veiculos().size());
        Veiculo* veiculo_selecionado = motorista_logado->buscar_veiculo_por_indice(indice_veiculo - 1);
        
        if (!veiculo_selecionado) {
            std::cout << "Selecao de veiculo invalida." << std::endl;
            return;
        }

        std::string origem, destino, data;
        char apenas_mulheres_char;
        bool apenas_mulheres;

        std::cout << "Origem: "; std::getline(std::cin, origem);
        std::cout << "Destino: "; std::getline(std::cin, destino);
        std::cout << "Data e Hora (ex: DD/MM/AAAA HH:MM): "; std::getline(std::cin, data);
        
        if (is_datetime_in_past(data)) {
            std::cout << "ERRO: Nao e possivel oferecer caronas com data e hora no passado." << std::endl;
            return;
        }

        if (_usuario_logado->get_genero() == Genero::FEMININO) {
            std::cout << "Apenas para mulheres? (s/n): ";
            std::cin >> apenas_mulheres_char;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            apenas_mulheres = (apenas_mulheres_char == 's' || apenas_mulheres_char == 'S');
        } else {
            apenas_mulheres = false;
            std::cout << "Nota: A opcao 'Apenas para mulheres' esta disponivel apenas para usuarias do genero feminino." << std::endl;
        }
        
        Carona nova_carona = CaronaFactory::criar_carona(origem, destino, data, _usuario_logado, veiculo_selecionado, apenas_mulheres, TipoCarona::AGENDADA);
        _caronas.push_back(nova_carona);
        std::cout << "Carona (ID: " << nova_carona.get_id() << ") criada com sucesso usando o veiculo " << veiculo_selecionado->get_placa() << "!" << std::endl;
    }

    void Sistema::fluxo_solicitar_carona() {
        std::cout << "\n--- Solicitar Carona ---" << std::endl;
        remover_caronas_passadas();

        std::string filtro_origem, filtro_destino, filtro_data, filtro_hora;
        int filtro_genero_int = -1;

        char usar_filtro;
        std::cout << "Deseja usar filtros para buscar caronas? (s/n): ";
        std::cin >> usar_filtro;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (usar_filtro == 's' || usar_filtro == 'S') {
            std::cout << "Filtro de Origem (deixe em branco para ignorar): "; std::getline(std::cin, filtro_origem);
            std::cout << "Filtro de Destino (deixe em branco para ignorar): "; std::getline(std::cin, filtro_destino);
            std::cout << "Filtro de Data (DD/MM/AAAA, deixe em branco para ignorar): "; std::getline(std::cin, filtro_data);
            std::cout << "Filtro de Hora (HH:MM, deixe em branco para ignorar): "; std::getline(std::cin, filtro_hora);
            
            if (_usuario_logado->get_genero() == Genero::FEMININO) {
                std::cout << "Filtro de Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar, -1:Sem Filtro): ";
                filtro_genero_int = coletar_int_input("", -1, 3);
            } else {
                filtro_genero_int = -1;
                std::cout << "Nota: A opcao 'Filtro de Genero' esta disponivel apenas para usuarias do genero feminino. Ignorando filtro de genero." << std::endl;
            }
        }

        std::vector<Carona*> caronas_filtradas;
        for (auto& carona : _caronas) {
            // NOVO: Nao mostrar carona do proprio usuario
            if (carona.get_motorista() == _usuario_logado) {
                continue; 
            }

            if (is_datetime_in_past(carona.get_data_hora())) {
                 continue;
            }

            bool atende_filtro_origem = filtro_origem.empty() || carona.get_origem().find(filtro_origem) != std::string::npos;
            bool atende_filtro_destino = filtro_destino.empty() || carona.get_destino().find(filtro_destino) != std::string::npos;
            
            bool atende_filtro_data = filtro_data.empty() || carona.get_data_hora().find(filtro_data) != std::string::npos;
            bool atende_filtro_hora = filtro_hora.empty() || carona.get_data_hora().find(filtro_hora) != std::string::npos;

            bool atende_filtro_genero = true;
            if (filtro_genero_int != -1) {
                if (carona.get_apenas_mulheres() && _usuario_logado->get_genero() != Genero::FEMININO) {
                    atende_filtro_genero = false;
                } else if (carona.get_apenas_mulheres() && static_cast<Genero>(filtro_genero_int) != Genero::FEMININO) {
                    atende_filtro_genero = false;
                }
                else if (!carona.get_apenas_mulheres() && static_cast<Genero>(filtro_genero_int) != Genero::PREFIRO_NAO_INFORMAR) {
                    if (static_cast<Genero>(filtro_genero_int) != carona.get_motorista()->get_genero()) {
                         // A logica e mais complexa e depende da interpretacao exata.
                         // Deixei mais aberta para customizacao
                    }
                }
            }


            if (carona.get_vagas_disponiveis() > 0 &&
                atende_filtro_origem && atende_filtro_destino &&
                atende_filtro_data && atende_filtro_hora &&
                atende_filtro_genero) {
                caronas_filtradas.push_back(&carona);
            }
        }

        if (_caronas.empty() || caronas_filtradas.empty()) {
            std::cout << "Nenhuma carona disponivel no momento ou com os filtros especificados." << std::endl;
            return;
        }

        std::cout << "\nCaronas disponiveis (filtradas):" << std::endl;
        for (const auto& carona_ptr : caronas_filtradas) {
            carona_ptr->exibir_info();
        }

        int id_carona;
        std::cout << "\nDigite o ID da carona que deseja solicitar (0 para voltar): ";
        id_carona = coletar_int_input("", 0, std::numeric_limits<int>::max());

        if (id_carona == 0) return;

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
                solicitacao->get_status() == StatusSolicitacao::PENDENTE &&
                !is_datetime_in_past(solicitacao->get_carona()->get_data_hora())) {
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
            if (solicitacao_escolhida->get_carona()->get_vagas_disponiveis() > 0) {
                solicitacao_escolhida->aceitar();
                solicitacao_escolhida->get_carona()->adicionar_passageiro(solicitacao_escolhida->get_passageiro());
                enviar_notificacao(solicitacao_escolhida->get_passageiro(),
                                "Sua solicitacao de carona foi ACEITA!");
                std::cout << "Solicitacao aceita!" << std::endl;
            } else {
                solicitacao_escolhida->recusar();
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
            if (is_datetime_in_past(solicitacao->get_carona()->get_data_hora())) {
                std::cout << "OBS: Esta carona ja ocorreu ou o horario de partida ja passou." << std::endl;
            }
        }
    }

    Usuario* Sistema::buscar_usuario_por_cpf(const std::string& cpf) {
        for (const auto& u : _usuarios) {
            if (u->get_cpf() == cpf) return u;
        }
        return nullptr;
    }
    
    Veiculo* Sistema::buscar_veiculo_por_placa_motorista(Motorista* motorista, const std::string& placa) {
        if (!motorista) return nullptr;
        for (Veiculo* v : motorista->get_veiculos()) {
            if (v && v->get_placa() == placa) {
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
                if (campos.size() >= 11) {
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
                    if (!cnh_numero_lida.empty() && cnh_numero_lida != "0") {
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
                        if (m_ptr->buscar_veiculo_por_placa(placa_veic) == nullptr) { // Verificar se ja existe
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
                std::getline(ss, placa_veiculo_carona);

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
        carregar_dados_rotinas(); // Adicionado para carregar rotinas
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
                    arquivo_usuarios << "1;"
                                     << m_ptr->get_cnh_numero();
                } else {
                    arquivo_usuarios << "0";
                    std::cerr << "AVISO: Usuario " << u->get_cpf() << " indicou ser motorista mas cast falhou. Nao salvando CNH." << std::endl;
                }
            } else {
                arquivo_usuarios << "0";
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

    void Sistema::salvar_dados_rotinas() { // Implementacao adicionada
        std::ofstream arquivo_rotinas("rotinas.txt", std::ios::trunc);
        if (!arquivo_rotinas.is_open()) {
            std::cerr << "ERRO: Nao foi possivel abrir o arquivo rotinas.txt para salvar dados." << std::endl;
            return;
        }

        int rotinas_salvas = 0;
        for (const auto& u : _usuarios) {
            if (u->is_motorista()) {
                Motorista* m_ptr = dynamic_cast<Motorista*>(u);
                if (m_ptr) {
                    for (const auto& rotina : m_ptr->get_rotinas()) {
                        arquivo_rotinas << u->get_cpf() << ";";
                        const std::vector<DiaDaSemana>& dias = rotina.get_dias();
                        for (size_t i = 0; i < dias.size(); ++i) {
                            arquivo_rotinas << static_cast<int>(dias[i]);
                            if (i < dias.size() - 1) {
                                arquivo_rotinas << ",";
                            }
                        }
                        arquivo_rotinas << ";"
                                        << rotina.get_horario_saida() << ";"
                                        << rotina.get_local_saida_padrao() << ";"
                                        << rotina.get_destino_final() << ";"
                                        << rotina.get_placa_veiculo_usado() << ";"
                                        << (rotina.get_apenas_mulheres() ? "1" : "0") << std::endl;
                        rotinas_salvas++;
                    }
                }
            }
        }
        arquivo_rotinas.close();
        std::cout << "-> " << rotinas_salvas << " rotinas salvas em rotinas.txt." << std::endl;
    }

    void Sistema::carregar_dados_rotinas() { // Implementacao adicionada
        std::ifstream arquivo_rotinas("rotinas.txt");
        if (arquivo_rotinas.is_open()) {
            std::string linha;
            int rotinas_carregadas = 0;
            while (std::getline(arquivo_rotinas, linha)) {
                std::stringstream ss(linha);
                std::string cpf_motorista_rotina, dias_str, horario, origem, destino, placa_veiculo_str, apenas_mulheres_str;

                std::getline(ss, cpf_motorista_rotina, ';');
                std::getline(ss, dias_str, ';');
                std::getline(ss, horario, ';');
                std::getline(ss, origem, ';');
                std::getline(ss, destino, ';');
                std::getline(ss, placa_veiculo_str, ';');
                std::getline(ss, apenas_mulheres_str);

                std::vector<DiaDaSemana> dias_lidos;
                std::stringstream ss_dias(dias_str);
                std::string dia_token;
                while (std::getline(ss_dias, dia_token, ',')) {
                    try {
                        dias_lidos.push_back(static_cast<DiaDaSemana>(std::stoi(dia_token)));
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "ERRO: Valor invalido para dia da semana em rotinas.txt para motorista " << cpf_motorista_rotina << ". Ignorando dia. Erro: " << e.what() << std::endl;
                    } catch (const std::out_of_range& e) {
                        std::cerr << "ERRO: Valor de dia da semana fora do limite em rotinas.txt para motorista " << cpf_motorista_rotina << ". Ignorando dia. Erro: " << e.what() << std::endl;
                    }
                }

                bool apenas_mulheres_lida;
                try {
                    apenas_mulheres_lida = (std::stoi(apenas_mulheres_str) == 1);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "ERRO: Valor invalido para 'apenas_mulheres' em rotinas.txt para motorista " << cpf_motorista_rotina << ". Ignorando rotina. Erro: " << e.what() << std::endl;
                    continue;
                } catch (const std::out_of_range& e) {
                    std::cerr << "ERRO: Valor de 'apenas_mulheres' fora do limite para motorista " << cpf_motorista_rotina << " em rotinas.txt. Ignorando rotina. Erro: " << e.what() << std::endl;
                    continue;
                }

                Usuario* motorista_assoc = buscar_usuario_por_cpf(cpf_motorista_rotina);
                if (motorista_assoc && motorista_assoc->is_motorista()) {
                    Motorista* m_ptr = dynamic_cast<Motorista*>(motorista_assoc);
                    if (m_ptr) {
                        m_ptr->adicionar_rotina(Rotina(dias_lidos, horario, origem, destino, placa_veiculo_str, apenas_mulheres_lida));
                        rotinas_carregadas++;
                    }
                } else {
                    std::cerr << "AVISO: Motorista com CPF " << cpf_motorista_rotina << " para rotina " << origem << "->" << destino << " nao encontrado ou nao e motorista valido. Rotina nao carregada." << std::endl;
                }
            }
            arquivo_rotinas.close();
            std::cout << "-> " << rotinas_carregadas << " rotinas carregados." << std::endl;
        } else {
            std::cout << "-> Arquivo rotinas.txt nao encontrado. Nao ha rotinas para carregar." << std::endl;
        }
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