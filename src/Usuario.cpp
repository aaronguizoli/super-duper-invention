#include "Usuario.hpp"
#include "Avaliacao.hpp"
#include <iostream>
#include <numeric>
#include <iomanip>

namespace ufmg_carona {
    // Construtor ATUALIZADO: 'deseja_oferecer_caronas' removido.
    Usuario::Usuario(std::string n, std::string c, std::string tel, std::string dt_nasc, std::string e, std::string s, Genero g, std::string vinculo_tipo, std::string detalhe_vinculo)
        : _nome(n), _cpf(c), _email(e), _senha(s), _telefone(tel), _data_nascimento(dt_nasc), _genero(g),
          _avaliacoes_recebidas(), _notificacoes(),
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
    
    // Implementacao padrao para is_motorista() - um Usuario comum nao eh motorista
    bool Usuario::is_motorista() const { return false; }

    // Implementacao dos getters de vinculo
    std::string Usuario::get_vinculo() const {
        if (_vinculo_tipo == "aluno") {
            return "Aluno do curso de " + _detalhe_vinculo;
        } else if (_vinculo_tipo == "funcionario") {
            // Se for funcionario e o detalhe for "0", exibe apenas "Funcionario"
            if (_detalhe_vinculo == "0") {
                return "Funcionario";
            }
            return "Funcionario do setor " + _detalhe_vinculo; // Caso o 0 nao seja usado para funcionario
        }
        return "Vinculo desconhecido";
    }
    std::string Usuario::get_vinculo_raw() const { return _vinculo_tipo; }
    std::string Usuario::get_detalhe_vinculo() const { return _detalhe_vinculo; } // Retorna o detalhe literal

    // Implementacao dos outros getters
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
        // Logica para exibir detalhe de vinculo apenas se for Aluno e o detalhe nao for "0"
        if (_vinculo_tipo == "aluno" && _detalhe_vinculo != "0") {
            std::cout << "Vinculo: " << get_vinculo() << std::endl;
        } else if (_vinculo_tipo == "funcionario") { // Para funcionarios, exibe "Funcionario" sem detalhe se for "0"
             // Se o detalhe para funcionario eh "0", nao exibe o detalhe
             if (_detalhe_vinculo == "0") {
                 std::cout << "Vinculo: Funcionario" << std::endl;
             } else {
                 std::cout << "Vinculo: Funcionario do setor " << _detalhe_vinculo << std::endl;
             }
        }
        else { // Para outros casos (incluindo aluno com detalhe "0" ou desconhecido)
            std::cout << "Vinculo: " << get_vinculo_raw() << std::endl;
        }

        std::cout << "Avaliacao Media: " << std::fixed << std::setprecision(1) << get_media_avaliacoes() << " estrelas" << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }

    const std::string& Usuario::get_telefone() const { return _telefone; }
    const std::string& Usuario::get_data_nascimento() const { return _data_nascimento; }
}