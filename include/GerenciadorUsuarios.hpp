#ifndef GERENCIADOR_USUARIOS_HPP
#define GERENCIADOR_USUARIOS_HPP

#include <vector>
#include <string>
#include <tuple>

namespace ufmg_carona {
    class TerminalIO;
    class Usuario;
    class Motorista;
    enum class Genero;

    class GerenciadorUsuarios {
    private:
        TerminalIO* _terminal_io;
        std::vector<Usuario*> _usuarios;
        
        void salvarDados();
        void carregarDados();

    public:
        GerenciadorUsuarios(TerminalIO* terminal_io);
        ~GerenciadorUsuarios();

        void cadastrarUsuario(const std::string& cpf_digitado, const std::string& nome_ufmg, const std::string& data_nascimento_ufmg,
                              const std::string& vinculo_ufmg, const std::string& detalhe_ufmg);
        Usuario* loginUsuario();
        void logoutUsuario(Usuario*& usuario_logado);
        void editarPerfil(Usuario* usuario);
        void tornarMotorista(Usuario*& usuario);
        Usuario* buscarUsuarioPorCpf(const std::string& cpf) const;
        const std::vector<Usuario*>& getUsuarios() const;
        void enviarNotificacao(Usuario* usuario, const std::string& mensagem);

        Usuario* fluxo_login();
        void fluxo_editar_perfil(Usuario* usuario_logado);
        void fluxo_tornar_motorista(Usuario*& usuario_logado);
        
        void carregarDadosUsuariosPublico();
        void salvarDadosUsuariosPublico();
    };
}
#endif