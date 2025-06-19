#include "Rotina.hpp"

namespace ufmg_carona {
    Rotina::Rotina(DiaDaSemana dia, const std::string& horario, const std::string& local, const std::string& destino)
        : _dia(dia), _horario_saida(horario), _local_saida_padrao(local), _destino_final(destino) {}
    Rotina::Rotina() : _dia(DiaDaSemana::DOMINGO) {}
}