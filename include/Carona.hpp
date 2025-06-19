#ifndef CARONA_HPP
#define CARONA_HPP
#include <string>
#include <vector>
#include <memory>

namespace ufmg_carona {
    class Usuario;
    enum class TipoCarona { AGENDADA, IMEDIATA };
    enum class StatusCarona { AGUARDANDO, LOTADA, EM_VIAGEM, FINALIZADA, CANCELADA };

    class Carona {
    private:
        int _id;
        static int _proximo_id;
        std::string _origem, _destino, _data_hora_partida;
        std::shared_ptr<Usuario> _motorista;
        std::vector<std::shared_ptr<Usuario>> _passageiros;
        int _vagas_disponiveis;
        bool _apenas_mulheres;
        StatusCarona _status;
        TipoCarona _tipo;
    public:
        Carona(std::string origem, std::string destino, std::string data, std::shared_ptr<Usuario> motorista, bool apenas_mulheres, TipoCarona tipo);
        static int gerar_proximo_id();
        int get_id() const;
        void exibir_info() const;
    };
}
#endif