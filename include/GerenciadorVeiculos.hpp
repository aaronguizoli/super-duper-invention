#ifndef GERENCIADOR_VEICULOS_HPP
#define GERENCIADOR_VEICULOS_HPP

#include <string>
#include <vector>

namespace ufmg_carona {
    class TerminalIO;
    class GerenciadorUsuarios;
    class Usuario;
    class Motorista;
    class Veiculo;

    class GerenciadorVeiculos {
    private:
        TerminalIO* _terminal_io;
        GerenciadorUsuarios* _ger_usuarios;

        void salvarDados(GerenciadorUsuarios* ger_usuarios);
        void carregarDados(GerenciadorUsuarios* ger_usuarios);

    public:
        GerenciadorVeiculos(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios);
        ~GerenciadorVeiculos();

        void cadastrarVeiculo(Motorista* motorista);
        void editarVeiculo(Motorista* motorista);
        void excluirVeiculo(Motorista* motorista);
        void gerenciarVeiculos(Motorista* motorista);
        Veiculo* buscarVeiculoPorPlacaMotorista(Motorista* motorista, const std::string& placa) const;

        void fluxo_cadastrar_veiculo(Usuario* usuario_logado);
        void fluxo_gerenciar_veiculos(Usuario* usuario_logado);
        void fluxo_editar_veiculo_externo(Motorista* motorista_logado);
        void fluxo_excluir_veiculo_externo(Motorista* motorista_logado);

        void carregarDadosVeiculosPublico(GerenciadorUsuarios* ger_usuarios);
        void salvarDadosVeiculosPublico(GerenciadorUsuarios* ger_usuarios);
    };
}
#endif