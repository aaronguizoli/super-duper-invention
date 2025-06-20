#include "Rotina.hpp"
#include <algorithm>

namespace ufmg_carona {
    Rotina::Rotina(const std::vector<DiaDaSemana>& dias, const std::string& h, const std::string& l, const std::string& d, const std::string& pv, bool am)
        : _dias_da_semana(dias), _horario_saida(h), _local_saida_padrao(l), _destino_final(d), _placa_veiculo_usado(pv), _apenas_mulheres(am) {}
    
    Rotina::Rotina() : _apenas_mulheres(false) {}

    const std::vector<DiaDaSemana>& Rotina::get_dias() const { return _dias_da_semana; }
    const std::string& Rotina::get_horario_saida() const { return _horario_saida; }
    const std::string& Rotina::get_local_saida_padrao() const { return _local_saida_padrao; }
    const std::string& Rotina::get_destino_final() const { return _destino_final; }
    const std::string& Rotina::get_placa_veiculo_usado() const { return _placa_veiculo_usado; }
    bool Rotina::get_apenas_mulheres() const { return _apenas_mulheres; }

    bool Rotina::aplica_se_ao_dia(DiaDaSemana dia) const {
        return std::find(_dias_da_semana.begin(), _dias_da_semana.end(), dia) != _dias_da_semana.end();
    }
}