#include "Sistema.hpp"
#include "Excecoes.hpp"
#include "Aluno.hpp"
#include "Funcionario.hpp"
#include "CaronaFactory.hpp"
#include "Veiculo.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

namespace ufmg_carona {

    Sistema::Sistema() : _usuario_logado(nullptr) { carregar_dados_iniciais(); }
    Sistema::~Sistema() { std::cout << "\nFinalizando o sistema..." << std::endl; }

    void Sistema::executar() {
        std::cout << "== Sistema de Caronas UFMG iniciado ==" << std::endl;
        std::string comando;
        while (true) {
            exibir_menu();
            std::cout << "> ";
            std::getline(std::cin, comando);
            if (comando == "sair" || std::cin.eof()) break;
            if (comando.empty()) continue;
            try { processar_comando(comando); } catch (const AppExcecao& e) { std::cerr << "ERRO: " << e.what() << std::endl; }
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
        std::string telefone, data_nascimento, endereco; // NOVAS VARIÁVEIS
        int gen_int;
        char deseja_caronas_char; // NOVA VARIÁVEL
        bool deseja_oferecer_caronas = false; // NOVA VARIÁVEL

        std::string placa, marca, modelo, cor; // NOVAS VARIÁVEIS para veículo
        int lugares; // NOVA VARIÁVEL para veículo

        std::cout << "--- Cadastro ---" << std::endl;
        std::cout << "Nome completo: "; std::getline(std::cin, nome);
        std::cout << "CPF: "; std::getline(std::cin, cpf);
        std::cout << "Telefone (apenas numeros): "; std::getline(std::cin, telefone); // NOVA SOLICITAÇÃO
        std::cout << "Data de Nascimento (DD/MM/AAAA): "; std::getline(std::cin, data_nascimento); // NOVA SOLICITAÇÃO
        std::cout << "Endereco: "; std::getline(std::cin, endereco); // NOVA SOLICITAÇÃO
        std::cout << "Email: "; std::getline(std::cin, email);
        std::cout << "Senha: "; std::getline(std::cin, senha);
        std::cout << "Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): "; std::cin >> gen_int;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer de entrada

        // Perguntar se deseja oferecer caronas
        std::cout << "Deseja oferecer caronas (ser motorista)? (s/n): ";
        std::cin >> deseja_caronas_char;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer
        deseja_oferecer_caronas = (deseja_caronas_char == 's' || deseja_caronas_char == 'S');

        if (buscar_usuario_por_cpf(cpf)) throw AppExcecao("CPF ja cadastrado.");

        Genero gen = static_cast<Genero>(gen_int);

        std::shared_ptr<Usuario> novo_usuario; // Cria um shared_ptr para o novo usuário

        std::cout << "Vinculo (aluno/funcionario): "; std::getline(std::cin, vinculo);
        if (vinculo == "aluno") {
            std::cout << "Curso: "; std::getline(std::cin, detalhe);
            // NOVO: Passando todas as informações para o construtor do Aluno
            novo_usuario = std::make_shared<Aluno>(nome, cpf, telefone, data_nascimento, endereco, email, senha, gen, detalhe, deseja_oferecer_caronas);
        } else if (vinculo == "funcionario") {
            std::cout << "Setor: "; std::getline(std::cin, detalhe);
            // NOVO: Passando todas as informações para o construtor do Funcionario
            novo_usuario = std::make_shared<Funcionario>(nome, cpf, telefone, data_nascimento, endereco, email, senha, gen, detalhe, deseja_oferecer_caronas);
        } else {
            throw AppExcecao("Vinculo invalido.");
        }

        // Se o usuário deseja oferecer caronas, solicitar dados do veículo
        if (deseja_oferecer_caronas) {
            std::cout << "--- Cadastro de Veiculo ---" << std::endl;
            // CNH não é um atributo de Veiculo nem de Usuario neste sistema.
            // Para adicionar CNH, você precisaria de um atributo na classe Usuario.
            // Por enquanto, vamos manter apenas os dados do veículo.
            std::cout << "Placa: "; std::getline(std::cin, placa);
            std::cout << "Marca: "; std::getline(std::cin, marca);
            std::cout << "Modelo: "; std::getline(std::cin, modelo);
            std::cout << "Cor: "; std::getline(std::cin, cor);
            std::cout << "Total de lugares (com motorista): "; std::cin >> lugares;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer

            // Cadastrar o veículo para o novo usuário
            novo_usuario->cadastrar_veiculo(Veiculo(placa, marca, modelo, cor, lugares));
            std::cout << "Veiculo cadastrado com sucesso durante o cadastro!" << std::endl;
        }

        _usuarios.push_back(novo_usuario); // Adiciona o novo usuário à lista
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
        } else { throw AutenticacaoFalhouException(); }
    }

