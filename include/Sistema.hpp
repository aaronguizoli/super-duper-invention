#ifndef SISTEMA_HPP
#define SISTEMA_HPP
#include <vector>
#include <string>
#include <memory>
#include "Usuario.hpp"
#include "Carona.hpp"
#include "Solicitacao.hpp"

namespace ufmg_carona {
    class Sistema {
    private:
        std::vector<std::shared_ptr<Usuario>> _usuarios;
        std::vector<Carona> _caronas;
        std::vector<std::shared_ptr<Solicitacao>> _solicitacoes;
        std::shared_ptr<Usuario> _usuario_logado;
        
        void carregar_dados_iniciais();
        void salvar_dados();
        std::shared_ptr<Usuario> buscar_usuario_por_cpf(const std::string& cpf);
        Carona* buscar_carona_por_id(int id);
        
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
        void enviar_notificacao(std::shared_ptr<Usuario> usuario, const std::string& mensagem);
        bool pode_solicitar_carona(std::shared_ptr<Usuario> passageiro, const Carona& carona);

    public:
        Sistema();
        ~Sistema();
        void executar();
    };
}
#endif
