#ifndef CARONA_HPP
#define CARONA_HPP
#include <string>
#include <vector>
#include <memory>

namespace ufmg_carona {
    class Usuario;
    class Solicitacao;
    enum class TipoCarona { AGENDADA, IMEDIATA };
    enum class StatusCarona { AGUARDANDO, LOTADA, EM_VIAGEM, FINALIZADA, CANCELADA };

    class Carona {
    private:
        int _id;
        static int _proximo_id;
        std::string _origem, _destino, _data_hora_partida;
        std::shared_ptr<Usuario> _motorista;
        std::vector<std::shared_ptr<Usuario>> _passageiros;
        std::vector<std::shared_ptr<Solicitacao>> _solicitacoes_pendentes;
        int _vagas_disponiveis;
        bool _apenas_mulheres;
        StatusCarona _status;
        TipoCarona _tipo;
        
    public:
        Carona(std::string origem, std::string destino, std::string data, std::shared_ptr<Usuario> motorista, bool apenas_mulheres, TipoCarona tipo);
        static int gerar_proximo_id();
        
        // Getters
        int get_id() const;
        std::shared_ptr<Usuario> get_motorista() const;
        const std::string& get_origem() const;
        const std::string& get_destino() const;
        const std::string& get_data_hora() const;
        int get_vagas_disponiveis() const;
        bool get_apenas_mulheres() const;
        
        // Métodos de exibição
        void exibir_info() const;
        void exibir_info_detalhada() const;
        
        // Gerenciamento de solicitações
        void adicionar_solicitacao(std::shared_ptr<Solicitacao> solicitacao);
        const std::vector<std::shared_ptr<Solicitacao>>& get_solicitacoes_pendentes() const;
        bool tem_solicitacoes_pendentes() const;
        
        // Gerenciamento de passageiros
        void adicionar_passageiro(std::shared_ptr<Usuario> passageiro);
        void remover_passageiro(std::shared_ptr<Usuario> passageiro);
    };
}
#endif
