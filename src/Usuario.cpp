#include "Usuario.hpp"
#include "Avaliacao.hpp" // Certifique-se de que Avaliacao.hpp está incluído para o tipo Avaliacao*
#include "Veiculo.hpp"
#include <iostream>
#include <numeric> // Para std::accumulate
#include <iomanip>
// #include <memory> // Removido

namespace ufmg_carona {
    // Construtor atualizado (assinatura já foi ajustada no .hpp)
    Usuario::Usuario(std::string n, std::string c, std::string tel, std::string dt_nasc, std::string end, std::string e, std::string s, Genero g, bool deseja_motorista)
        : _nome(n), _cpf(c), _email(e), _senha(s), _telefone(tel), _data_nascimento(dt_nasc), _endereco(end), _genero(g), _possui_veiculo(false), _deseja_oferecer_caronas(deseja_motorista) {}

    // NOVO: Implementação do destrutor para liberar as Avaliações
    Usuario::~Usuario() {
        for (Avaliacao* aval : _avaliacoes_recebidas) {
            delete aval; // Libera a memória de cada Avaliacao
        }
        _avaliacoes_recebidas.clear(); // Limpa o vetor
        // std::cout << "DEBUG: Usuario " << _nome << " destruído e avaliações liberadas." << std::endl; // Para debug, pode ser removido
    }

    const std::string& Usuario::get_cpf() const { return _cpf; }
    const std::string& Usuario::get_nome() const { return _nome; }
    bool Usuario::verificar_senha(const std::string& s) const { return _senha == s; }
    void Usuario::cadastrar_veiculo(const Veiculo& v) { _veiculo = v; _possui_veiculo = true; }
    bool Usuario::is_motorista() const { return _possui_veiculo; }
    const Veiculo& Usuario::get_veiculo() const { return _veiculo; }

    double Usuario::get_media_avaliacoes() const {
        if (_avaliacoes_recebidas.empty()) return 0.0;
        // ALTERAÇÃO CRÍTICA: Lambda ajustada para C++11.
        // O parâmetro 'aval' agora é explicitamente 'const Avaliacao*', e não 'const auto&'.
        double soma = std::accumulate(_avaliacoes_recebidas.begin(), _avaliacoes_recebidas.end(), 0.0,
            [](double acc, const Avaliacao* aval) { return acc + aval->get_nota(); });
        return soma / _avaliacoes_recebidas.size();
    }
    // ALTERAÇÃO: De std::shared_ptr<Avaliacao> para Avaliacao*
    void Usuario::adicionar_avaliacao_recebida(Avaliacao* a) { _avaliacoes_recebidas.push_back(a); }

    void Usuario::imprimir_perfil() const {
        std::cout << "\n--- Perfil de " << get_nome() << " ---" << std::endl;
        std::cout << "CPF: " << get_cpf() << std::endl;
        std::cout << "Telefone: " << get_telefone() << std::endl;
        std::cout << "Data de Nascimento: " << get_data_nascimento() << std::endl;
        std::cout << "Endereco: " << get_endereco() << std::endl;
        std::cout << "Vinculo: " << get_vinculo() << std::endl;
        std::cout << "Avaliacao Media: " << std::fixed << std::setprecision(1) << get_media_avaliacoes() << " estrelas" << std::endl;
        std::cout << "Deseja oferecer caronas: " << (get_deseja_oferecer_caronas() ? "Sim" : "Nao") << std::endl;
        if (is_motorista()) {
            std::cout << "Veiculo Cadastrado:" << std::endl;
            get_veiculo().exibir_info();
        } else { std::cout << "Veiculo Cadastrado: Nao" << std::endl; }
        std::cout << "---------------------------------" << std::endl;
    }

    // Getters (sem alterações na implementação)
    const std::string& Usuario::get_telefone() const { return _telefone; }
    const std::string& Usuario::get_data_nascimento() const { return _data_nascimento; }
    const std::string& Usuario::get_endereco() const { return _endereco; }
    bool Usuario::get_deseja_oferecer_caronas() const { return _deseja_oferecer_caronas; }
}