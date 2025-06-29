#ifndef UTILITARIOS_HPP
#define UTILITARIOS_HPP

#include <string>
#include <ctime> // Para std::tm
#include <map>   // Para as conversões de Zona/UFMGPosicao

namespace ufmg_carona {
    // Forward declarations de enums
    enum class Zona;
    enum class UFMGPosicao;

    class Utilitarios {
    private:
        // Mapeamentos para conversão de enums
        std::map<int, Zona> _int_para_zona;
        std::map<Zona, std::string> _zona_para_string;
        std::map<int, UFMGPosicao> _int_para_ufmg_posicao;
        std::map<UFMGPosicao, std::string> _ufmg_posicao_para_string;

    public:
        Utilitarios(); // Construtor para inicializar os mapas

        std::tm parseDatetimeString(const std::string& dt_str) const;
        std::string getCurrentDatetimeString() const;
        bool isDatetimeInPast(const std::string& dt_str) const;

        std::string zonaToString(Zona z) const;
        Zona stringToZona(const std::string& s) const;
        std::string ufmgPosicaoToString(UFMGPosicao up) const;
        UFMGPosicao stringToUfmgPosicao(const std::string& s) const;

        // Métodos para acessar os mapas de zona/posicao
        const std::map<int, Zona>& getIntParaZona() const;
        const std::map<Zona, std::string>& getZonaParaString() const;
        const std::map<int, UFMGPosicao>& getIntParaUfmgPosicao() const;
        const std::map<UFMGPosicao, std::string>& getUfmgPosicaoParaString() const;
    };
} // namespace ufmg_carona

#endif // UTILITARIOS_HPP