    void Sistema::fluxo_logout() {
        _usuario_logado = nullptr;
        std::cout << "Logout efetuado." << std::endl;
    }

    void Sistema::fluxo_cadastrar_veiculo() {
        if (_usuario_logado->is_motorista()) throw AppExcecao("Voce ja possui um veiculo cadastrado.");
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
        if (!_usuario_logado->is_motorista()) throw AppExcecao("Cadastre um veiculo para oferecer caronas.");
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
        for (const auto& carona : _caronas) { carona.exibir_info(); }
    }

    void Sistema::exibir_menu() {
        if (_usuario_logado) {
            std::cout << "\nLogado como " << _usuario_logado->get_nome() << " | Comandos: perfil, buscar_caronas, oferecer_carona, cadastrar_veiculo, logout, sair" << std::endl;
        } else {
            std::cout << "\nComandos: cadastro, login, sair" << std::endl;
        }
    }

    std::shared_ptr<Usuario> Sistema::buscar_usuario_por_cpf(const std::string& cpf) {
        for (const auto& u : _usuarios) {
            if (u->get_cpf() == cpf) return u;
        }
        return nullptr;
    }

    void Sistema::carregar_dados_iniciais() {
        std::ifstream arquivo_usuarios("usuarios.txt");
        if (arquivo_usuarios.is_open()) {
            // Atualize as variáveis para carregar as novas informações
            std::string linha, cpf, nome, email, senha, vinculo, detalhe, gen_str;
            std::string telefone, data_nascimento, endereco; // NOVAS VARIÁVEIS
            std::string deseja_oferecer_caronas_str; // NOVA VARIÁVEL para carregar

            while (std::getline(arquivo_usuarios, linha)) {
                std::stringstream ss(linha);
                std::getline(ss, cpf, ';');
                std::getline(ss, nome, ';');
                std::getline(ss, telefone, ';'); // NOVO: Ler telefone
                std::getline(ss, data_nascimento, ';'); // NOVO: Ler data_nascimento
                std::getline(ss, endereco, ';'); // NOVO: Ler endereco
                std::getline(ss, email, ';');
                std::getline(ss, senha, ';');
                std::getline(ss, gen_str, ';');
                std::getline(ss, deseja_oferecer_caronas_str, ';'); // NOVO: Ler se deseja oferecer caronas
                std::getline(ss, vinculo, ';');
                std::getline(ss, detalhe);

                Genero gen = static_cast<Genero>(std::stoi(gen_str));
                bool deseja_caronas_do_arquivo = (std::stoi(deseja_oferecer_caronas_str) == 1); // Converte para bool

                // Crie o usuário com todas as novas informações
                if (vinculo == "aluno") {
                    _usuarios.push_back(std::make_shared<Aluno>(nome, cpf, telefone, data_nascimento, endereco, email, senha, gen, detalhe, deseja_caronas_do_arquivo));
                } else if (vinculo == "funcionario") {
                    _usuarios.push_back(std::make_shared<Funcionario>(nome, cpf, telefone, data_nascimento, endereco, email, senha, gen, detalhe, deseja_caronas_do_arquivo));
                }
                // Atualmente, o carregamento de veículo específico do arquivo não está implementado aqui.
                // Se deseja_caronas_do_arquivo for true, um veículo padrão seria cadastrado
                // para o usuário ao carregar, ou você adicionaria a lógica para ler os dados do veículo
                // também do arquivo usuarios.txt ou de um arquivo separado.
                if (deseja_caronas_do_arquivo) {
                    // Por simplicidade, se o usuário deseja caronas e não tem veículo cadastrado via arquivo,
                    // um veículo padrão é adicionado. Em um sistema real, você carregaria os dados do veículo.
                    if (!_usuarios.back()->is_motorista()) { // Verifica se ainda não tem veículo
                         _usuarios.back()->cadastrar_veiculo(Veiculo("PADRAO123", "Default", "Modelo", "Cor", 4));
                    }
                }
            }
            std::cout << "-> " << _usuarios.size() << " usuarios carregados." << std::endl;
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
                    // A lógica aqui garante que o motorista tenha um veículo antes de criar a carona.
                    // Isso é importante caso o arquivo de usuários não tenha fornecido os dados do veículo.
                    if (!motorista_ptr->is_motorista()) motorista_ptr->cadastrar_veiculo(Veiculo("QWE-5678", "VW", "Gol", "Prata", 5));
                    bool apenas_mulheres = (std::stoi(apenas_mulheres_str) == 1);
                    _caronas.push_back(CaronaFactory::criar_carona(origem, destino, data, motorista_ptr, apenas_mulheres, TipoCarona::AGENDADA));
                }
            }
            std::cout << "-> " << _caronas.size() << " caronas carregadas." << std::endl;
        }
    }
}