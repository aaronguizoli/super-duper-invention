#include "Sistema.hpp"
#include "Excecoes.hpp"
#include "Aluno.hpp"
#include "Funcionario.hpp"
#include "CaronaFactory.hpp"
#include "Veiculo.hpp"
#include "Pagamento.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits> // Para std::numeric_limits

namespace ufmg_carona {

    Sistema::Sistema() : _usuario_logado(nullptr) {
        carregar_dados_iniciais();
    }

    // A definição do destrutor está aqui, no arquivo .cpp, e não mais no .hpp
    Sistema::~Sistema() {
        std::cout << "\nFinalizando o sistema..." << std::endl;
        // salvar_dados(); // Futura implementação para salvar o estado atual
    }

    void Sistema::executar() {
        std::cout << "== Sistema de Caronas UFMG iniciado ==" << std::endl;
        std::string comando;
        while (true) {
            exibir_menu();
            std::cout << "> ";
            std::getline(std::cin, comando);

            if (comando == "sair" || std::cin.eof()) {
                break;
            }
            if (comando.empty()) {
                continue;
            }
            try {
                processar_comando(comando);
            } catch (const AppExcecao& e) {
                std::cerr << "ERRO: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "ERRO: Ocorreu um erro inesperado." << std::endl;
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
            else if (comando == "cadastrar_veiculo") fluxo_cadastrar_veiculo();
            else throw ComandoInvalidoException(comando);
        }
    }

    void Sistema::fluxo_cadastro() {
        std::string nome, cpf, email, senha, vinculo, detalhe;
        int gen_int;

        std::cout << "--- Cadastro ---" << std::endl;
        std::cout << "Nome completo: "; std::getline(std::cin, nome);
        std::cout << "CPF: "; std::getline(std::cin, cpf);
        std::cout << "Email: "; std::getline(std::cin, email);
        std::cout << "Senha: "; std::getline(std::cin, senha);
        std::cout << "Genero (0:Masc, 1:Fem, 2:Outro): "; std::cin >> gen_int;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Vinculo (aluno/funcionario): "; std::getline(std::cin, vinculo);

        if (buscar_usuario_por_cpf(cpf)) {
            throw AppExcecao("CPF ja cadastrado.");
        }

        Genero gen = static_cast<Genero>(gen_int);
        if (vinculo == "aluno") {
            std::cout << "Curso: "; std::getline(std::cin, detalhe);
            _usuarios.push_back(std::make_shared<Aluno>(nome, cpf, email, senha, gen, detalhe));
        } else if (vinculo == "funcionario") {
            std::cout << "Setor: "; std::getline(std::cin, detalhe);
            _usuarios.push_back(std::make_shared<Funcionario>(nome, cpf, email, senha, gen, detalhe));
        } else {
            throw AppExcecao("Vinculo invalido.");
        }
        std::cout << "Cadastro realizado com sucesso!" << std::endl;
    }

    void Sistema::fluxo_login() {
        std::string cpf, senha;
        std::cout << "--- Login ---" << std::endl;
        std::cout << "CPF: "; std::getline(std::cin, cpf);
        std::cout << "Senha: "; std::getline(std::cin, senha);

        auto u = buscar_usuario_por_cpf(cpf);
        
        if (u && u->verificar_senha(senha)) {
            _usuario_logado = u;
            std::cout << "Login bem-sucedido!" << std::endl;
        } else {
            throw AutenticacaoFalhouException();
        }
    }
    
    void Sistema::fluxo_logout() {
        _usuario_logado = nullptr;
        std::cout << "Logout efetuado." << std::endl;
    }

    void Sistema::fluxo_cadastrar_veiculo() {
        if (_usuario_logado->is_motorista()) {
            throw AppExcecao("Voce ja possui um veiculo cadastrado.");
        }
        std::string placa, marca, modelo, cor;
        int lugares;
        std::cout << "--- Cadastro de Veiculo ---" << std::endl;
        std::cout << "Placa: "; std::getline(std::cin, placa);
        std::cout << "Marca: "; std::getline(std::cin, marca);
        std::cout << "Modelo: "; std::getline(std::cin, modelo);
        std::cout << "Cor: "; std::getline(std::cin, cor);
        std::cout << "Total de lugares (com motorista): "; std::cin >> lugares;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        _usuario_logado->cadastrar_veiculo(Veiculo(placa, marca, modelo, cor, lugares));
        std::cout << "Veiculo cadastrado com sucesso!" << std::endl;
    }

    void Sistema::fluxo_oferecer_carona() {
        if (!_usuario_logado->is_motorista()) {
            throw AppExcecao("Cadastre um veiculo para oferecer caronas.");
        }
        std::string origem, destino, data;
        char apenas_mulheres_char;
        std::cout << "--- Oferecer Carona ---" << std::endl;
        std::cout << "Origem: "; std::getline(std::cin, origem);
        std::cout << "Destino: "; std::getline(std::cin, destino);
        std::cout << "Data e Hora (ex: 18/06/2025 08:00): "; std::getline(std::cin, data);
        std::cout << "Apenas para mulheres? (s/n): "; std::cin >> apenas_mulheres_char;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        bool apenas_mulheres = (apenas_mulheres_char == 's' || apenas_mulheres_char == 'S');
        Carona nova_carona = CaronaFactory::criar_carona(origem, destino, data, _usuario_logado, apenas_mulheres, TipoCarona::AGENDADA);
        _caronas.push_back(nova_carona);
        std::cout << "Carona (ID: " << nova_carona.get_id() << ") criada com sucesso!" << std::endl;
    }

    void Sistema::fluxo_buscar_caronas() {
        std::cout << "\n--- Caronas Disponiveis ---" << std::endl;
        if (_caronas.empty()) {
            std::cout << "Nenhuma carona disponivel no momento." << std::endl;
            return;
        }
        for (const auto& carona : _caronas) {
            carona.exibir_info();
        }
        std::cout << "--------------------------" << std::endl;
    }

    void Sistema::exibir_menu() {
        if (_usuario_logado) {
            std::cout << "\nLogado como " << _usuario_logado->get_nome() << " | Comandos: perfil | buscar_caronas | oferecer_carona | cadastrar_veiculo | logout | sair" << std::endl;
        } else {
            std::cout << "\nComandos: cadastro | login | sair" << std::endl;
        }
    }

    std::shared_ptr<Usuario> Sistema::buscar_usuario_por_cpf(const std::string& cpf) {
        for (const auto& u : _usuarios) {
            if (u->get_cpf() == cpf) {
                return u;
            }
        }
        return nullptr;
    }

    void Sistema::carregar_dados_iniciais() {
        std::cout << "-> Carregando dados iniciais..." << std::endl;
        std::ifstream arquivo_usuarios("usuarios.txt");
        if (arquivo_usuarios.is_open()) {
            std::string linha, cpf, nome, email, senha, vinculo, detalhe, gen_str;
            while (std::getline(arquivo_usuarios, linha)) {
                std::stringstream ss(linha);
                std::getline(ss, cpf, ';');
                std::getline(ss, nome, ';');
                std::getline(ss, email, ';');
                std::getline(ss, senha, ';');
                std::getline(ss, gen_str, ';');
                std::getline(ss, vinculo, ';');
                std::getline(ss, detalhe);
                Genero gen = static_cast<Genero>(std::stoi(gen_str));
                if (vinculo == "aluno") {
                    _usuarios.push_back(std::make_shared<Aluno>(nome, cpf, email, senha, gen, detalhe));
                } else if (vinculo == "funcionario") {
                    _usuarios.push_back(std::make_shared<Funcionario>(nome, cpf, email, senha, gen, detalhe));
                }
            }
            std::cout << "   - " << _usuarios.size() << " usuarios carregados." << std::endl;
            arquivo_usuarios.close();
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
                
                auto motorista_ptr = buscar_usuario_por_cpf(cpf_motorista);
                if (motorista_ptr) {
                    if (!motorista_ptr->is_motorista()) {
                        motorista_ptr->cadastrar_veiculo(Veiculo("QWE-5678", "VW", "Gol", "Prata", 5));
                    }
                    bool apenas_mulheres = (std::stoi(apenas_mulheres_str) == 1);
                    _caronas.push_back(CaronaFactory::criar_carona(origem, destino, data, motorista_ptr, apenas_mulheres, TipoCarona::AGENDADA));
                }
            }
            std::cout << "   - " << _caronas.size() << " caronas carregadas." << std::endl;
            arquivo_caronas.close();
        }
    }
    
    void Sistema::salvar_dados() {
        // Implementação futura para salvar em JSON
    }

} // namespace ufmg_carona