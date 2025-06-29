#ifndef GERENCIADOR_CARONAS_HPP
#define GERENCIADOR_CARONAS_HPP

#include <vector>
#include <string>
#include "Carona.hpp"

namespace ufmg_carona {
    class TerminalIO;
    class GerenciadorUsuarios;
    class GerenciadorVeiculos;
    class GerenciadorSolicitacoes;
    class Motorista;
    class Carona;
    class Usuario;
    class Veiculo;

    class GerenciadorCaronas {
    private:
        TerminalIO* _terminal_io;
        GerenciadorUsuarios* _ger_usuarios;
        GerenciadorVeiculos* _ger_veiculos;
        GerenciadorSolicitacoes* _ger_solicitacoes;
        std::vector<Carona> _caronas;
        static int _proximo_id_carona;

        void salvarDados();
        void carregarDados();

    public:
        GerenciadorCaronas(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios, GerenciadorVeiculos* ger_veiculos, GerenciadorSolicitacoes* ger_solicitacoes = nullptr);
        ~GerenciadorCaronas();

        void setGerenciadorSolicitacoes(GerenciadorSolicitacoes* ger_solicitacoes);

        void oferecerCarona(Motorista* motorista);
        Carona* buscarCaronaPorId(int id);
        void removerCaronasPassadas();
        void finalizarCarona(Carona* carona);
        void cancelarCarona(Carona* carona);
        void gerenciarCaronas(Motorista* motorista);
        const std::vector<Carona>& getTodasCaronas() const;

        void fluxo_oferecer_carona(Usuario* usuario_logado);
        void fluxo_gerenciar_caronas(Motorista* motorista_logado);
        void fluxo_minhas_caronas(Motorista* motorista_logado);

        void carregarDadosCaronasPublico(GerenciadorUsuarios* ger_usuarios, GerenciadorVeiculos* ger_veiculos);
        void salvarDadosCaronasPublico();
    };
}
#endif