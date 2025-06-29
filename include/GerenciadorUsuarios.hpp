#ifndef GERENCIADOR_USUARIOS_HPP
#define GERENCIADOR_USUARIOS_HPP

#include <vector>
#include <string>
#include <tuple> // Para o retorno de buscarDadosUfmgPorCpf

namespace ufmg_carona {
    // Forward declarations
    class TerminalIO;
    class Usuario;
    class Motorista; // Para downcast de Usuario* para Motorista*
    enum class Genero; // Para o método editarPerfil

    class GerenciadorUsuarios {
    private:
        TerminalIO* _terminal_io;
        std::vector<Usuario*> _usuarios; // Coleção de todos os usuários
        
        void salvarDados(); // Interno
        void carregarDados(); // Interno

    public:
        GerenciadorUsuarios(TerminalIO* terminal_io);
        ~GerenciadorUsuarios();

        // <--- NOVA ASSINATURA: Recebe dados UFMG já processados
        void cadastrarUsuario(const std::string& cpf_digitado, const std::string& nome_ufmg, const std::string& data_nascimento_ufmg,
                              const std::string& vinculo_ufmg, const std::string& detalhe_ufmg);
        Usuario* loginUsuario(); // Retorna o usuário logado se sucesso
        void logoutUsuario(Usuario*& usuario_logado); // Recebe por referência para limpar o ponteiro
        void editarPerfil(Usuario* usuario);
        void tornarMotorista(Usuario*& usuario); // Recebe por referência para atualizar o ponteiro em Sistema
        Usuario* buscarUsuarioPorCpf(const std::string& cpf) const;
        const std::vector<Usuario*>& getUsuarios() const; // Retorna a coleção de usuários
        void enviarNotificacao(Usuario* usuario, const std::string& mensagem); // Remover o bool para_motorista, isso é responsabilidade de quem chama

        // Métodos auxiliares para o Sistema
        // void fluxo_cadastro(std::tuple<bool, std::string, std::string, std::string, std::string> (*buscar_dados_ufmg_func)(const std::string&)); // <--- REMOVIDO
        Usuario* fluxo_login();
        void fluxo_editar_perfil(Usuario* usuario_logado);
        void fluxo_tornar_motorista(Usuario*& usuario_logado); // Ref para atualizar o _usuario_logado no Sistema
        
        // Funções para carregar/salvar dados em conjunto com o Sistema
        void carregarDadosUsuariosPublico(); // Método público para carregar
        void salvarDadosUsuariosPublico(); // Método público para salvar
    };
} // namespace ufmg_carona

#endif // GERENCIADOR_USUARIOS_HPP