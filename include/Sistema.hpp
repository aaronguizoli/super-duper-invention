#ifndef SISTEMA_HPP
#define SISTEMA_HPP
#include <vector>
#include <string>
#include <tuple> // Para usar std::tuple na funcao de busca de dados
#include <fstream> // Para manipulacao de arquivos
#include "Usuario.hpp"
#include "Carona.hpp"
#include "Solicitacao.hpp"
#include "Genero.hpp"

namespace ufmg_carona {
    class Sistema {
    private:
        std::vector<Usuario*> _usuarios;
        std::vector<Carona> _caronas;
        std::vector<Solicitacao*> _solicitacoes;
        Usuario* _usuario_logado;

        void carregar_dados_iniciais();
        void salvar_dados_usuarios(); // NOVO: Funcao para salvar os usuarios no usuarios.txt

        Usuario* buscar_usuario_por_cpf(const std::string& cpf);
        Carona* buscar_carona_por_id(int id);

        // NOVO: Funcao para buscar dados completos de um usuario no arquivo dados_ufmg.txt
        // Retorno: tuple<encontrado, nome, cpf, data_nascimento, vinculo, detalhe>
        std::tuple<bool, std::string, std::string, std::string, std::string, std::string> buscar_dados_ufmg_por_cpf(const std::string& cpf_buscado);


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