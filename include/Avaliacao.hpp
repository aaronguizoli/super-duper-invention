#ifndef AVALIACAO_HPP
#define AVALIACAO_HPP
#include <string>
// #include <memory> // Removido
namespace ufmg_carona {
    class Usuario; // Declaração antecipada, pois usaremos ponteiros para Usuario
    class Avaliacao {
    private:
        int _nota;
        std::string _comentario;
        // ALTERAÇÃO: De std::weak_ptr<Usuario> para Usuario*
        // A Avaliacao NÃO é proprietária desses ponteiros, apenas os observa.
        Usuario* _avaliador;
        Usuario* _avaliado;
    public:
        // ALTERAÇÃO: De std::shared_ptr<Usuario> para Usuario*
        Avaliacao(int nota, std::string comentario, Usuario* avaliador, Usuario* avaliado);
        // Não é necessário destrutor aqui, pois Avaliacao não é proprietária de Usuario*.
        // A memória dos objetos Usuario é gerenciada por outra classe (Sistema).
        int get_nota() const;
    };
}
#endif