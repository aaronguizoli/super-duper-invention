#include "Notificacao.hpp"
namespace ufmg_carona {
    Notificacao::Notificacao(std::string mensagem) : _mensagem(mensagem), _lida(false) {}
    void Notificacao::marcar_como_lida() { _lida = true; }
}