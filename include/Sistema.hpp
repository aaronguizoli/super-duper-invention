#ifndef SISTEMA_HPP
#define SISTEMA_HPP

#include <string>
#include <tuple>

namespace ufmg_carona {
    class GerenciadorUsuarios;
    class GerenciadorCaronas;
    class GerenciadorSolicitacoes;
    class GerenciadorAvaliacoes;
    class GerenciadorVeiculos;
    class TerminalIO;
    class Usuario;
    class Motorista;
}

namespace ufmg_carona {
    class Sistema {
    private:
        GerenciadorUsuarios* _gerenciadorUsuarios;
        GerenciadorCaronas* _gerenciadorCaronas;
        GerenciadorSolicitacoes* _gerenciadorSolicitacoes;
        GerenciadorAvaliacoes* _gerenciadorAvaliacoes;
        GerenciadorVeiculos* _gerenciadorVeiculos;
        TerminalIO* _terminalIO;

        Usuario* _usuario_logado;

        void carregarTodosDados();
        void salvarTodosDados();
        
        std::tuple<bool, std::string, std::string, std::string, std::string> buscarDadosUfmgPorCpf(const std::string& cpf_buscado);

        void exibirMenuInicialNaoLogado();
        void exibirMenuLogado();
        void exibirMenuPassageiro();
        void exibirMenuMotorista();

        void processarComandoLogado(const std::string& comando_str);

        void fluxo_cadastro();
        void fluxo_login();
        void fluxo_logout();
        void fluxo_editar_perfil_ou_veiculos();
        
    public:
        Sistema();
        ~Sistema();
        void executar();
    };
}
#endif