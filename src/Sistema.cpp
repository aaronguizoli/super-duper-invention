#include "Sistema.hpp"
#include "Excecoes.hpp"
#include "Aluno.hpp"
#include "Funcionario.hpp"
#include "CaronaFactory.hpp"
#include "Veiculo.hpp"
#include "Solicitacao.hpp" // Para Solicitacao*
#include "Avaliacao.hpp" // Para Avaliacao*
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm> // Para std::remove

namespace ufmg_carona {

    Sistema::Sistema() : _usuario_logado(nullptr) { // _usuario_logado é agora um ponteiro bruto
        carregar_dados_iniciais();
    }

    // NOVO: Implementação do destrutor de Sistema para liberar toda a memória alocada
    Sistema::~Sistema() {
        std::cout << "\nFinalizando o sistema..." << std::endl;
        // Libera a memória de todos os usuários
        for (Usuario* u : _usuarios) {
            delete u;
        }
        _usuarios.clear(); // Limpa o vetor

        // Libera a memória de todas as solicitações
        for (Solicitacao* s : _solicitacoes) {
            delete s;
        }
        _solicitacoes.clear(); // Limpa o vetor

        // As caronas são objetos diretos (std::vector<Carona>), então são destruídas automaticamente
        // quando o _caronas sai de escopo.
        // std::cout << "DEBUG: Memória de usuários e solicitações liberada." << std::endl; // Para debug, pode ser removido
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
            } catch (const std::bad_alloc& e) { // Adicionado para capturar erros de alocação de memória
                std::cerr << "ERRO: Falha na alocação de memória: " << e.what() << std::endl;
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
        std::string nome, cpf, email, senha, vinculo, detalhe;
        std::string telefone, data_nascimento, endereco;
        int gen_int;
        char deseja_caronas_char;
        bool deseja_oferecer_caronas = false;

        std::string placa, marca, modelo, cor;
        int lugares;

        std::cout << "--- Cadastro ---" << std::endl;
        std::cout << "Nome completo: "; std::getline(std::cin, nome);
        std::cout << "CPF: "; std::getline(std::cin, cpf);
        std::cout << "Telefone (apenas numeros): "; std::getline(std::cin, telefone);
        std::cout << "Data de Nascimento (DD/MM/AAAA): "; std::getline(std::cin, data_nascimento);
        std::cout << "Endereco: "; std::getline(std::cin, endereco);
        std::cout << "Email: "; std::getline(std::cin, email);
        std::cout << "Senha: "; std::getline(std::cin, senha);
        std::cout << "Genero (0:Masc, 1:Fem, 2:Outro, 3:Nao Informar): "; std::cin >> gen_int;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Deseja oferecer caronas (ser motorista)? (s/n): ";
        std::cin >> deseja_caronas_char;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        deseja_oferecer_caronas = (deseja_caronas_char == 's' || deseja_caronas_char == 'S');

        if (buscar_usuario_por_cpf(cpf)) throw AppExcecao("CPF ja cadastrado.");

        Genero gen = static_cast<Genero>(gen_int);

        // ALTERAÇÃO: Uso de ponteiro bruto e 'new' para alocação
        Usuario* novo_usuario = nullptr; // Inicializa com nullptr

        std::cout << "Vinculo (aluno/funcionario): "; std::getline(std::cin, vinculo);
        if (vinculo == "aluno") {
            std::cout << "Curso: "; std::getline(std::cin, detalhe);
            novo_usuario = new Aluno(nome, cpf, telefone, data_nascimento, endereco, email, senha, gen, detalhe, deseja_oferecer_caronas);
        } else if (vinculo == "funcionario") {
            std::cout << "Setor: "; std::getline(std::cin, detalhe);
            novo_usuario = new Funcionario(nome, cpf, telefone, data_nascimento, endereco, email, senha, gen, detalhe, deseja_oferecer_caronas);
        } else {
            throw AppExcecao("Vinculo invalido.");
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
        std::cout << "Cadastro realizado com sucesso!" << std::endl;
    }

    void Sistema::fluxo_login() {
        std::string cpf, senha;
        std::cout << "--- Login ---" << std::endl;
        std::cout << "CPF: "; std::getline(std::cin, cpf);
        std::cout << "Senha: "; std::getline(std::cin, senha);
        // ALTERAÇÃO: buscar_usuario_por_cpf agora retorna Usuario*
        Usuario* u = buscar_usuario_por_cpf(cpf);
        if (u && u->verificar_senha(senha)) {
            _usuario_logado = u; // Atribui o ponteiro bruto
            std::cout << "Login bem-sucedido!" << std::endl;
        } else { throw AutenticacaoFalhouException(); }
    }

    void Sistema::fluxo_logout() {
        _usuario_logado = nullptr; // Apenas define para nulo, não deleta (Sistema não é proprietário via _usuario_logado, é uma referência)
        std::cout << "Logout efetuado." << std::endl;
    }

    void Sistema::fluxo_cadastrar_veiculo() {
        if (!_usuario_logado->is_motorista()) throw AppExcecao("Voce ja possui um veiculo cadastrado.");
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
        // ALTERAÇÃO: Passa o ponteiro bruto do motorista
        Carona nova_carona = CaronaFactory::criar_carona(origem, destino, data, _usuario_logado, apenas_mulheres, TipoCarona::AGENDADA);
        _caronas.push_back(nova_carona); // Caronas são objetos diretos no vector, sem ponteiros
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
            if (_usuario_logado->is_motorista()) {
                std::cout << "Comandos: perfil, buscar_caronas, solicitar_carona, oferecer_carona, gerenciar_solicitacoes, status_caronas, cadastrar_veiculo, logout, sair" << std::endl;
            } else {
                std::cout << "Comandos: perfil, buscar_caronas, solicitar_carona, status_caronas, cadastrar_veiculo, logout, sair" << std::endl;
            }
        } else {
            std::cout << "\nComandos: cadastro, login, sair" << std::endl;
        }
    }

