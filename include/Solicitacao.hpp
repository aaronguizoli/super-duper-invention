#ifndef SOLICITACAO_HPP
#define SOLICITACAO_HPP

#include <memory>

namespace ufmg_carona {
    // Forward declarations para evitar inclusões circulares
    class Usuario;
    class Carona;

    enum class StatusSolicitacao {
        PENDENTE,
        ACEITA,
        RECUSADA
    };

    /**
     * @class Solicitacao
     * @brief Gerencia um pedido de um passageiro para entrar em uma carona.
     */
    class Solicitacao {
    private:
        std::shared_ptr<Usuario> _passageiro;
        Carona* _carona_alvo; // Ponteiro simples, pois o Sistema é o dono da Carona
        StatusSolicitacao _status; // <- ATRIBUTO QUE FALTAVA

    public:
        Solicitacao(std::shared_ptr<Usuario> passageiro, Carona* carona);

        void aceitar();
        void recusar();
        StatusSolicitacao get_status() const; // <- MÉTODO QUE FALTAVA
    };
}

#endif // SOLICITACAO_HPP