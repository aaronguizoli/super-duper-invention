#include "Notificacao.hpp"

namespace ufmg_carona {
    Notificacao::Notificacao(std::string mensagem) : _mensagem(mensagem), _lida(false) {}
    std::string Notificacao::get_mensagem() const { return _mensagem; }
    bool Notificacao::foi_lida() const { return _lida; }
    void Notificacao::marcar_como_lida() { _lida = true; }
}