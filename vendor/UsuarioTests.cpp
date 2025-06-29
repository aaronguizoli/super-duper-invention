// Define que este arquivo irá gerar a função main() para os testes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "Usuario.hpp" 

// Um conjunto de testes para a classe Usuario
TEST_CASE("Testes da Classe Usuario") {
    // Cria um objeto Usuario para ser usado nos sub-casos de teste
    ufmg_carona::Usuario usuario(
        "Fulano de Tal",
        "12345678900",
        "999999999",
        "01/01/2000",
        "fulano@email.com",
        "senha123",
        ufmg_carona::Genero::MASCULINO,
        "aluno",
        "Ciencia da Computacao"
    );

    // Sub-caso de teste para o construtor e getters básicos
    SUBCASE("Construtor e Getters") {
        CHECK(usuario.get_nome() == "Fulano de Tal");
        CHECK(usuario.get_cpf() == "12345678900");
        CHECK(usuario.get_email() == "fulano@email.com");
    }

    // Sub-caso de teste para a verificação de senha
    SUBCASE("Verificação de Senha") {
        CHECK(usuario.verificar_senha("senha123") == true);
        CHECK_FALSE(usuario.verificar_senha("senha_errada"));
    }

    // Sub-caso de teste para o status de motorista
    SUBCASE("Status de Motorista") {
        CHECK_FALSE(usuario.is_motorista());
        // Aqui você poderia adicionar a lógica para tornar o usuário um motorista
        // e depois verificar com CHECK(usuario.is_motorista() == true);
    }

    // Sub-caso de teste para a média de avaliações
    SUBCASE("Média de Avaliações") {
        // Sem avaliações, a média deve ser 0
        // Usamos CHECK com doctest::Approx para comparar números de ponto flutuante
        CHECK(usuario.get_media_avaliacoes() == doctest::Approx(0.0));
    }
}
