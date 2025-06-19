#include "Usuario.hpp"
#include "Avaliacao.hpp"
#include "Veiculo.hpp"
#include <iostream>
#include <numeric>
#include <iomanip>

namespace ufmg_carona {
    Usuario::Usuario(std::string nome, std::string cpf, std::string email, std::string senha, Genero genero)
        : _nome(nome), _cpf(cpf), _email(email), _senha(senha), _genero(genero), _possui_veiculo(false) {}

    const std::string& Usuario::get_cpf() const { return _cpf; }
    const std::string& Usuario::get_nome() const { return _nome; }
    bool Usuario::verificar_senha(const std::string& senha) const { return _senha == senha; }
    void Usuario::cadastrar_veiculo(const Veiculo& veiculo) { _veiculo = veiculo; _possui_veiculo = true; }
    bool Usuario::is_motorista() const { return _possui_veiculo; }
    const Veiculo& Usuario::get_veiculo() const { return _veiculo; }

    double Usuario::get_media_avaliacoes() const {
        if (_avaliacoes_recebidas.empty()) return 0.0;
        double soma = std::accumulate(_avaliacoes_recebidas.begin(), _avaliacoes_recebidas.end(), 0.0,
            [](double acc, const auto& aval) { return acc + aval->get_nota(); });
        return soma / _avaliacoes_recebidas.size();
    }

    void Usuario::adicionar_avaliacao_recebida(std::shared_ptr<Avaliacao> avaliacao) {
        _avaliacoes_recebidas.push_back(avaliacao);
    }

    // IMPLEMENTAÇÃO DO MÉTODO ADICIONADA AQUI
    void Usuario::imprimir_perfil() const {
        std::cout << "\n--- Perfil de " << get_nome() << " ---" << std::endl;
        std::cout << "CPF: " << get_cpf() << std::endl;
        std::cout << "Vinculo: " << get_vinculo() << std::endl;
        std::cout << "Avaliacao Media: " << std::fixed << std::setprecision(1) << get_media_avaliacoes() << " estrelas" << std::endl;
        if (is_motorista()) {
            std::cout << "Veiculo Cadastrado:" << std::endl;
            get_veiculo().exibir_info();
        } else {
            std::cout << "Veiculo Cadastrado: Nao" << std::endl;
        }
        std::cout << "---------------------------------" << std::endl;
    }
}