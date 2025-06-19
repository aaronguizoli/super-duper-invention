#ifndef CARONA_HPP
#define CARONA_HPP
#include <string>
#include <vector>
// #include <memory> // Removido

namespace ufmg_carona {
    class Usuario; // Declaração antecipada
    class Solicitacao; // Declaração antecipada
    enum class TipoCarona { AGENDADA, IMEDIATA };
    enum class StatusCarona { AGUARDANDO, LOTADA, EM_VIAGEM, FINALIZADA, CANCELADA };

    class Carona {
    private:
        int _id;
        static int _proximo_id;
        std::string _origem, _destino, _data_hora_partida;
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Usuario* _motorista; // Carona não é proprietária do motorista
        // ALTERAÇÃO: De std::vector<std::shared_ptr<Usuario>> para std::vector<Usuario*>
        std::vector<Usuario*> _passageiros; // Carona não é proprietária dos passageiros
        // ALTERAÇÃO: De std::vector<std::shared_ptr<Solicitacao>> para std::vector<Solicitacao*>
        std::vector<Solicitacao*> _solicitacoes_pendentes; // Carona não é proprietária das solicitações
        int _vagas_disponiveis;
        bool _apenas_mulheres;
        StatusCarona _status;
        TipoCarona _tipo;

    public:
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Carona(std::string origem, std::string destino, std::string data, Usuario* motorista, bool apenas_mulheres, TipoCarona tipo);
        // Não é necessário destrutor customizado, pois Carona não é proprietária dos ponteiros brutos que armazena.
        // Eles são gerenciados pela classe Sistema.
        static int gerar_proximo_id();

        // Getters
        int get_id() const;
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Usuario* get_motorista() const;
        const std::string& get_origem() const;
        const std::string& get_destino() const;
        const std::string& get_data_hora() const;
        int get_vagas_disponiveis() const;
        bool get_apenas_mulheres() const;

        // Métodos de exibição
        void exibir_info() const;
        void exibir_info_detalhada() const;

        // Gerenciamento de solicitações
        // ALTERAÇÃO: De std::shared_ptr<Solicitacao> para Solicitacao*
        void adicionar_solicitacao(Solicitacao* solicitacao);
        // ALTERAÇÃO: De const std::vector<std::shared_ptr<Solicitacao>>& para const std::vector<Solicitacao*>&
        const std::vector<Solicitacao*>& get_solicitacoes_pendentes() const;
        bool tem_solicitacoes_pendentes() const;

        // Gerenciamento de passageiros
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        void adicionar_passageiro(Usuario* passageiro);
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        void remover_passageiro(Usuario* passageiro);
    };
}
#endif