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

        // Funcao para buscar dados completos de um usuario no arquivo dados_ufmg.txt
        std::tuple<bool, std::string, std::string, std::string, std::string> buscar_dados_ufmg_por_cpf(const std::string& cpf_buscado);


        void exibir_menu();
        void processar_comando(const std::string& comando);

        void fluxo_cadastro();
        void fluxo_login();
        void fluxo_logout();
        void fluxo_oferecer_carona();
        void fluxo_buscar_caronas();
        void fluxo_solicitar_carona();
        void fluxo_gerenciar_solicitacoes();
        void fluxo_status_caronas();
        void fluxo_cadastrar_veiculo();

        void enviar_notificacao(Usuario* usuario, const std::string& mensagem);
        bool pode_solicitar_carona(Usuario* passageiro, const Carona& carona);

    public:
        Sistema();
        ~Sistema();
        void executar();
    };
}
#endif