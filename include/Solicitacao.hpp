#ifndef SOLICITACAO_HPP
#define SOLICITACAO_HPP
#include <memory>
#include <string>

namespace ufmg_carona {
    class Usuario;
    class Carona;
    enum class StatusSolicitacao { PENDENTE, ACEITA, RECUSADA };
    
    class Solicitacao {
    private: 
        std::shared_ptr<Usuario> _passageiro; 
        Carona* _carona_alvo; 
        StatusSolicitacao _status;
        
    public: 
        Solicitacao(std::shared_ptr<Usuario> passageiro, Carona* carona); 
        void aceitar(); 
        void recusar();
        
        // Getters
        std::shared_ptr<Usuario> get_passageiro() const;
        Carona* get_carona() const;
        StatusSolicitacao get_status() const;
        std::string get_status_string() const;
        
        // Métodos de exibição
        void exibir_info() const;
        void exibir_para_motorista() const;
    };
}
#endif
