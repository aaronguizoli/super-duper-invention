#include "Utilitarios.hpp"
#include "Zona.hpp" // Para os enums Zona e UFMGPosicao

#include <iostream>
#include <sstream>
#include <chrono>   // Para std::chrono::system_clock
#include <iomanip>  // Para std::put_time

namespace ufmg_carona {

    Utilitarios::Utilitarios() {
        _zona_para_string = {
            {Zona::PAMPULHA, "Pampulha"},
            {Zona::CENTRO_SUL, "Centro-Sul"},
            {Zona::NOROESTE, "Noroeste"},
            {Zona::LESTE, "Leste"},
            {Zona::OESTE, "Oeste"},
            {Zona::NORTE, "Norte"}, // Norte não estava no mapeamento original, adicionei
            {Zona::VENDA_NOVA, "Venda Nova"},
            {Zona::BARREIRO, "Barreiro"}
        };
        _int_para_zona = {
            {1, Zona::PAMPULHA}, {2, Zona::CENTRO_SUL}, {3, Zona::NOROESTE},
            {4, Zona::LESTE}, {5, Zona::OESTE}, {6, Zona::NORTE},
            {7, Zona::VENDA_NOVA}, {8, Zona::BARREIRO}
        };

        _ufmg_posicao_para_string = {
            {UFMGPosicao::ORIGEM, "Origem"},
            {UFMGPosicao::DESTINO, "Destino"}
        };
        _int_para_ufmg_posicao = {
            {1, UFMGPosicao::ORIGEM},
            {2, UFMGPosicao::DESTINO}
        };
    }

    std::tm Utilitarios::parseDatetimeString(const std::string& dt_str) const {
        std::tm tm_struct = {};
        std::stringstream ss(dt_str);
        ss >> std::get_time(&tm_struct, "%d/%m/%Y %H:%M");
        if (ss.fail()) {
            std::cerr << "ERRO ao parsear data/hora: " << dt_str << std::endl;
        }
        return tm_struct;
    }

    std::string Utilitarios::getCurrentDatetimeString() const {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* local_tm = std::localtime(&now_c);
        std::stringstream ss;
        ss << std::put_time(local_tm, "%d/%m/%Y %H:%M");
        return ss.str();
    }

    bool Utilitarios::isDatetimeInPast(const std::string& dt_str) const {
        std::tm carona_tm = parseDatetimeString(dt_str);
        std::time_t carona_time = std::mktime(&carona_tm);

        if (carona_time == -1) {
             std::cerr << "AVISO: Data/hora invalida para comparacao: " << dt_str << std::endl;
             return true;
        }

        auto now = std::chrono::system_clock::now();
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);

        return std::difftime(current_time, carona_time) > 0;
    }

    std::string Utilitarios::zonaToString(Zona z) const {
        auto it = _zona_para_string.find(z);
        if (it != _zona_para_string.end()) {
            return it->second;
        }
        return "Zona Desconhecida";
    }

    Zona Utilitarios::stringToZona(const std::string& s) const {
        for (const auto& pair : _zona_para_string) {
            if (pair.second == s) {
                return pair.first;
            }
        }
        return Zona::PAMPULHA; // Retorno padrão
    }

    std::string Utilitarios::ufmgPosicaoToString(UFMGPosicao up) const {
        auto it = _ufmg_posicao_para_string.find(up);
        if (it != _ufmg_posicao_para_string.end()) {
            return it->second;
        }
        return "Posicao Desconhecida";
    }

    UFMGPosicao Utilitarios::stringToUfmgPosicao(const std::string& s) const {
        for (const auto& pair : _ufmg_posicao_para_string) {
            if (pair.second == s) {
                return pair.first;
            }
        }
        return UFMGPosicao::ORIGEM; // Retorno padrão
    }

    const std::map<int, Zona>& Utilitarios::getIntParaZona() const {
        return _int_para_zona;
    }

    const std::map<Zona, std::string>& Utilitarios::getZonaParaString() const {
        return _zona_para_string;
    }

    const std::map<int, UFMGPosicao>& Utilitarios::getIntParaUfmgPosicao() const {
        return _int_para_ufmg_posicao;
    }

    const std::map<UFMGPosicao, std::string>& Utilitarios::getUfmgPosicaoParaString() const {
        return _ufmg_posicao_para_string;
    }

} // namespace ufmg_carona