    // ALTERAÇÃO: Retorna Usuario*
    Usuario* Sistema::buscar_usuario_por_cpf(const std::string& cpf) {
        for (const auto& u : _usuarios) {
            if (u->get_cpf() == cpf) return u;
        }
        return nullptr;
    }

    void Sistema::carregar_dados_iniciais() {
        std::ifstream arquivo_usuarios("usuarios.txt");
        if (arquivo_usuarios.is_open()) {
            std::string linha, cpf, nome, email, senha, vinculo, detalhe, gen_str;
            std::string telefone, data_nascimento, endereco;
            std::string deseja_oferecer_caronas_str;

            while (std::getline(arquivo_usuarios, linha)) {
                std::stringstream ss(linha);
                std::getline(ss, cpf, ';');
                std::getline(ss, nome, ';');
                std::getline(ss, telefone, ';');
                std::getline(ss, data_nascimento, ';');
                std::getline(ss, endereco, ';');
                std::getline(ss, email, ';');
                std::getline(ss, senha, ';');
                std::getline(ss, gen_str, ';');
                std::getline(ss, deseja_oferecer_caronas_str, ';');
                std::getline(ss, vinculo, ';');
                std::getline(ss, detalhe);

                Genero gen = static_cast<Genero>(std::stoi(gen_str));
                bool deseja_caronas_do_arquivo = (std::stoi(deseja_oferecer_caronas_str) == 1);

                // ALTERAÇÃO: Uso de 'new' em vez de std::make_shared
                Usuario* novo_usuario_carregado = nullptr;
                if (vinculo == "aluno") {
                    novo_usuario_carregado = new Aluno(nome, cpf, telefone, data_nascimento, endereco, email, senha, gen, detalhe, deseja_caronas_do_arquivo);
                } else if (vinculo == "funcionario") {
                    novo_usuario_carregado = new Funcionario(nome, cpf, telefone, data_nascimento, endereco, email, senha, gen, detalhe, deseja_caronas_do_arquivo);
                }

                if (novo_usuario_carregado) {
                    _usuarios.push_back(novo_usuario_carregado);
                    if (deseja_caronas_do_arquivo && !novo_usuario_carregado->is_motorista()) {
                        novo_usuario_carregado->cadastrar_veiculo(Veiculo("PADRAO123", "Default", "Modelo", "Cor", 4));
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
                // ALTERAÇÃO: buscar_usuario_por_cpf retorna Usuario*
                Usuario* motorista_ptr = buscar_usuario_por_cpf(cpf_motorista);
                if (motorista_ptr) {
                    if (!motorista_ptr->is_motorista()) motorista_ptr->cadastrar_veiculo(Veiculo("QWE-5678", "VW", "Gol", "Prata", 5));
                    bool apenas_mulheres = (std::stoi(apenas_mulheres_str) == 1);
                    // ALTERAÇÃO: Passa o ponteiro bruto
                    _caronas.push_back(CaronaFactory::criar_carona(origem, destino, data, motorista_ptr, apenas_mulheres, TipoCarona::AGENDADA));
                }
            }
            std::cout << "-> " << _caronas.size() << " caronas carregadas." << std::endl;
        }
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

        // ALTERAÇÃO: Passa _usuario_logado (ponteiro bruto)
        if (!pode_solicitar_carona(_usuario_logado, *carona_escolhida)) {
            return;
        }

        // ALTERAÇÃO: Uso de 'new' para criar Solicitacao
        Solicitacao* nova_solicitacao = new Solicitacao(_usuario_logado, carona_escolhida);
        _solicitacoes.push_back(nova_solicitacao); // Sistema é proprietário da solicitação
        carona_escolhida->adicionar_solicitacao(nova_solicitacao); // Carona apenas observa

        std::string mensagem = "Nova solicitacao de carona de " + _usuario_logado->get_nome() +
                              " para a carona ID: " + std::to_string(id_carona);
        // ALTERAÇÃO: Passa o ponteiro bruto do motorista
        enviar_notificacao(carona_escolhida->get_motorista(), mensagem);

        std::cout << "Solicitacao enviada com sucesso!" << std::endl;
    }

    void Sistema::fluxo_gerenciar_solicitacoes() {
        if (!_usuario_logado->is_motorista()) {
            std::cout << "Apenas motoristas podem gerenciar solicitacoes." << std::endl;
            return;
        }

        std::cout << "\n--- Gerenciar Solicitacoes ---" << std::endl;

        // ALTERAÇÃO: vetor de ponteiros brutos
        std::vector<Solicitacao*> solicitacoes_motorista;
        for (const auto& solicitacao : _solicitacoes) { // Itera sobre ponteiros brutos
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
            solicitacoes_motorista[i]->exibir_para_motorista(); // Acesso direto ao ponteiro
        }

        int escolha;
        std::cout << "\nEscolha uma solicitacao para responder (0 para voltar): ";
        std::cin >> escolha;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (escolha == 0 || escolha > static_cast<int>(solicitacoes_motorista.size())) {
            return;
        }

        Solicitacao* solicitacao_escolhida = solicitacoes_motorista[escolha - 1]; // Pega o ponteiro bruto

        char resposta;
        std::cout << "Aceitar solicitacao? (s/n): ";
        std::cin >> resposta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (resposta == 's' || resposta == 'S') {
            solicitacao_escolhida->aceitar();
            solicitacao_escolhida->get_carona()->adicionar_passageiro(solicitacao_escolhida->get_passageiro()); // Passa ponteiro bruto
            enviar_notificacao(solicitacao_escolhida->get_passageiro(), // Passa ponteiro bruto
                             "Sua solicitacao de carona foi ACEITA!");
            std::cout << "Solicitacao aceita!" << std::endl;
        } else {
            solicitacao_escolhida->recusar();
            enviar_notificacao(solicitacao_escolhida->get_passageiro(), // Passa ponteiro bruto
                             "Sua solicitacao de carona foi RECUSADA.");
            std::cout << "Solicitacao recusada." << std::endl;
        }
    }

    void Sistema::fluxo_status_caronas() {
        std::cout << "\n--- Status das Minhas Solicitacoes ---" << std::endl;

        // ALTERAÇÃO: vetor de ponteiros brutos
        std::vector<Solicitacao*> minhas_solicitacoes;
        for (const auto& solicitacao : _solicitacoes) { // Itera sobre ponteiros brutos
            if (solicitacao->get_passageiro() == _usuario_logado) { // Comparação direta de ponteiros
                minhas_solicitacoes.push_back(solicitacao);
            }
        }

        if (minhas_solicitacoes.empty()) {
            std::cout << "Voce nao fez nenhuma solicitacao de carona ainda." << std::endl;
            return;
        }

        for (const auto& solicitacao : minhas_solicitacoes) { // Itera sobre ponteiros brutos
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

    // ALTERAÇÃO: Recebe Usuario*
    void Sistema::enviar_notificacao(Usuario* usuario, const std::string& mensagem) {
        if (usuario) { // Garante que o ponteiro não é nulo antes de desreferenciar
            std::cout << "[NOTIFICACAO para " << usuario->get_nome() << "]: " << mensagem << std::endl;
        }
    }

    // ALTERAÇÃO: Recebe Usuario*
    bool Sistema::pode_solicitar_carona(Usuario* passageiro, const Carona& carona) {
        // Verificar se é o próprio motorista
        if (passageiro == carona.get_motorista()) { // Comparação direta de ponteiros
            std::cout << "Voce nao pode solicitar sua propria carona!" << std::endl;
            return false;
        }

        // Verificar se há vagas
        if (carona.get_vagas_disponiveis() <= 0) {
            std::cout << "Esta carona nao possui vagas disponiveis." << std::endl;
            return false;
        }

        // Verificar se já solicitou esta carona
        for (const auto& solicitacao : _solicitacoes) { // Itera sobre ponteiros brutos
            if (solicitacao->get_passageiro() == passageiro && // Comparação direta de ponteiros
                solicitacao->get_carona()->get_id() == carona.get_id()) {
                std::cout << "Voce ja solicitou esta carona!" << std::endl;
                return false;
            }
        }

        return true;
    }
}