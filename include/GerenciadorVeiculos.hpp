#ifndef GERENCIADOR_VEICULOS_HPP
#define GERENCIADOR_VEICULOS_HPP

#include <string>
#include <vector> // Necessário para get_veiculos() do Motorista

namespace ufmg_carona {
    // Forward declarations
    class TerminalIO;
    class GerenciadorUsuarios; // <--- ADICIONADO: Para poder acessar a lista de usuários e seus veículos
    class Usuario; // <--- ADICIONADO (Já estava na versão anterior, apenas para manter a visibilidade da adição)
    class Motorista;
    class Veiculo; // Já incluído acima, mas bom manter a consistência

    class GerenciadorVeiculos {
    private:
        TerminalIO* _terminal_io;
        GerenciadorUsuarios* _ger_usuarios; // <--- ADICIONADO COMO MEMBRO
        // Não precisamos de uma coleção interna de veículos aqui, pois eles são parte do Motorista.
        // A responsabilidade é gerenciar os veículos de um dado motorista.

        void salvarDados(GerenciadorUsuarios* ger_usuarios); // Interno
        void carregarDados(GerenciadorUsuarios* ger_usuarios); // Interno

    public:
        // <--- CONSTRUTOR ATUALIZADO
        GerenciadorVeiculos(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios);
        ~GerenciadorVeiculos(); // O destrutor aqui não precisa liberar Veiculos, pois Motorista já faz isso.

        void cadastrarVeiculo(Motorista* motorista);
        void editarVeiculo(Motorista* motorista);
        void excluirVeiculo(Motorista* motorista);
        void gerenciarVeiculos(Motorista* motorista);
        Veiculo* buscarVeiculoPorPlacaMotorista(Motorista* motorista, const std::string& placa) const;

        // Métodos auxiliares para o Sistema
        void fluxo_cadastrar_veiculo(Usuario* usuario_logado); // Recebe Usuario* para verificar se é motorista
        void fluxo_gerenciar_veiculos(Usuario* usuario_logado); // Recebe Usuario* para verificar se é motorista
        void fluxo_editar_veiculo_externo(Motorista* motorista_logado); // Chamado de gerenciar
        void fluxo_excluir_veiculo_externo(Motorista* motorista_logado); // Chamado de gerenciar

        // Funções para carregar/salvar dados em conjunto com o Sistema
        void carregarDadosVeiculosPublico(GerenciadorUsuarios* ger_usuarios); // Método público para carregar
        void salvarDadosVeiculosPublico(GerenciadorUsuarios* ger_usuarios); // Método público para salvar
    };
} // namespace ufmg_carona

#endif // GERENCIADOR_VEICULOS_HPP