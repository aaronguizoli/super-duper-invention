#ifndef SISTEMA_HPP
#define SISTEMA_HPP

#include <string>
#include <tuple> // Para buscarDadosUfmgPorCpf

// Forward declarations para as novas classes de gerenciamento
namespace ufmg_carona {
    class GerenciadorUsuarios;
    class GerenciadorCaronas;
    class GerenciadorSolicitacoes;
    class GerenciadorAvaliacoes;
    class GerenciadorVeiculos; // Adicionado
    class TerminalIO;
    class Usuario; // Necessário para _usuario_logado
    class Motorista; // Necessário para downcast
}

namespace ufmg_carona {
    class Sistema {
    private:
        GerenciadorUsuarios* _gerenciadorUsuarios;
        GerenciadorCaronas* _gerenciadorCaronas;
        GerenciadorSolicitacoes* _gerenciadorSolicitacoes;
        GerenciadorAvaliacoes* _gerenciadorAvaliacoes;
        GerenciadorVeiculos* _gerenciadorVeiculos;
        TerminalIO* _terminalIO; // Adicionado

        Usuario* _usuario_logado; // Permanece aqui, gerenciado pelo Sistema

        void carregarTodosDados();
        void salvarTodosDados();
        
        // MANTIDO COMO NÃO-ESTÁTICO, ACESSANDO _terminalIO
        std::tuple<bool, std::string, std::string, std::string, std::string> buscarDadosUfmgPorCpf(const std::string& cpf_buscado);

        // Funções de menu movidas para TerminalIO, mas mantidas as chamadas de orquestração aqui
        // Estas são chamadas de métodos de TerminalIO que exibem o menu.
        void exibirMenuInicialNaoLogado();
        void exibirMenuLogado();
        void exibirMenuPassageiro();
        void exibirMenuMotorista();

        // Roteador de comandos para usuário logado
        void processarComandoLogado(const std::string& comando_str);

        // Fluxos de alto nível que delegam para os gerenciadores
        void fluxo_cadastro();
        void fluxo_login();
        void fluxo_logout();
        void fluxo_editar_perfil_ou_veiculos();
        
    public:
        Sistema();
        ~Sistema();
        void executar();
    };
} // namespace ufmg_carona

#endif // SISTEMA_HPP