#ifndef MOTORISTA_HPP
#define MOTORISTA_HPP

#include "Usuario.hpp"
#include "Veiculo.hpp"
#include "Rotina.hpp"
#include <vector>

namespace ufmg_carona {
    class Motorista : public Usuario {
    private:
        std::vector<Veiculo*> _veiculos;
        std::string _cnh_numero;
        std::vector<Rotina> _rotinas;

    public:
        Motorista(std::string nome, std::string cpf, std::string telefone, std::string data_nascimento,
                  std::string email, std::string senha, Genero genero, std::string vinculo_tipo,
                  std::string detalhe_vinculo, std::string cnh_numero);

        ~Motorista() override;

        void imprimir_perfil() const override;

        void adicionar_veiculo(Veiculo* veiculo);
        bool is_motorista() const override;
        
        const std::string& get_cnh_numero() const;

        const std::vector<Veiculo*>& get_veiculos() const;

        Veiculo* buscar_veiculo_por_placa(const std::string& placa) const;
        Veiculo* buscar_veiculo_por_indice(size_t indice) const;


        bool remover_veiculo(const std::string& placa);

        void adicionar_rotina(const Rotina& rotina);
        const std::vector<Rotina>& get_rotinas() const;
        bool remover_rotina(int indice);
    };
}

#endif // MOTORISTA_HPP