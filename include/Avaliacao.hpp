#ifndef AVALIACAO_HPP
#define AVALIACAO_HPP
#include <string>
#include <memory>

namespace ufmg_carona {
    class Usuario;
    class Avaliacao {
    private:
        int _nota;
        std::string _comentario;
        std::weak_ptr<Usuario> _avaliador;
        std::weak_ptr<Usuario> _avaliado;
    public:
        Avaliacao(int nota, std::string comentario, std::shared_ptr<Usuario> avaliador, std::shared_ptr<Usuario> avaliado);
        int get_nota() const;
    };
}
#endif