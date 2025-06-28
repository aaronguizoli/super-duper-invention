#ifndef AVALIACAO_HPP
#define AVALIACAO_HPP
#include <string>
namespace ufmg_carona {
    class Usuario; 
    class Avaliacao {
    private:
        int _nota;
        std::string _comentario;
        Usuario* _avaliador;
        Usuario* _avaliado;
    public:
        Avaliacao(int nota, std::string comentario, Usuario* avaliador, Usuario* avaliado);
        int get_nota() const;
    };
}
#endif