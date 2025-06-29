#ifndef GERENCIADOR_SOLICITACOES_HPP
#define GERENCIADOR_SOLICITACOES_HPP

#include <vector>
#include <string>
#include "Solicitacao.hpp"

namespace ufmg_carona {
    class TerminalIO;
    class GerenciadorUsuarios;
    class GerenciadorCaronas;
    class Usuario;
    class Motorista;
    class Carona;
    class Solicitacao;

    class GerenciadorSolicitacoes {
    private:
        TerminalIO* _terminal_io;
        GerenciadorUsuarios* _ger_usuarios;
        GerenciadorCaronas* _ger_caronas;
        std::vector<Solicitacao*> _solicitacoes;

        void salvarDados();
        void carregarDados();

    public:
        GerenciadorSolicitacoes(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios, GerenciadorCaronas* ger_caronas = nullptr);
        ~GerenciadorSolicitacoes();

        void setGerenciadorCaronas(GerenciadorCaronas* ger_caronas);

        void solicitarCarona(Usuario* passageiro);
        void statusMinhasSolicitacoes(Usuario* passageiro);
        void solicitacoesPendentesMotorista(Motorista* motorista);
        bool podeSolicitarCarona(Usuario* passageiro, const Carona& carona);
        void cancelarOutrasSolicitacoesPassageiro(Usuario* passageiro, const Carona& carona_aceita);
        
        void fluxo_solicitar_carona(Usuario* usuario_logado);
        void fluxo_status_caronas(Usuario* usuario_logado);
        void fluxo_solicitacoes_pendentes_motorista_externo(Usuario* usuario_logado);

        void carregarDadosSolicitacoesPublico(GerenciadorUsuarios* ger_usuarios, GerenciadorCaronas* ger_caronas);
        void salvarDadosSolicitacoesPublico();

        const std::vector<Solicitacao*>& getSolicitacoes() const;
    };
}
#endif