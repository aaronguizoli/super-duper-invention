#include "Sistema.hpp"
#include "Excecoes.hpp"
#include "Aluno.hpp"
#include "Funcionario.hpp"
#include "CaronaFactory.hpp"
#include "Veiculo.hpp"
#include "Solicitacao.hpp"
#include "Avaliacao.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <map>
#include <tuple> // Incluido para std::tuple

namespace ufmg_carona {

    Sistema::Sistema() : _usuario_logado(nullptr) {
        carregar_dados_iniciais(); // Carrega do usuarios.txt
    }

    Sistema::~Sistema() {
        std::cout << "\nFinalizando o sistema..." << std::endl;
        salvar_dados_usuarios(); // Salva antes de destruir
        // Libera a memoria de todos os usuarios
        for (Usuario* u : _usuarios) {
            delete u;
        }
        _usuarios.clear();

        // Libera a memoria de todas as solicitacoes
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
        char deseja_caronas_char;
        bool deseja_oferecer_caronas = false;

        std::string nome_ufmg, data_nascimento_ufmg, vinculo_ufmg, detalhe_ufmg;

        std::string placa, marca, modelo, cor;
        int lugares;

        std::cout << "--- Cadastro ---" << std::endl;
        std::cout << "CPF: "; std::getline(std::cin, cpf_digitado);

        // 1. Verificar se o CPF já está cadastrado no sistema (na memória)
        if (buscar_usuario_por_cpf(cpf_digitado)) {
            throw AppExcecao("CPF ja cadastrado no sistema. Por favor, faca login.");
        }

        // 2. Buscar o CPF no arquivo de dados da UFMG (dados_ufmg.txt)
        auto dados_ufmg = buscar_dados_ufmg_por_cpf(cpf_digitado);
        bool cpf_encontrado_ufmg = std::get<0>(dados_ufmg);

        if (!cpf_encontrado_ufmg) {
            std::cout << "CPF nao encontrado no cadastro da UFMG. O cadastro nao pode ser realizado." << std::endl;
            return;
        }

        nome_ufmg = std::get<1>(dados_ufmg);
        data_nascimento_ufmg = std::get<3>(dados_ufmg);
        vinculo_ufmg = std::get<4>(dados_ufmg);
        detalhe_ufmg = std::get<5>(dados_ufmg);

        // 3. Validar a completude dos dados do arquivo dados_ufmg.txt
        if (nome_ufmg.empty() || data_nascimento_ufmg.empty() || vinculo_ufmg.empty() || detalhe_ufmg.empty()) {
            std::cout << "As informacoes do seu CPF estao incompletas no cadastro da UFMG. Por favor, procure a secretaria da UFMG para regularizar seu cadastro." << std::endl;
            return;
        }

        // 4. Coletar o restante das informações do usuário
        std::cout << "Telefone (apenas numeros): "; std::getline(std::cin, telefone_digitado);
        std::cout << "Email: "; std::getline(std::cin, email_digitado);
        std::cout << "Senha: "; std::getline(std::cin, senha_digitada);
        std::cout << "Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): "; std::cin >> gen_int;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        Genero gen_digitado = static_cast<Genero>(gen_int);

        std::cout << "Deseja oferecer caronas (ser motorista)? (s/n): ";
        std::cin >> deseja_caronas_char;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        deseja_oferecer_caronas = (deseja_caronas_char == 's' || deseja_caronas_char == 'S');

        Usuario* novo_usuario = nullptr;

        if (vinculo_ufmg == "aluno") {
            novo_usuario = new Aluno(nome_ufmg, cpf_digitado, telefone_digitado, data_nascimento_ufmg, email_digitado, senha_digitada, gen_digitado, detalhe_ufmg, deseja_oferecer_caronas);
        } else if (vinculo_ufmg == "funcionario") {
            novo_usuario = new Funcionario(nome_ufmg, cpf_digitado, telefone_digitado, data_nascimento_ufmg, email_digitado, senha_digitada, gen_digitado, detalhe_ufmg, deseja_oferecer_caronas);
        } else {
            throw AppExcecao("Vinculo invalido no arquivo da UFMG para o CPF informado.");
        }

        if (deseja_oferecer_caronas) {
            std::cout << "--- Cadastro de Veiculo ---" << std::endl;
            std::cout << "Placa: "; std::getline(std::cin, placa);
            std::cout << "Marca: "; std::getline(std::cin, marca);
            std::cout << "Modelo: "; std::getline(std::cin, modelo);
            std::cout << "Cor: "; std::getline(std::cin, cor);
            std::cout << "Total de lugares (com motorista): "; std::cin >> lugares;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            novo_usuario->cadastrar_veiculo(Veiculo(placa, marca, modelo, cor, lugares));
            std::cout << "Veiculo cadastrado com sucesso durante o cadastro!" << std::endl;
        }

        _usuarios.push_back(novo_usuario); // Adiciona o ponteiro bruto à lista
        salvar_dados_usuarios(); // Salva o novo usuário no usuarios.txt
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
        salvar_dados_usuarios(); // Salva a atualização do veículo
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
            std::cout << "\nLogado como " << _usuario_logado->get_nome() << std::endl;
            if (_usuario_logado->get_deseja_oferecer_caronas()) {
                std::cout << "Comandos: perfil, buscar_caronas, solicitar_carona, oferecer_carona, gerenciar_solicitacoes, status_caronas, cadastrar_veiculo, logout, sair" << std::endl;
            } else {
                std::cout << "Comandos: perfil, buscar_caronas, solicitar_carona, status_caronas, cadastrar_veiculo, logout, sair" << std::endl;
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
    
    // NOVO: Funcao para buscar dados do usuario no arquivo dados_ufmg.txt
    // Retorno: tuple<encontrado, nome, cpf, data_nascimento, vinculo, detalhe>
    std::tuple<bool, std::string, std::string, std::string, std::string, std::string> Sistema::buscar_dados_ufmg_por_cpf(const std::string& cpf_buscado) {
        std::ifstream arquivo_ufmg("dados_ufmg.txt");
        if (!arquivo_ufmg.is_open()) {
            std::cerr << "ERRO: Nao foi possivel abrir o arquivo dados_ufmg.txt." << std::endl;
            // Retorna tupla com false e strings vazias para indicar erro ou nao encontrado
            return std::make_tuple(false, "", "", "", "", "");
        }

        std::string linha;
        while (std::getline(arquivo_ufmg, linha)) {
            std::stringstream ss(linha);
            std::string nome_lido, cpf_lido, data_nascimento_lida, vinculo_lido, detalhe_lido;

            std::getline(ss, nome_lido, ';');
            std::getline(ss, cpf_lido, ';');
            std::getline(ss, data_nascimento_lida, ';');
            std::getline(ss, vinculo_lido, ';');
            std::getline(ss, detalhe_lido); // Último campo da linha, sem delimitador

            if (cpf_lido == cpf_buscado) {
                arquivo_ufmg.close();
                // Retorna true e os dados lidos
                return std::make_tuple(true, nome_lido, cpf_lido, data_nascimento_lida, vinculo_lido, detalhe_lido);
            }
        }
        arquivo_ufmg.close();
        return std::make_tuple(false, "", "", "", "", ""); // CPF não encontrado
    }

    void Sistema::carregar_dados_iniciais() {
        std::ifstream arquivo_usuarios("usuarios.txt");
        if (arquivo_usuarios.is_open()) {
            std::string linha;
            // Campos que serão lidos do usuarios.txt
            std::string cpf, nome, telefone, data_nascimento, email, senha, vinculo, detalhe;
            std::string gen_str, deseja_caronas_str;
            std::string endereco_dummy; // Campo dummy para ler o endereco antigo se existir

            while (std::getline(arquivo_usuarios, linha)) {
                std::stringstream ss(linha);
                
                // Leitura dos campos na ordem esperada do usuarios.txt
                std::getline(ss, cpf, ';');
                std::getline(ss, nome, ';');
                std::getline(ss, telefone, ';');
                std::getline(ss, data_nascimento, ';');
                
                // Tentativa de ler o campo de endereco. Se a linha tem menos campos (nova estrutura),
                // este getline falhara para o proximo campo.
                // Uma forma robusta seria verificar o numero de delimitadores ou usar um formato mais flexivel (JSON/XML).
                // Para manter a simplicidade com CSV, precisamos de uma logica que se adapte.
                // A abordagem atual lera o proximo campo como endereco se ele nao for email.
                // Vamos assumir que a primeira leitura para usuario.txt carregará a estrutura ANTIGA.
                // Para a nova estrutura, precisamos de um mecanismo para ignorar ou preencher.
                // Como _endereco foi removido de Usuario, ele nao sera mais usado no objeto,
                // mas a leitura precisa consumir o campo.
                // Uma solução para transição é ler o campo 'endereco' para uma variável dummy.
                
                // Verifica o número de campos na linha para adaptar a leitura
                size_t num_separators = std::count(linha.begin(), linha.end(), ';');

                if (num_separators == 10) { // Antiga estrutura com endereco
                    std::getline(ss, endereco_dummy, ';'); // Le o endereco para uma dummy
                    std::getline(ss, email, ';');
                    std::getline(ss, senha, ';');
                    std::getline(ss, gen_str, ';');
                    std::getline(ss, deseja_caronas_str, ';');
                    std::getline(ss, vinculo, ';');
                    std::getline(ss, detalhe);
                } else if (num_separators == 9) { // Nova estrutura sem endereco
                    // std::getline(ss, email, ';'); // Isso causaria erro se o proximo campo fosse outro
                    // Para garantir que nao le um campo "errado", precisamos ser mais precisos.
                    // A nova estrutura significa que o campo apos data_nascimento é email.
                    // Para evitar confusao, redefinir a leitura completa da linha.
                    // Se o objetivo é que carregar_dados_iniciais sempre leia a NOVA estrutura,
                    // entao o arquivo original usuarios.txt precisa ser migrado ou adaptado.

                    // Pelo enunciado, usuarios.txt VAI MUDAR. Entao, a leitura deve refletir essa nova estrutura.
                    // Se a linha tem 9 separadores, significa que o campo seguinte a data_nascimento é email.
                    std::getline(ss, email, ';');
                    std::getline(ss, senha, ';');
                    std::getline(ss, gen_str, ';');
                    std::getline(ss, deseja_caronas_str, ';');
                    std::getline(ss, vinculo, ';');
                    std::getline(ss, detalhe);
                } else {
                    std::cerr << "AVISO: Linha com formato invalido no usuarios.txt: " << linha << std::endl;
                    continue; // Pula a linha invalida
                }

                Genero gen = static_cast<Genero>(std::stoi(gen_str));
                bool deseja_caronas_do_arquivo = (std::stoi(deseja_caronas_str) == 1);

                Usuario* novo_usuario_carregado = nullptr;
                if (vinculo == "aluno") {
                    novo_usuario_carregado = new Aluno(nome, cpf, telefone, data_nascimento, email, senha, gen, detalhe, deseja_caronas_do_arquivo);
                } else if (vinculo == "funcionario") {
                    novo_usuario_carregado = new Funcionario(nome, cpf, telefone, data_nascimento, email, senha, gen, detalhe, deseja_caronas_do_arquivo);
                }

                if (novo_usuario_carregado) {
                    _usuarios.push_back(novo_usuario_carregado);
                    if (deseja_caronas_do_arquivo && !novo_usuario_carregado->is_motorista()) {
                        novo_usuario_carregado->cadastrar_veiculo(Veiculo("PADRAO" + cpf.substr(0,5), "Default", "Modelo", "Cor", 4));
                    }
                }
            }
            std::cout << "-> " << _usuarios.size() << " usuarios carregados do usuarios.txt." << std::endl;
        } else {
            std::cout << "-> Arquivo usuarios.txt nao encontrado. Nao ha usuarios para carregar." << std::endl;
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
                Usuario* motorista_ptr = buscar_usuario_por_cpf(cpf_motorista);
                if (motorista_ptr) {
                    if (!motorista_ptr->is_motorista()) {
                         motorista_ptr->cadastrar_veiculo(Veiculo("QWE-5678", "VW", "Gol", "Prata", 5));
                         std::cout << "ATENCAO: Veiculo padrao criado para motorista " << motorista_ptr->get_nome() << " carregado de caronas.txt" << std::endl;
                    }
                    bool apenas_mulheres = (std::stoi(apenas_mulheres_str) == 1);
                    _caronas.push_back(CaronaFactory::criar_carona(origem, destino, data, motorista_ptr, apenas_mulheres, TipoCarona::AGENDADA));
                } else {
                    std::cerr << "AVISO: Motorista com CPF " << cpf_motorista << " para carona " << origem << "->" << destino << " nao encontrado. Carona nao carregada." << std::endl;
                }
            }
            std::cout << "-> " << _caronas.size() << " caronas carregadas." << std::endl;
        } else {
            std::cout << "-> Arquivo caronas.txt nao encontrado. Nao ha caronas para carregar." << std::endl;
        }
    }

    // NOVO: Funcao para salvar todos os usuarios na lista _usuarios para o arquivo usuarios.txt
    void Sistema::salvar_dados_usuarios() {
        std::ofstream arquivo_usuarios("usuarios.txt", std::ios::trunc); // Abre para escrita, truncando (limpando) o arquivo
        if (!arquivo_usuarios.is_open()) {
            std::cerr << "ERRO: Nao foi possivel abrir o arquivo usuarios.txt para salvar dados." << std::endl;
            return;
        }

        for (const auto& u : _usuarios) {
            // Escreve os dados na nova ordem e sem o campo endereco
            arquivo_usuarios << u->get_cpf() << ";"
                             << u->get_nome() << ";"
                             << u->get_telefone() << ";"
                             << u->get_data_nascimento() << ";"
                             << u->get_email() << ";" // NOVO: get_email() e get_senha() precisam ser adicionados em Usuario.hpp/cpp
                             << u->get_senha() << ";"
                             << static_cast<int>(u->get_genero()) << ";" // Assumindo que Genero pode ser convertido para int
                             << (u->get_deseja_oferecer_caronas() ? "1" : "0") << ";"
                             << u->get_vinculo_raw() << ";" // NOVO: get_vinculo_raw() para obter apenas "aluno" ou "funcionario"
                             << u->get_detalhe_vinculo() // NOVO: get_detalhe_vinculo() para obter "curso" ou "setor"
                             << std::endl;
        }
        arquivo_usuarios.close();
        std::cout << "-> " << _usuarios.size() << " usuarios salvos em usuarios.txt." << std::endl;
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
        if (!_usuario_logado->get_deseja_oferecer_caronas()) {
            std::cout << "Apenas motoristas podem gerenciar solicitacoes." << std::endl;
            return;
        }

        std::cout << "\n--- Gerenciar Solicitacoes ---" << std::endl;

        std::vector<Solicitacao*> solicitacoes_motorista;
        for (const auto& solicitacao : _solicitacoes) {
            if (solicitacao->get_carona()->get_motorista() == _usuario_logado &&
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