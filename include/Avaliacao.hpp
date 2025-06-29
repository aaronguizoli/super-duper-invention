#ifndef AVALIACAO_HPP
#define AVALIACAO_HPP
#include <string>
namespace ufmg_carona {
    class Usuario; 
    class Carona;
    class Avaliacao {
    private:
        int _nota;
        std::string _comentario;
        Usuario* _avaliador;
        Usuario* _avaliado;
        Carona* _carona_referencia;
    public:
        Avaliacao(int nota, std::string comentario, Usuario* avaliador, Usuario* avaliado, Carona* carona_ref);
        int get_nota() const;
        const std::string& get_comentario() const;
        Usuario* get_avaliador() const;
        Usuario* get_avaliado() const;
        Carona* get_carona_referencia() const;
    };
}
#endif