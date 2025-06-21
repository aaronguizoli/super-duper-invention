#ifndef SISTEMA_HPP
#define SISTEMA_HPP
#include <vector>
#include <string>
#include <tuple>
#include <fstream>
#include <ctime>
#include <map> // Incluir para std::map
#include "Usuario.hpp"
#include "Motorista.hpp"
#include "Carona.hpp"
#include "Solicitacao.hpp"
#include "Genero.hpp"
#include "Veiculo.hpp"
#include "Rotina.hpp"
#include "Zona.hpp" // Incluir para usar Zona e UFMGPosicao

namespace ufmg_carona {
    class Sistema {
    private:
        std::vector<Usuario*> _usuarios;
        std::vector<Carona> _caronas;
        std::vector<Solicitacao*> _solicitacoes;
        Usuario* _usuario_logado;

        // Mapeamentos para Zona e UFMGPosicao
        std::map<int, Zona> _int_para_zona;
        std::map<Zona, std::string> _zona_para_string;
        std::map<int, UFMGPosicao> _int_para_ufmg_posicao;
        std::map<UFMGPosicao, std::string> _ufmg_posicao_para_string;


        void carregar_dados_iniciais();
        void salvar_dados_usuarios();
        void salvar_dados_veiculos();
        void salvar_dados_rotinas();
        void carregar_dados_rotinas();
        // Metodos para carregar e salvar caronas com as novas Zonas
        void carregar_dados_caronas();
        void salvar_dados_caronas();
        // Metodos para carregar e salvar solicitacoes
        void carregar_dados_solicitacoes();
        void salvar_dados_solicitacoes();


        Usuario* buscar_usuario_por_cpf(const std::string& cpf);
        Carona* buscar_carona_por_id(int id);
        Veiculo* buscar_veiculo_por_placa_motorista(Motorista* motorista, const std::string& placa);


        std::tuple<bool, std::string, std::string, std::string, std::string> buscar_dados_ufmg_por_cpf(const std::string& cpf_buscado);

        // Funcoes de Menu e Fluxo
        void exibir_menu_inicial_nao_logado();
        void exibir_menu_logado();
        void exibir_menu_passageiro();
        void exibir_menu_motorista();
        void exibir_menu(); 

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
        // Nova regra para cancelar solicitacoes do passageiro
        void cancelar_outras_solicitacoes_passageiro(Usuario* passageiro, const Carona& carona_aceita);


        int coletar_int_input(const std::string& prompt, int min_val, int max_val);
        std::string coletar_string_input(const std::string& prompt); // Função auxiliar para coletar string
        std::vector<DiaDaSemana> coletar_dias_da_semana(const std::string& prompt);
        // Funcoes para coletar Zona e UFMGPosicao
        Zona coletar_zona_input(const std::string& prompt);
        UFMGPosicao coletar_ufmg_posicao_input(const std::string& prompt);

        // Funcoes de conversao de enum para string
        std::string zona_to_string(Zona z) const;
        Zona string_to_zona(const std::string& s) const; // Para leitura de arquivos
        std::string ufmg_posicao_to_string(UFMGPosicao up) const;
        UFMGPosicao string_to_ufmg_posicao(const std::string& s) const; // Para leitura de arquivos


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