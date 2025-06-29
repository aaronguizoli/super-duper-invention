#ifndef GERENCIADOR_AVALIACOES_HPP
#define GERENCIADOR_AVALIACOES_HPP

#include <vector>
#include <string>

namespace ufmg_carona {
    class TerminalIO;
    class GerenciadorUsuarios;
    class GerenciadorCaronas;
    class GerenciadorSolicitacoes;
    class Usuario;
    class Motorista;
    class Avaliacao;
    class Carona;
    class Solicitacao;

    class GerenciadorAvaliacoes {
    private:
        TerminalIO* _terminal_io;
        GerenciadorUsuarios* _ger_usuarios;
        GerenciadorCaronas* _ger_caronas;
        GerenciadorSolicitacoes* _ger_solicitacoes;
        std::vector<Avaliacao*> _avaliacoes_globais;

    public:
        void salvarDados();
        void carregarDados();

    public:
        GerenciadorAvaliacoes(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios,
                              GerenciadorCaronas* ger_caronas, GerenciadorSolicitacoes* ger_solicitacoes);
        ~GerenciadorAvaliacoes();

        void avaliarCaronaPassageiro(Usuario* passageiro);
        void avaliarPassageirosMotorista(Motorista* motorista);
        void exibirMinhasAvaliacoesRecebidas(Usuario* usuario) const;
        void exibirAvaliacoesQueFiz(Usuario* usuario) const;

        void fluxo_avaliacoes(Usuario* usuario_logado);
    };
}
#endif