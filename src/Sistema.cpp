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
        std::string comando;
        while (true) {
            exibir_menu();
            std::cout << "> ";
            std::getline(std::cin, comando);
            if (comando == "sair" || std::cin.eof()) break;
            if (comando.empty()) continue;
            try {
                processar_comando(comando);
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            } catch (const std::bad_alloc& e) {
                std::cerr << "ERRO: Falha na alocacao de memoria: " << e.what() << std::endl;
            }
        }
    }

    void Sistema::processar_comando(const std::string& comando) {
        if (!_usuario_logado) {
            if (comando == "cadastro") fluxo_cadastro();
            else if (comando == "login") fluxo_login();
            else throw ComandoInvalidoException(comando);
        } else {
            if (comando == "logout") fluxo_logout();
            else if (comando == "perfil") _usuario_logado->imprimir_perfil();
            else if (comando == "oferecer_carona") fluxo_oferecer_carona();
            else if (comando == "buscar_caronas") fluxo_buscar_caronas();
            else if (comando == "solicitar_carona") fluxo_solicitar_carona();
            else if (comando == "gerenciar_solicitacoes") fluxo_gerenciar_solicitacoes();
            else if (comando == "status_caronas") fluxo_status_caronas();
            else if (comando == "cadastrar_veiculo") fluxo_cadastrar_veiculo();
            else throw ComandoInvalidoException(comando);
        }
    }

    void Sistema::fluxo_cadastro() {
        std::string cpf_digitado, telefone_digitado, email_digitado, senha_digitada;
        int gen_int;
        char deseja_motorista_char;
        bool deseja_ser_motorista = false;

        std::string nome_ufmg, data_nascimento_ufmg, vinculo_ufmg, detalhe_ufmg;
        std::string cnh_numero_digitado;
        
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
        std::cout << "Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): "; std::cin >> gen_int;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        Genero gen_digitado = static_cast<Genero>(gen_int);

        std::cout << "Deseja oferecer caronas (ser motorista)? (s/n): ";
        std::cin >> deseja_motorista_char;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        deseja_ser_motorista = (deseja_motorista_char == 's' || deseja_motorista_char == 'S');

        Usuario* novo_usuario = nullptr;

        if (deseja_ser_motorista) {
            std::cout << "--- Cadastro de Motorista ---" << std::endl;
            std::cout << "Numero da CNH: "; std::getline(std::cin, cnh_numero_digitado);
            novo_usuario = new Motorista(nome_ufmg, cpf_digitado, telefone_digitado, data_nascimento_ufmg,
                                         email_digitado, senha_digitada, gen_digitado, vinculo_ufmg,
                                         detalhe_ufmg, cnh_numero_digitado);
        } else {
            novo_usuario = new Usuario(nome_ufmg, cpf_digitado, telefone_digitado, data_nascimento_ufmg,
                                       email_digitado, senha_digitada, gen_digitado, vinculo_ufmg, detalhe_ufmg);
        }

        _usuarios.push_back(novo_usuario);
        salvar_dados_usuarios();
        std::cout << "Cadastro realizado com sucesso!" << std::endl;
        
        if (deseja_ser_motorista) {
            char cadastrar_primeiro_veiculo_char;
            std::cout << "Deseja cadastrar seu primeiro veiculo agora? (s/n): ";
            std::cin >> cadastrar_primeiro_veiculo_char;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (cadastrar_primeiro_veiculo_char == 's' || cadastrar_primeiro_veiculo_char == 'S') {
                _usuario_logado = novo_usuario;
                fluxo_cadastrar_veiculo();
                _usuario_logado = nullptr;
            }
        }
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
        std::cout << "--- Cadastrar Novo Veiculo ---" << std::endl;
        std::cout << "Placa: "; std::getline(std::cin, placa);
        if (motorista_logado->buscar_veiculo_por_placa(placa) != nullptr) {
            throw AppExcecao("Voce ja possui um veiculo com esta placa.");
        }

        std::cout << "Marca: "; std::getline(std::cin, marca);
        std::cout << "Modelo: "; std::getline(std::cin, modelo);
        std::cout << "Cor: "; std::getline(std::cin, cor);
        std::cout << "Total de lugares (com motorista): "; std::cin >> lugares;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        motorista_logado->adicionar_veiculo(new Veiculo(placa, marca, modelo, cor, lugares));
        salvar_dados_veiculos();
        std::cout << "Veiculo cadastrado com sucesso!" << std::endl;
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
        for (const auto& veic : motorista_logado->get_veiculos()) {
            if (veic) veic->exibir_info();
        }
        std::string placa_escolhida;
        std::cout << "Digite a placa do veiculo que deseja usar para esta carona: ";
        std::getline(std::cin, placa_escolhida);
        
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

    void Sistema::fluxo_buscar_caronas() {
        std::cout << "\n--- Caronas Disponiveis ---" << std::endl;
        if (_caronas.empty()) {
            std::cout << "Nenhuma carona disponivel no momento." << std::endl;
            return;
        }
        for (const auto& carona : _caronas) { carona.exibir_info(); }
    }

    void Sistema::exibir_menu() {
        if (_usuario_logado) {
            std::cout << "\nLogado como " << _usuario_logado->get_nome() << std::endl;
            if (_usuario_logado->is_motorista()) {
                std::cout << "Comandos: perfil, buscar_caronas, solicitar_carona, oferecer_carona, gerenciar_solicitacoes, status_caronas, cadastrar_veiculo, logout, sair" << std::endl;
            } else {
                std::cout << "Comandos: perfil, buscar_caronas, solicitar_carona, cadastrar_veiculo, logout, sair" << std::endl;
            }
        } else {
            std::cout << "\nComandos: cadastro, login, sair" << std::endl;
        }
    }

    Usuario* Sistema::buscar_usuario_por_cpf(const std::string& cpf) {
        for (const auto& u : _usuarios) {
            if (u->get_cpf() == cpf) return u;
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
                if (campos.size() == 11) { 
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
                    if (cnh_numero_lida.empty()) {
                        std::cerr << "AVISO: Motorista com CPF " << cpf << " sem CNH no usuarios.txt. Carregado como Usuario comum." << std::endl;
                        novo_usuario_carregado = new Usuario(nome, cpf, telefone, data_nascimento, email, senha, gen,
                                                             vinculo_tipo, detalhe_vinculo);
                    } else {
                        novo_usuario_carregado = new Motorista(nome, cpf, telefone, data_nascimento, email, senha, gen,
                                                             vinculo_tipo, detalhe_vinculo, cnh_numero_lida);
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
                        m_ptr->adicionar_veiculo(new Veiculo(placa_veic, marca_veic, modelo_veic, cor_veic, lugares_veic));
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
            std::string linha, cpf_motorista, origem, destino, data, apenas_mulheres_str;
            while (std::getline(arquivo_caronas, linha)) {
                std::stringstream ss(linha);
                std::getline(ss, cpf_motorista, ';');
                std::getline(ss, origem, ';');
                std::getline(ss, destino, ';');
                std::getline(ss, data, ';');
                std::getline(ss, apenas_mulheres_str);
                
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
                    if (m_carona_ptr && !m_carona_ptr->get_veiculos().empty()) {
                        veiculo_usado_carona = m_carona_ptr->get_veiculos()[0];
                    } else {
                        std::cerr << "AVISO: Motorista " << motorista_ptr->get_nome() << " (CPF: " << cpf_motorista << ") sem veiculos cadastrados para carona em caronas.txt. Carona nao carregada." << std::endl;
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

    void Sistema::fluxo_solicitar_carona() {
        std::cout << "\n--- Solicitar Carona ---" << std::endl;
        if (_caronas.empty()) {
            std::cout << "Nenhuma carona disponivel no momento." << std::endl;
            return;
        }

        std::cout << "Caronas disponiveis:" << std::endl;
        for (const auto& carona : _caronas) {
            if (carona.get_vagas_disponiveis() > 0) {
                carona.exibir_info();
            }
        }

        int id_carona;
        std::cout << "\nDigite o ID da carona que deseja solicitar: ";
        std::cin >> id_carona;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
        std::cin >> escolha;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (escolha == 0 || escolha > static_cast<int>(solicitacoes_motorista.size())) {
            return;
        }

        Solicitacao* solicitacao_escolhida = solicitacoes_motorista[escolha - 1];

        char resposta;
        std::cout << "Aceitar solicitacao? (s/n): ";
        std::cin >> resposta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (resposta == 's' || resposta == 'S') {
            solicitacao_escolhida->aceitar();
            solicitacao_escolhida->get_carona()->adicionar_passageiro(solicitacao_escolhida->get_passageiro());
            enviar_notificacao(solicitacao_escolhida->get_passageiro(),
                             "Sua solicitacao de carona foi ACEITA!");
            std::cout << "Solicitacao aceita!" << std::endl;
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
                std::cout << "Voce ja solicitou esta carona!" << std::endl;
                return false;
            }
        }
        return true;
    }
}