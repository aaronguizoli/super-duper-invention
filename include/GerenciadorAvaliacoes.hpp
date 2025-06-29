#ifndef GERENCIADOR_AVALIACOES_HPP
#define GERENCIADOR_AVALIACOES_HPP

#include <vector>
#include <string>

namespace ufmg_carona {
    // Forward declarations para evitar inclusões circulares e dependências desnecessárias
    class TerminalIO;
    class GerenciadorUsuarios;
    class GerenciadorCaronas;
    class GerenciadorSolicitacoes;
    class Usuario;
    class Motorista;
    class Avaliacao;
    class Carona;
    class Solicitacao; // Adicionado para poder gerenciar as flags de avaliação em Solicitacao

    class GerenciadorAvaliacoes {
    private:
        TerminalIO* _terminal_io;
        GerenciadorUsuarios* _ger_usuarios;
        GerenciadorCaronas* _ger_caronas;
        GerenciadorSolicitacoes* _ger_solicitacoes; // Adicionado para acesso às solicitações
        std::vector<Avaliacao*> _avaliacoes_globais; // Avaliações podem ser armazenadas globalmente aqui

    public: // <--- ALTERADO: Métodos tornados públicos para acesso pelo Sistema
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

        // Métodos auxiliares para o Sistema
        void fluxo_avaliacoes(Usuario* usuario_logado);
    };
} // namespace ufmg_carona

#endif // GERENCIADOR_AVALIACOES_HPP