#ifndef SISTEMA_HPP
#define SISTEMA_HPP
#include <vector>
#include <string>
#include <tuple>
#include <fstream>
#include "Usuario.hpp"
#include "Motorista.hpp"
#include "Carona.hpp"
#include "Solicitacao.hpp"
#include "Genero.hpp"
#include "Veiculo.hpp"

namespace ufmg_carona {
    class Sistema {
    private:
        std::vector<Usuario*> _usuarios;
        std::vector<Carona> _caronas;
        std::vector<Solicitacao*> _solicitacoes;
        Usuario* _usuario_logado;

        void carregar_dados_iniciais();
        void salvar_dados_usuarios();
        void salvar_dados_veiculos();

        Usuario* buscar_usuario_por_cpf(const std::string& cpf);
        Carona* buscar_carona_por_id(int id);
        Veiculo* buscar_veiculo_por_placa_motorista(Motorista* motorista, const std::string& placa);


        // Funcao para buscar dados completos de um usuario no arquivo dados_ufmg.txt
        std::tuple<bool, std::string, std::string, std::string, std::string> buscar_dados_ufmg_por_cpf(const std::string& cpf_buscado);

        // --- MÉTODOS DE MENU E FLUXO ---
        void exibir_menu(); // Menu principal (logado/nao logado)
        void exibir_menu_inicial_nao_logado(); // NOVO: Menu inicial com numeros
        void exibir_menu_logado(); // Novo menu para usuario logado
        void exibir_menu_passageiro(); // Novo menu para passageiro
        void exibir_menu_motorista(); // Novo menu para motorista
        
        // void processar_comando(const std::string& comando_str); // Esta será dividida
        void processar_comando_logado(const std::string& comando_str); // NOVO: Logica para comandos de usuario logado


        // Fluxos de entrada/saida
        void fluxo_cadastro();
        void fluxo_login();
        void fluxo_logout();
        
        // Fluxos principais (agora acessados via submenus ou diretamente)
        void fluxo_oferecer_carona();
        void fluxo_solicitar_carona(); // Agora inclui filtros
        void fluxo_gerenciar_solicitacoes();
        void fluxo_status_caronas();
        void fluxo_cadastrar_veiculo();
        void fluxo_editar_perfil(); // Edicao de dados do perfil
        
        // NOVOS: Fluxos de submenus para organização e fluxo hierárquico
        void fluxo_passageiro_menu();
        void fluxo_motorista_menu();
        void fluxo_editar_perfil_ou_veiculos(); // Submenu para perfil e veiculos
        void fluxo_tornar_motorista(); // Fluxo para um usuário se tornar motorista
        
        // NOVOS: Fluxos de gerenciamento de veiculos
        void fluxo_gerenciar_veiculos();
        void fluxo_editar_veiculo(Motorista* motorista);
        void fluxo_excluir_veiculo(Motorista* motorista);

        void enviar_notificacao(Usuario* usuario, const std::string& mensagem);
        bool pode_solicitar_carona(Usuario* passageiro, const Carona& carona);

        // Funcao auxiliar para coletar input inteiro com validacao
        int coletar_int_input(const std::string& prompt, int min_val, int max_val);
        
    public:
        Sistema();
        ~Sistema();
        void executar();
    };
}
#endif