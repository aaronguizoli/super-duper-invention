#ifndef SISTEMA_HPP
#define SISTEMA_HPP
#include <vector>
#include <string>
#include <tuple>
#include <fstream>
#include <ctime>
#include "Usuario.hpp"
#include "Motorista.hpp"
#include "Carona.hpp"
#include "Solicitacao.hpp"
#include "Genero.hpp"
#include "Veiculo.hpp"
#include "Rotina.hpp"

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
        void salvar_dados_rotinas(); // Adicionado
        void carregar_dados_rotinas(); // Adicionado

        Usuario* buscar_usuario_por_cpf(const std::string& cpf);
        Carona* buscar_carona_por_id(int id);
        Veiculo* buscar_veiculo_por_placa_motorista(Motorista* motorista, const std::string& placa);


        std::tuple<bool, std::string, std::string, std::string, std::string> buscar_dados_ufmg_por_cpf(const std::string& cpf_buscado);

        // Funcoes de Menu e Fluxo
        void exibir_menu_inicial_nao_logado();
        void exibir_menu_logado();
        void exibir_menu_passageiro();
        void exibir_menu_motorista();
        void exibir_menu(); // Mantida para consistencia, mas lógica principal movida para executar()

        void processar_comando_logado(const std::string& comando_str);

        void fluxo_cadastro();
        void fluxo_login();
        void fluxo_logout();
        
        void fluxo_oferecer_carona();
        void fluxo_solicitar_carona();
        void fluxo_gerenciar_solicitacoes();
        void fluxo_status_caronas();
        void fluxo_cadastrar_veiculo();
        void fluxo_editar_perfil();
        
        void fluxo_passageiro_menu();
        void fluxo_motorista_menu();
        void fluxo_editar_perfil_ou_veiculos();
        void fluxo_tornar_motorista();
        
        void fluxo_gerenciar_veiculos();
        void fluxo_editar_veiculo(Motorista* motorista);
        void fluxo_excluir_veiculo(Motorista* motorista);

        void fluxo_gerenciar_rotinas();
        void fluxo_adicionar_rotina(Motorista* motorista);
        void fluxo_visualizar_rotinas(Motorista* motorista);
        void fluxo_excluir_rotina(Motorista* motorista);

        void gerar_caronas_de_rotinas();

        void enviar_notificacao(Usuario* usuario, const std::string& mensagem);
        bool pode_solicitar_carona(Usuario* passageiro, const Carona& carona);

        int coletar_int_input(const std::string& prompt, int min_val, int max_val);
        std::vector<DiaDaSemana> coletar_dias_da_semana(const std::string& prompt);

        std::tm parse_datetime_string(const std::string& dt_str) const;
        std::string get_current_datetime_string() const;
        bool is_datetime_in_past(const std::string& dt_str) const;
        void remover_caronas_passadas();

    public:
        Sistema();
        ~Sistema();
        void executar();
    };
}
#endif