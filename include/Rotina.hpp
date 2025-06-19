#ifndef ROTINA_HPP
#define ROTINA_HPP
#include <string>

namespace ufmg_carona {
    enum class DiaDaSemana { DOMINGO, SEGUNDA, TERCA, QUARTA, QUINTA, SEXTA, SABADO };

    class Rotina {
    private:
        DiaDaSemana _dia;
        std::string _horario_saida;
        std::string _local_saida_padrao;
        std::string _destino_final;
    public:
        Rotina(DiaDaSemana dia, const std::string& horario, const std::string& local, const std::string& destino);
        Rotina();
    };
}
#endif