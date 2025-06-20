#ifndef CARONA_HPP
#define CARONA_HPP
#include <string>
#include <vector>

namespace ufmg_carona {
    class Usuario;
    class Solicitacao;
    class Motorista;
    class Veiculo;

    enum class TipoCarona { AGENDADA, IMEDIATA };
    enum class StatusCarona { AGUARDANDO, LOTADA, EM_VIAGEM, FINALIZADA, CANCELADA };

    class Carona {
    private:
        int _id;
        static int _proximo_id;
        std::string _origem, _destino, _data_hora_partida;
        Usuario* _motorista;
        Veiculo* _veiculo_usado;
        std::vector<Usuario*> _passageiros;
        std::vector<Solicitacao*> _solicitacoes_pendentes;
        int _vagas_disponiveis;
        bool _apenas_mulheres;
        StatusCarona _status;
        TipoCarona _tipo;

    public:
        Carona(std::string origem, std::string destino, std::string data, Usuario* motorista, Veiculo* veiculo_usado, bool apenas_mulheres, TipoCarona tipo);
        static int gerar_proximo_id();

        int get_id() const;
        Usuario* get_motorista() const;
        Veiculo* get_veiculo_usado() const;
        const std::string& get_origem() const;
        const std::string& get_destino() const;
        const std::string& get_data_hora() const;
        int get_vagas_disponiveis() const;
        bool get_apenas_mulheres() const;

        void exibir_info() const;
        void exibir_info_detalhada() const;

        void adicionar_solicitacao(Solicitacao* solicitacao);
        const std::vector<Solicitacao*>& get_solicitacoes_pendentes() const;
        bool tem_solicitacoes_pendentes() const;

        void adicionar_passageiro(Usuario* passageiro);
        void remover_passageiro(Usuario* passageiro);
    };
}
#endif