#ifndef NOTIFICACAO_HPP
#define NOTIFICACAO_HPP
#include <string>
namespace ufmg_carona {
    class Notificacao {
    private: std::string _mensagem; bool _lida;
    public: Notificacao(std::string mensagem); void marcar_como_lida();
    };
}
#endif