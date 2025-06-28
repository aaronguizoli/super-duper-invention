#ifndef SOLICITACAO_HPP
#define SOLICITACAO_HPP
#include <string>

namespace ufmg_carona {
    class Usuario;
    class Carona;

    enum class StatusSolicitacao {
        PENDENTE,
        ACEITA,
        RECUSADA,
        AGUARDANDO_RESPOSTA_PASSAGEIRO,
        RECUSADA_PROPOSTA_MOTORISTA
    };

    class Solicitacao {
    private:
        Usuario* _passageiro;
        Carona* _carona_alvo;
        StatusSolicitacao _status;
        std::string _local_embarque_passageiro;
        std::string _local_desembarque_passageiro;
        std::string _local_embarque_motorista_proposto;
        std::string _local_desembarque_motorista_proposto;


    public:
        Solicitacao(Usuario* passageiro, Carona* carona, std::string local_embarque_p, std::string local_desembarque_p);
        
        void aceitar();
        void recusar();
        void propor_locais_motorista(std::string local_embarque_m, std::string local_desembarque_m);
        void aceitar_proposta_motorista();
        void recusar_proposta_motorista();
        
        void set_status(StatusSolicitacao novo_status);
        void set_carona(Carona* new_carona_ptr); 


        Usuario* get_passageiro() const;
        Carona* get_carona() const;
        StatusSolicitacao get_status() const;
        std::string get_status_string() const;

        const std::string& get_local_embarque_passageiro() const;
        const std::string& get_local_desembarque_passageiro() const;
        const std::string& get_local_embarque_motorista_proposto() const;
        const std::string& get_local_desembarque_motorista_proposto() const;

        void exibir_info() const;
        void exibir_para_motorista() const;
    };
}
#endif