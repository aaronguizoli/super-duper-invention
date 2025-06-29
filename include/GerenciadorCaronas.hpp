#ifndef GERENCIADOR_CARONAS_HPP
#define GERENCIADOR_CARONAS_HPP

#include <vector>
#include <string>
#include "Carona.hpp" // Inclui Carona para ter acesso ao enum StatusCarona

namespace ufmg_carona {
    // Forward declarations
    class TerminalIO;
    class GerenciadorUsuarios;
    class GerenciadorVeiculos;
    class GerenciadorSolicitacoes; // <--- ADICIONADO
    class Motorista;
    class Carona; // Já incluído acima, mas bom manter a consistência
    class Usuario; // Para passagem de motorista como Usuario* no construtor da carona
    class Veiculo; // Para passagem de veiculo_usado no construtor da carona

    class GerenciadorCaronas {
    private:
        TerminalIO* _terminal_io;
        GerenciadorUsuarios* _ger_usuarios;
        GerenciadorVeiculos* _ger_veiculos;
        GerenciadorSolicitacoes* _ger_solicitacoes; // <--- ADICIONADO
        std::vector<Carona> _caronas;
        static int _proximo_id_carona; // Para manter o controle global dos IDs de carona

        void salvarDados(); // Método interno
        void carregarDados(); // Método interno

    public:
        // <--- CONSTRUTOR ATUALIZADO
        GerenciadorCaronas(TerminalIO* terminal_io, GerenciadorUsuarios* ger_usuarios, GerenciadorVeiculos* ger_veiculos, GerenciadorSolicitacoes* ger_solicitacoes);
        ~GerenciadorCaronas();

        void oferecerCarona(Motorista* motorista);
        Carona* buscarCaronaPorId(int id);
        void removerCaronasPassadas();
        void finalizarCarona(Carona* carona);
        void cancelarCarona(Carona* carona);
        void gerenciarCaronas(Motorista* motorista);
        const std::vector<Carona>& getTodasCaronas() const;

        // Métodos auxiliares para o Sistema
        void fluxo_oferecer_carona(Usuario* usuario_logado);
        void fluxo_gerenciar_caronas(Motorista* motorista_logado);
        void fluxo_minhas_caronas(Motorista* motorista_logado);

        // Funções para carregar/salvar dados em conjunto com o Sistema
        void carregarDadosCaronasPublico(GerenciadorUsuarios* ger_usuarios, GerenciadorVeiculos* ger_veiculos); // Método público para carregar
        void salvarDadosCaronasPublico(); // Método público para salvar
    };
} // namespace ufmg_carona

#endif // GERENCIADOR_CARONAS_HPP