#include "Usuario.hpp"
#include "Avaliacao.hpp"
#include "Veiculo.hpp"
#include <iostream>
#include <numeric>
#include <iomanip>

namespace ufmg_carona {
    // Construtor ATUALIZADO: 'vinculo_tipo' e 'detalhe_vinculo' adicionados
    Usuario::Usuario(std::string n, std::string c, std::string tel, std::string dt_nasc, std::string e, std::string s, Genero g, bool deseja_motorista, std::string vinculo_tipo, std::string detalhe_vinculo)
        : _nome(n), _cpf(c), _email(e), _senha(s), _telefone(tel), _data_nascimento(dt_nasc), _genero(g),
          _possui_veiculo(false), _deseja_oferecer_caronas(deseja_motorista),
          _vinculo_tipo(vinculo_tipo), _detalhe_vinculo(detalhe_vinculo) {}

    Usuario::~Usuario() {
        for (Avaliacao* aval : _avaliacoes_recebidas) {
            delete aval;
        }
        _avaliacoes_recebidas.clear();
    }

    const std::string& Usuario::get_cpf() const { return _cpf; }
    const std::string& Usuario::get_nome() const { return _nome; }
    bool Usuario::verificar_senha(const std::string& s) const { return _senha == s; }
    void Usuario::cadastrar_veiculo(const Veiculo& v) { _veiculo = v; _possui_veiculo = true; }
    bool Usuario::is_motorista() const { return _possui_veiculo; }
    const Veiculo& Usuario::get_veiculo() const { return _veiculo; }

    // Implementação dos getters que antes eram virtuais puros ou estavam nas subclasses
    std::string Usuario::get_vinculo() const {
        if (_vinculo_tipo == "aluno") {
            return "Aluno do curso de " + _detalhe_vinculo;
        } else if (_vinculo_tipo == "funcionario") {
            return "Funcionario do setor " + _detalhe_vinculo;
        }
        return "Vinculo desconhecido";
    }
    std::string Usuario::get_vinculo_raw() const { return _vinculo_tipo; }
    std::string Usuario::get_detalhe_vinculo() const { return _detalhe_vinculo; }

    // Implementação dos outros getters
    const std::string& Usuario::get_email() const { return _email; }
    const std::string& Usuario::get_senha() const { return _senha; }
    Genero Usuario::get_genero() const { return _genero; }

    double Usuario::get_media_avaliacoes() const {
        if (_avaliacoes_recebidas.empty()) return 0.0;
        double soma = std::accumulate(_avaliacoes_recebidas.begin(), _avaliacoes_recebidas.end(), 0.0,
            [](double acc, const Avaliacao* aval) { return acc + aval->get_nota(); });
        return soma / _avaliacoes_recebidas.size();
    }
    void Usuario::adicionar_avaliacao_recebida(Avaliacao* a) { _avaliacoes_recebidas.push_back(a); }

    void Usuario::imprimir_perfil() const {
        std::cout << "\n--- Perfil de " << get_nome() << " ---" << std::endl;
        std::cout << "CPF: " << get_cpf() << std::endl;
        std::cout << "Telefone: " << get_telefone() << std::endl;
        std::cout << "Data de Nascimento: " << get_data_nascimento() << std::endl;
        std::cout << "Vinculo: " << get_vinculo() << std::endl;
        std::cout << "Avaliacao Media: " << std::fixed << std::setprecision(1) << get_media_avaliacoes() << " estrelas" << std::endl;
        std::cout << "Deseja oferecer caronas: " << (get_deseja_oferecer_caronas() ? "Sim" : "Nao") << std::endl;
        if (is_motorista()) {
            std::cout << "Veiculo Cadastrado:" << std::endl;
            get_veiculo().exibir_info();
        } else { std::cout << "Veiculo Cadastrado: Nao" << std::endl; }
        std::cout << "---------------------------------" << std::endl;
    }

    const std::string& Usuario::get_telefone() const { return _telefone; }
    const std::string& Usuario::get_data_nascimento() const { return _data_nascimento; }
    bool Usuario::get_deseja_oferecer_caronas() const { return _deseja_oferecer_caronas; }
}