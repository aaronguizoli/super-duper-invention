#ifndef SISTEMA_HPP
#define SISTEMA_HPP
#include <vector>
#include <string>
#include <memory>
#include "Usuario.hpp"
#include "Carona.hpp"

namespace ufmg_carona {
    class Sistema {
    private:
        std::vector<std::shared_ptr<Usuario>> _usuarios;
        std::vector<Carona> _caronas;
        std::shared_ptr<Usuario> _usuario_logado;
        
        void carregar_dados_iniciais();
        void salvar_dados();
        std::shared_ptr<Usuario> buscar_usuario_por_cpf(const std::string& cpf);
        
        void exibir_menu();
        void processar_comando(const std::string& comando);
        
        void fluxo_cadastro();
        void fluxo_login();
        void fluxo_logout();
        void fluxo_oferecer_carona();
        void fluxo_buscar_caronas();
        void fluxo_cadastrar_veiculo();

    public:
        Sistema();
        ~Sistema();
        void executar();
    };
}
#endif