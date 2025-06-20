#ifndef MOTORISTA_HPP
#define MOTORISTA_HPP

#include "Usuario.hpp"
#include "Veiculo.hpp"
#include <vector>

namespace ufmg_carona {
    class Motorista : public Usuario {
    private:
        std::vector<Veiculo*> _veiculos; // Lista de veiculos do motorista
        std::string _cnh_numero; // Numero da CNH do motorista

    public:
        // Construtor do Motorista, que chama o construtor do Usuario base e inicializa seus proprios membros
        Motorista(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento,
                  std::string email, std::string senha, Genero genero, std::string vinculo_tipo,
                  std::string detalhe_vinculo, std::string cnh_numero);

        // Destrutor para liberar a memoria dos objetos Veiculo*
        ~Motorista() override;

        // Sobrescreve o metodo imprimir_perfil() para incluir dados do motorista e seus veiculos
        void imprimir_perfil() const override;

        // Metodos especificos de Motorista
        void adicionar_veiculo(Veiculo* veiculo); // Adiciona um veiculo a lista
        bool is_motorista() const override; // Sobrescreve para indicar que este eh um motorista e se possui veiculo
        
        // Getter para a CNH
        const std::string& get_cnh_numero() const;

        // Getter para a lista de veiculos (para iterar e escolher)
        const std::vector<Veiculo*>& get_veiculos() const;

        // Metodo para buscar um veiculo por placa (necessario para oferecer carona)
        Veiculo* buscar_veiculo_por_placa(const std::string& placa) const;
    };
}

#endif // MOTORISTA_HPP