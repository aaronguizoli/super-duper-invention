#ifndef CARONA_HPP
#define CARONA_HPP
#include <string>
#include <vector>

namespace ufmg_carona {
    class Usuario;
    class Solicitacao;
    class Motorista;
    class Veiculo; // Incluir Veiculo para poder armazenar um ponteiro para ele

    enum class TipoCarona { AGENDADA, IMEDIATA };
    enum class StatusCarona { AGUARDANDO, LOTADA, EM_VIAGEM, FINALIZADA, CANCELADA };

    class Carona {
    private:
        int _id;
        static int _proximo_id;
        std::string _origem, _destino, _data_hora_partida;
        Usuario* _motorista; // Carona nao eh proprietaria do motorista
        Veiculo* _veiculo_usado; // Ponteiro para o veiculo usado nesta carona
        std::vector<Usuario*> _passageiros;
        std::vector<Solicitacao*> _solicitacoes_pendentes;
        int _vagas_disponiveis;
        bool _apenas_mulheres;
        StatusCarona _status;
        TipoCarona _tipo;

    public:
        // Construtor ATUALIZADO: Recebe um ponteiro para o Veiculo que sera usado
        Carona(std::string origem, std::string destino, std::string data, Usuario* motorista, Veiculo* veiculo_usado, bool apenas_mulheres, TipoCarona tipo);
        static int gerar_proximo_id();

        // Getters
        int get_id() const;
        Usuario* get_motorista() const;
        Veiculo* get_veiculo_usado() const; // Getter para o veiculo usado
        const std::string& get_origem() const;
        const std::string& get_destino() const;
        const std::string& get_data_hora() const;
        int get_vagas_disponiveis() const;
        bool get_apenas_mulheres() const;

        // Metodos de exibicao
        void exibir_info() const;
        void exibir_info_detalhada() const;

        // Gerenciamento de solicitacoes
        void adicionar_solicitacao(Solicitacao* solicitacao);
        const std::vector<Solicitacao*>& get_solicitacoes_pendentes() const;
        bool tem_solicitacoes_pendentes() const;

        // Gerenciamento de passageiros
        void adicionar_passageiro(Usuario* passageiro);
        void remover_passageiro(Usuario* passageiro);
    };
}
#endif