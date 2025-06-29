#ifndef GERENCIADOR_SOLICITACOES_HPP
#define GERENCIADOR_SOLICITACOES_HPP

#include <vector>
#include <string>
#include "Solicitacao.hpp" // Inclui Solicitacao para ter acesso ao enum StatusSolicitacao

namespace ufmg_carona {
    // Forward declarations
    class TerminalIO;
    class GerenciadorUsuarios;
    class GerenciadorCaronas;
    class Usuario;
    class Motorista;
    class Carona;
    class Solicitacao; // Já incluído acima, mas bom manter a consistência

    class GerenciadorSolicitacoes {
    private:
        TerminalIO* _terminal_io;
        GerenciadorUsuarios* _ger_usuarios;
        GerenciadorCaronas* _ger_caronas;
        std::vector<Solicitacao*> _solicitacoes; // Todas as solicitações

        void salvarDados();
        void carregarDados(); // Alterado para receber dependências

    public:
        GerenciadorSolicitacoes(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios, GerenciadorCaronas* ger_caronas);
        ~GerenciadorSolicitacoes();

        void solicitarCarona(Usuario* passageiro);
        void statusMinhasSolicitacoes(Usuario* passageiro);
        void solicitacoesPendentesMotorista(Motorista* motorista);
        bool podeSolicitarCarona(Usuario* passageiro, const Carona& carona);
        void cancelarOutrasSolicitacoesPassageiro(Usuario* passageiro, const Carona& carona_aceita);
        
        // Métodos auxiliares para o Sistema
        void fluxo_solicitar_carona(Usuario* usuario_logado);
        void fluxo_status_caronas(Usuario* usuario_logado);
        void fluxo_solicitacoes_pendentes_motorista_externo(Usuario* usuario_logado); // Para ser chamado do GerenciadorCaronas

        // Funções para carregar/salvar dados em conjunto com o Sistema
        void carregarDadosSolicitacoesPublico(GerenciadorUsuarios* ger_usuarios, GerenciadorCaronas* ger_caronas); // Método público para carregar
        void salvarDadosSolicitacoesPublico(); // Método público para salvar

        // Getter para as solicitacoes (necessário para o GerenciadorAvaliacoes)
        const std::vector<Solicitacao*>& getSolicitacoes() const;
    };
} // namespace ufmg_carona

#endif // GERENCIADOR_SOLICITACOES_HPP