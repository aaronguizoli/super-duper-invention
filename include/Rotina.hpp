#ifndef ROTINA_HPP
#define ROTINA_HPP
#include <string>
#include <vector>

namespace ufmg_carona {
    enum class DiaDaSemana { DOMINGO, SEGUNDA, TERCA, QUARTA, QUINTA, SEXTA, SABADO };

    class Rotina {
    private:
        std::vector<DiaDaSemana> _dias_da_semana;
        std::string _horario_saida;
        std::string _local_saida_padrao;
        std::string _destino_final;
        std::string _placa_veiculo_usado;
        bool _apenas_mulheres;

    public:
        Rotina(const std::vector<DiaDaSemana>& dias, const std::string& horario, const std::string& local, const std::string& destino, const std::string& placa_veiculo, bool apenas_mulheres);
        Rotina();

        const std::vector<DiaDaSemana>& get_dias() const;
        const std::string& get_horario_saida() const;
        const std::string& get_local_saida_padrao() const;
        const std::string& get_destino_final() const;
        const std::string& get_placa_veiculo_usado() const;
        bool get_apenas_mulheres() const;

        bool aplica_se_ao_dia(DiaDaSemana dia) const;
    };
}
#endif