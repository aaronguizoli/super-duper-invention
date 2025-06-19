#ifndef SISTEMA_HPP
#define SISTEMA_HPP
#include <vector>
#include <string>
// #include <memory> // Removido
#include "Usuario.hpp"
#include "Carona.hpp"
#include "Solicitacao.hpp"

namespace ufmg_carona {
    class Sistema {
    private:
        // ALTERAÇÃO: De std::vector<std::shared_ptr<Usuario>> para std::vector<Usuario*>
        std::vector<Usuario*> _usuarios; // Sistema AGORA é o proprietário desses objetos
        std::vector<Carona> _caronas; // Caronas são objetos diretos, não ponteiros, gerenciado pelo vector
        // ALTERAÇÃO: De std::vector<std::shared_ptr<Solicitacao>> para std::vector<Solicitacao*>
        std::vector<Solicitacao*> _solicitacoes; // Sistema AGORA é o proprietário desses objetos
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Usuario* _usuario_logado; // Apenas um ponteiro de observação, Sistema não é proprietário via este membro

        void carregar_dados_iniciais();
        void salvar_dados(); // Se implementado, precisaria salvar corretamente os dados dos ponteiros brutos.
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Usuario* buscar_usuario_por_cpf(const std::string& cpf);
        Carona* buscar_carona_por_id(int id); // Já era ponteiro bruto

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

        // Métodos auxiliares
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        void enviar_notificacao(Usuario* usuario, const std::string& mensagem);
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        bool pode_solicitar_carona(Usuario* passageiro, const Carona& carona);

    public:
        Sistema();
        // NOVO: Destrutor para liberar a memória de todos os Usuários e Solicitações.
        ~Sistema();
        void executar();
    };
}
#endif