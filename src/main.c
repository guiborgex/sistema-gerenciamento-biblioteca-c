/* ========= INCLUDES E DEFINES ========= */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LIVROS 100
#define MAX_USUARIOS 100
#define MAX_EMPRESTIMOS 100
#define DEVOLVIDO 0
#define EMPRESTADO 1


/* ========= STRUCTS ========= */

typedef struct { //DATA
    int dia;
    int mes;
    int ano;
} Data;

typedef struct { //LIVRO
    int codigo;
    char titulo[101];
    char autor[81];
    char editora[61];
    int ano_publicacao;
    int exemplares_total;
    int exemplares_disponiveis;
} Livro;

typedef struct { //USUARIO
    int matricula;
    char nome[101];
    char curso[51];
    char telefone[16];
    Data data_cadastro;
} Usuario;

typedef struct { //EMPRESTIMO/DEVOLUCAO
    int codigo;
    int matricula_usuario;
    int codigo_livro;
    Data data_emprestimo;
    Data data_prevista_devolucao;
    int status;
} Emprestimo;

/* ========= VARIAVEIS GLOBAIS ========= */

Livro livros[MAX_LIVROS];
Usuario usuarios[MAX_USUARIOS];
Emprestimo emprestimos[MAX_EMPRESTIMOS];

int qtd_livros = 0;
int qtd_usuarios = 0;
int qtd_emprestimos = 0;

/* ========= PROTOTIPOS DAS FUNCOES ========= */

void pesquisar_livro(void);
void imprimir_livro_completo(Livro *L);
void imprimir_usuario_completo(Usuario *U);
void cadastrar_livro(void);
void listar_livros(void);
void pesquisar_usuario(void);
void cadastrar_usuario(void);
void listar_usuarios(void);
void registrar_emprestimo(void);
void listar_emprestimos(void);
void listar_emprestimos_ativos(void);
void devolver_livro(void);
void relatorio_livro_mais_emprestado(void);
void relatorio_usuarios_em_atraso(void);
void renovar_emprestimo(void);
void busca_avancada_livros(void);

/* ========= FUNCOES UTILITARIAS ========= */

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*  LE INTEIRO COM VALIDACAO  */
int ler_inteiro(const char *msg, int min, int max) {
    int valor;
    char entrada[100];

    while (1) {
        printf("%s", msg);
        fgets(entrada, sizeof(entrada), stdin);

        if (sscanf(entrada, "%d", &valor) != 1) {
            printf("Entrada invalida. Digite um numero inteiro.\n");
            continue;
        }

        if (valor < min || valor > max) {
            printf("Valor fora do intervalo [%d a %d].\n", min, max);
            continue;
        }

        return valor;
    }
}

/*  LE STRING DE FORMA SEGURA  */
void ler_string(const char *msg, char *dest, int tamanho) {
    printf("%s", msg);
    fgets(dest, tamanho, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}

/*  VALIDA DATA SIMPLES  */
int validar_data(int d, int m, int a) {
    if (a < 1900 || a > 2100) return 0;
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > 31) return 0;
    return 1;
}

/*  LE DATA COM VALIDACAO  */
void ler_data(const char *msg, Data *data) {
    while (1) {
        printf("%s (dd mm aaaa): ", msg);

        if (scanf("%d %d %d",
            &data->dia,
            &data->mes,
            &data->ano) == 3 &&
            validar_data(data->dia, data->mes, data->ano)) {

            limpar_buffer();
            return;
        }

        printf("Data invalida!\n");
        limpar_buffer();
    }
}

/*  DATAS  */
int data_menor(Data d1, Data d2) {
    if (d1.ano != d2.ano) return d1.ano < d2.ano;
    if (d1.mes != d2.mes) return d1.mes < d2.mes;
    return d1.dia < d2.dia;
}

int dias_no_mes(int mes, int ano) {
    switch (mes) {
        case 1: case 3: case 5: case 7:
        case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            if ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0))
                return 29;
            return 28;
        default:
            return 30;
    }
}

void adicionar_dias(Data *data, int dias) {
    data->dia += dias;

    while (data->dia > dias_no_mes(data->mes, data->ano)) {
        data->dia -= dias_no_mes(data->mes, data->ano);
        data->mes++;

        if (data->mes > 12) {
            data->mes = 1;
            data->ano++;
        }
    }
}

/* ========= LIVROS ========= */

int buscar_livro(int codigo) {
    for (int i = 0; i < qtd_livros; i++) {
        if (livros[i].codigo == codigo)
            return i;
    }
    return -1;
}

void cadastrar_livro() {
    if (qtd_livros == MAX_LIVROS) {
        printf("Limite de livros atingido.\n");
        return;
    }

    Livro novo;

    novo.codigo = ler_inteiro("Digite o codigo do livro: ", 1, 999999);

    if (buscar_livro(novo.codigo) != -1) {
        printf("ERRO: Codigo ja cadastrado.\n");
        return;
    }

    ler_string("Titulo: ", novo.titulo, sizeof(novo.titulo));
    ler_string("Autor: ", novo.autor, sizeof(novo.autor));
    ler_string("Editora: ", novo.editora, sizeof(novo.editora));

    novo.ano_publicacao = ler_inteiro("Ano de publicacao: ", 1900, 2100);
    novo.exemplares_total = ler_inteiro("Quantidade de exemplares: ", 1, 1000);
    novo.exemplares_disponiveis = novo.exemplares_total;

    livros[qtd_livros++] = novo;

    printf("Livro cadastrado com sucesso!\n");
}

void listar_livros() {
    if (qtd_livros == 0) {
        printf("Nenhum livro cadastrado.\n");
        return;
    }

    for (int i = 0; i < qtd_livros; i++) {
        printf("\nLivro %d\n", i + 1);
        printf("Codigo: %d\n", livros[i].codigo);
        printf("Titulo: %s\n", livros[i].titulo);
        printf("Autor: %s\n", livros[i].autor);
        printf("Editora: %s\n", livros[i].editora);
        printf("Ano: %d\n", livros[i].ano_publicacao);
        printf("Disponiveis: %d\n", livros[i].exemplares_disponiveis);
    }
}

void pesquisar_livro_por_codigo(void) {
    if (qtd_livros == 0) {
        printf("Nenhum livro cadastrado.\n");
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    int codigo = ler_inteiro("Digite o codigo do livro a pesquisar: ", 1, 999999);

    int idx = buscar_livro(codigo);
    if (idx == -1) {
        printf("Livro com codigo %d nao encontrado.\n", codigo);
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    Livro *L = &livros[idx];

    printf("\n--- Dados do Livro ---\n");
    printf("Codigo: %d\n", L->codigo);
    printf("Titulo: %s\n", L->titulo);
    printf("Autor: %s\n", L->autor);
    printf("Editora: %s\n", L->editora);
    printf("Ano: %d\n", L->ano_publicacao);
    printf("Disponiveis: %d\n", L->exemplares_disponiveis);

    printf("\nPressione ENTER para voltar...");
    getchar();
}

void relatorio_livro_mais_emprestado() {

    if (qtd_livros == 0 || qtd_emprestimos == 0) {
        printf("Nao ha dados suficientes para gerar o relatorio.\n");
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    int maior = 0;
    int indice_livro = -1;

    for (int i = 0; i < qtd_livros; i++) {

        int contador = 0;

        for (int j = 0; j < qtd_emprestimos; j++) {
            if (emprestimos[j].codigo_livro == livros[i].codigo) {
                contador++;
            }
        }

        if (contador > maior) {
            maior = contador;
            indice_livro = i;
        }
    }

    if (maior == 0) {
        printf("Nenhum livro foi emprestado ate o momento.\n");
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    printf("\n=== LIVRO MAIS EMPRESTADO ===\n");
    imprimir_livro_completo(&livros[indice_livro]);
    printf("Total de emprestimos: %d\n", maior);

    printf("\nPressione ENTER para voltar...");
    getchar();
}

void imprimir_livro_completo(Livro *L) {

    printf("\n--- DADOS DO LIVRO ---\n");
    printf("Codigo: %d\n", L->codigo);
    printf("Titulo: %s\n", L->titulo);
    printf("Autor: %s\n", L->autor);
    printf("Editora: %s\n", L->editora);
    printf("Ano de publicacao: %d\n", L->ano_publicacao);
    printf("Exemplares totais: %d\n", L->exemplares_total);
    printf("Exemplares disponiveis: %d\n", L->exemplares_disponiveis);
    printf("----------------------\n");
}

void busca_avancada_livros() {

    char titulo[101];
    char autor[81];
    int ano_min, ano_max;
    int apenas_disponiveis;
    int encontrou = 0;

    if (qtd_livros == 0) {
        printf("Nenhum livro cadastrado.\n");
        getchar();
        return;
    }

    printf("\n=== BUSCA AVANCADA DE LIVROS ===\n");
    printf("Deixe em branco ou 0 para ignorar o criterio.\n\n");

    /* Titulo */
    ler_string("Parte do titulo: ", titulo, sizeof(titulo));

    /* Autor */
    ler_string("Parte do autor: ", autor, sizeof(autor));

    /* Ano mínimo */
    ano_min = ler_inteiro("Ano minimo (0 para ignorar): ", 0, 2100);

    /* Ano máximo */
    ano_max = ler_inteiro("Ano maximo (0 para ignorar): ", 0, 2100);

    /* Disponibilidade */
    apenas_disponiveis = ler_inteiro(
        "Apenas livros disponiveis? (1-SIM / 0-NAO): ",
        0, 1
    );

    printf("\n=== RESULTADO DA BUSCA ===\n");

    for (int i = 0; i < qtd_livros; i++) {

        /* Filtros */
        if (strlen(titulo) > 0 &&
            strstr(livros[i].titulo, titulo) == NULL)
            continue;

        if (strlen(autor) > 0 &&
            strstr(livros[i].autor, autor) == NULL)
            continue;

        if (ano_min != 0 &&
            livros[i].ano_publicacao < ano_min)
            continue;

        if (ano_max != 0 &&
            livros[i].ano_publicacao > ano_max)
            continue;

        if (apenas_disponiveis == 1 &&
            livros[i].exemplares_disponiveis == 0)
            continue;

        imprimir_livro_completo(&livros[i]);
        encontrou = 1;
    }

    if (!encontrou) {
        printf("\nNenhum livro encontrado com os criterios informados.\n");
    }

    printf("\nPressione ENTER para voltar...");
    getchar();
}

/* ========= USUARIOS ========= */

int buscar_usuario(int matricula) {
    for (int i = 0; i < qtd_usuarios; i++) {
        if (usuarios[i].matricula == matricula)
            return i;
    }
    return -1;
}

void cadastrar_usuario() {
    if (qtd_usuarios == MAX_USUARIOS) {
        printf("Limite de usuarios atingido.\n");
        return;
    }

    Usuario novo;

    novo.matricula = ler_inteiro("Matricula: ", 1, 999999);

    if (buscar_usuario(novo.matricula) != -1) {
        printf("ERRO: Matricula ja cadastrada.\n");
        return;
    }

    ler_string("Nome: ", novo.nome, sizeof(novo.nome));
    ler_string("Curso: ", novo.curso, sizeof(novo.curso));
    ler_string("Telefone: ", novo.telefone, sizeof(novo.telefone));

    ler_data("Data de cadastro", &novo.data_cadastro);

    usuarios[qtd_usuarios++] = novo;

    printf("Usuario cadastrado com sucesso!\n");
}

void listar_usuarios() {
    if (qtd_usuarios == 0) {
        printf("Nenhum usuario cadastrado.\n");
        return;
    }

    for (int i = 0; i < qtd_usuarios; i++) {
        printf("\nUsuario %d\n", i + 1);
        printf("Matricula: %d\n", usuarios[i].matricula);
        printf("Nome: %s\n", usuarios[i].nome);
        printf("Curso: %s\n", usuarios[i].curso);
        printf("Telefone: %s\n", usuarios[i].telefone);
        printf("Data cadastro: %02d/%02d/%d\n",
               usuarios[i].data_cadastro.dia,
               usuarios[i].data_cadastro.mes,
               usuarios[i].data_cadastro.ano);
    }
}

void pesquisar_usuario() {

    if (qtd_usuarios == 0) {
        printf("Nenhum usuario cadastrado.\n");
        getchar();
        return;
    }

    int opcao;
    int encontrou = 0;
    int matricula;
    char texto[101];

    printf("\n=== PESQUISAR USUARIO ===\n");
    printf("1 - Por matricula\n");
    printf("2 - Por nome\n");

    opcao = ler_inteiro("Opcao: ", 1, 2);

    if (opcao == 1) {

        matricula = ler_inteiro("Digite a matricula: ", 1, 999999);

        for (int i = 0; i < qtd_usuarios; i++) {
            if (usuarios[i].matricula == matricula) {
                imprimir_usuario_completo(&usuarios[i]);
                encontrou = 1;
                break;
            }
        }

    } else {

        ler_string("Digite parte do nome: ", texto, sizeof(texto));

        for (int i = 0; i < qtd_usuarios; i++) {
            if (strstr(usuarios[i].nome, texto) != NULL) {
                imprimir_usuario_completo(&usuarios[i]);
                encontrou = 1;
            }
        }
    }

    if (!encontrou) {
        printf("\nNenhum usuario encontrado.\n");
    }

    printf("\nPressione ENTER para voltar...");
    getchar();
}

void pesquisar_usuario_por_matricula(void) {

    if (qtd_usuarios == 0) {
        printf("Nenhum usuario cadastrado.\n");
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    int mat = ler_inteiro("Digite a matricula do usuario a pesquisar: ", 1, 999999);

    int idx = buscar_usuario(mat);
    if (idx == -1) {
        printf("Usuario com matricula %d nao encontrado.\n", mat);
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    Usuario *U = &usuarios[idx];

    printf("\n--- Dados do Usuario ---\n");
    printf("Matricula: %d\n", U->matricula);
    printf("Nome: %s\n", U->nome);
    printf("Curso: %s\n", U->curso);
    printf("Telefone: %s\n", U->telefone);
    printf("Data de cadastro: %02d/%02d/%04d\n",
           U->data_cadastro.dia,
           U->data_cadastro.mes,
           U->data_cadastro.ano);
    printf("------------------------\n");

    printf("\nPressione ENTER para voltar...");
    getchar();
}

void imprimir_usuario_completo(Usuario *U) {

    printf("\n--- DADOS DO USUARIO ---\n");
    printf("Matricula: %d\n", U->matricula);
    printf("Nome: %s\n", U->nome);
    printf("Curso: %s\n", U->curso);
    printf("Telefone: %s\n", U->telefone);
    printf("Cadastro: %02d/%02d/%d\n",
           U->data_cadastro.dia,
           U->data_cadastro.mes,
           U->data_cadastro.ano);
}

/* ========= EMPRESTIMOS/DEVOLUCOES ========= */

void registrar_emprestimo() {

    if (qtd_emprestimos == MAX_EMPRESTIMOS) {
        printf("Limite de emprestimos atingido.\n");
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    Emprestimo novo;

    novo.codigo = ler_inteiro(
        "Codigo do emprestimo: ",
        1, 999999
    );

    novo.matricula_usuario = ler_inteiro(
        "Matricula do usuario: ",
        1, 999999
    );

    int idx_usuario = buscar_usuario(novo.matricula_usuario);
    if (idx_usuario == -1) {
        printf("Usuario nao encontrado.\n");
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    novo.codigo_livro = ler_inteiro(
        "Codigo do livro: ",
        1, 999999
    );

    int idx_livro = buscar_livro(novo.codigo_livro);
    if (idx_livro == -1) {
        printf("Livro nao encontrado.\n");
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    if (livros[idx_livro].exemplares_disponiveis == 0) {
        printf("Nenhum exemplar disponivel deste livro.\n");
        printf("Pressione ENTER para voltar...");
        getchar();
        return;
    }

    ler_data("Data do emprestimo", &novo.data_emprestimo);

    novo.data_prevista_devolucao = novo.data_emprestimo;
    adicionar_dias(&novo.data_prevista_devolucao, 7);

    novo.status = EMPRESTADO;

    livros[idx_livro].exemplares_disponiveis--;

    emprestimos[qtd_emprestimos++] = novo;

    printf("\nEmprestimo realizado com sucesso!\n");
    printf("Devolucao prevista para: %02d/%02d/%d\n",
       novo.data_prevista_devolucao.dia,
       novo.data_prevista_devolucao.mes,
       novo.data_prevista_devolucao.ano);

    printf("Pressione ENTER para continuar...");
    getchar();
}

void listar_emprestimos() {

    if (qtd_emprestimos == 0) {
        printf("Nenhum emprestimo registrado.\n");
        getchar();
    }

    for (int i = 0; i < qtd_emprestimos; i++) {
        printf("\nEmprestimo %d\n", i + 1);
        printf("Codigo: %d\n", emprestimos[i].codigo);
        printf("Usuario (matricula): %d\n", emprestimos[i].matricula_usuario);
        printf("Livro (codigo): %d\n", emprestimos[i].codigo_livro);
        printf("Data emprestimo: %02d/%02d/%d\n",
               emprestimos[i].data_emprestimo.dia,
               emprestimos[i].data_emprestimo.mes,
               emprestimos[i].data_emprestimo.ano);
        printf("Devolucao prevista: %02d/%02d/%d\n",
               emprestimos[i].data_prevista_devolucao.dia,
               emprestimos[i].data_prevista_devolucao.mes,
               emprestimos[i].data_prevista_devolucao.ano);
        printf("Status: %s\n",
               emprestimos[i].status == EMPRESTADO ? "Emprestado" : "Devolvido");
    }

    getchar();
}

void listar_emprestimos_ativos() {

    int encontrou = 0;

    if (qtd_emprestimos == 0) {
        printf("Nenhum emprestimo registrado.\n");
        getchar();
        return;
    }

    printf("\n=== EMPRESTIMOS ATIVOS ===\n");

    for (int i = 0; i < qtd_emprestimos; i++) {

        if (emprestimos[i].status == EMPRESTADO) {

            printf("\nCodigo: %d\n", emprestimos[i].codigo);
            printf("Usuario (matricula): %d\n", emprestimos[i].matricula_usuario);
            printf("Livro (codigo): %d\n", emprestimos[i].codigo_livro);
            printf("Data emprestimo: %02d/%02d/%d\n",
                   emprestimos[i].data_emprestimo.dia,
                   emprestimos[i].data_emprestimo.mes,
                   emprestimos[i].data_emprestimo.ano);
            printf("Devolucao prevista: %02d/%02d/%d\n",
                   emprestimos[i].data_prevista_devolucao.dia,
                   emprestimos[i].data_prevista_devolucao.mes,
                   emprestimos[i].data_prevista_devolucao.ano);

            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\nNao ha emprestimos ativos no momento.\n");
    }

    getchar();
}

void devolver_livro() {

    if (qtd_emprestimos == 0) {
        printf("Nao ha emprestimos registrados.\n");
        getchar();
        return;
    }

    int codigo = ler_inteiro("Informe o codigo do emprestimo: ", 1, 999999);


    for (int i = 0; i < qtd_emprestimos; i++) {

        if (emprestimos[i].codigo == codigo &&
            emprestimos[i].status == EMPRESTADO) {

            int idx_livro = buscar_livro(emprestimos[i].codigo_livro);

            if (idx_livro != -1) {
                livros[idx_livro].exemplares_disponiveis++;
            }

            emprestimos[i].status = DEVOLVIDO;

            printf("Livro devolvido com sucesso!\n");
            getchar(); getchar();
            return;
        }
    }

    printf("Emprestimo nao encontrado ou ja devolvido.\n");
    getchar(); getchar();
}

void relatorio_usuarios_em_atraso() {

    if (qtd_emprestimos == 0) {
        printf("Nao ha emprestimos registrados.\n");
        getchar();
        return;
    }

    Data hoje;
    printf("Informe a data atual\n");
    ler_data("Data atual", &hoje);

    int encontrou = 0;

    printf("\n=== USUARIOS COM EMPRESTIMOS EM ATRASO ===\n");

    for (int i = 0; i < qtd_emprestimos; i++) {

        if (emprestimos[i].status == EMPRESTADO &&
            data_menor(emprestimos[i].data_prevista_devolucao, hoje)) {

            int idx_usuario = buscar_usuario(emprestimos[i].matricula_usuario);
            int idx_livro = buscar_livro(emprestimos[i].codigo_livro);

            if (idx_usuario == -1 || idx_livro == -1)
                continue;

            printf("\n--- EMPRESTIMO EM ATRASO ---\n");

            imprimir_usuario_completo(&usuarios[idx_usuario]);

            printf("\nLivro emprestado:\n");
            imprimir_livro_completo(&livros[idx_livro]);

            printf("Data do emprestimo: %02d/%02d/%d\n",
                   emprestimos[i].data_emprestimo.dia,
                   emprestimos[i].data_emprestimo.mes,
                   emprestimos[i].data_emprestimo.ano);

            printf("Devolucao prevista: %02d/%02d/%d\n",
                   emprestimos[i].data_prevista_devolucao.dia,
                   emprestimos[i].data_prevista_devolucao.mes,
                   emprestimos[i].data_prevista_devolucao.ano);

            printf("----------------------------\n");

            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\nNenhum emprestimo em atraso.\n");
    }

    printf("\nPressione ENTER para voltar...");
    getchar();
}

void renovar_emprestimo() {

    if (qtd_emprestimos == 0) {
        printf("Nao ha emprestimos registrados.\n");
        getchar();
        return;
    }

    Data hoje;
    printf("Informe a data atual\n");
    ler_data("Data atual", &hoje);

    int codigo = ler_inteiro(
        "Digite o codigo do emprestimo a renovar: ",
        1, 999999
    );

    for (int i = 0; i < qtd_emprestimos; i++) {

        if (emprestimos[i].codigo == codigo) {

            if (emprestimos[i].status != EMPRESTADO) {
                printf("Este emprestimo ja foi devolvido.\n");
                getchar();
                return;
            }

            /* Verifica atraso */
            if (data_menor(
                emprestimos[i].data_prevista_devolucao,
                hoje)) {

                printf("Emprestimo em atraso. Renovacao nao permitida.\n");
                getchar();
                return;
            }

            /* Nova data */
            ler_data(
                "Nova data prevista de devolucao",
                &emprestimos[i].data_prevista_devolucao
            );

            printf("\nEmprestimo renovado com sucesso!\n");
            getchar();
            return;
        }
    }

    printf("Emprestimo nao encontrado.\n");
    getchar();
}

/* ========= MENUS ========= */

void menu_usuarios() {
    int op;

    do {
        printf("\n=== CONTROLE DE USUARIOS ===\n");
        printf("1 - Cadastrar usuario\n");
        printf("2 - Listar usuarios\n");
        printf("3 - Pesquisar usuario\n");
        printf("0 - Voltar ao menu principal\n");
        op = ler_inteiro("Opcao: ", 0, 3);

        switch (op) {
            case 1:
                cadastrar_usuario();
                break;
            case 2:
                listar_usuarios();
                break;
            case 3:
                pesquisar_usuario();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida!\n");
        }

        if (op != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }

    } while (op != 0);
}

void menu_livros() {
    int op;

    do {
        printf("\n=== CONTROLE DE LIVROS ===\n");
        printf("1 - Cadastrar livro\n");
        printf("2 - Listar livros\n");
        printf("3 - Pesquisar livro (codigo/titulo/autor)\n");
        printf("4 - Busca avancada de livros\n");
        printf("0 - Voltar ao menu principal\n");
        op = ler_inteiro("Opcao: ", 0, 4);


        switch (op) {
            case 1:
                cadastrar_livro();
                break;
            case 2:
                listar_livros();
                break;
            case 3:
                pesquisar_livro();
                break;
            case 4:
                busca_avancada_livros();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida!\n");
        }

        if (op != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }

    } while (op != 0);
}

void menu_emprestimos() {
    int op;

    do {
        printf("\n=== EMPRESTIMOS / DEVOLUCOES ===\n");
        printf("1 - Registrar emprestimo\n");
        printf("2 - Listar TODOS os emprestimos\n");
        printf("3 - Listar emprestimos ATIVOS\n");
        printf("4 - Devolver livro\n");
        printf("5 - Livro mais emprestado\n");
        printf("6 - Usuarios com emprestimos em atraso\n");
        printf("7 - Renovar emprestimo\n");
        printf("0 - Voltar ao menu principal\n");
        op = ler_inteiro("Opcao: ", 0, 7);


        switch (op) {
            case 1:
                registrar_emprestimo();
                break;
            case 2:
                listar_emprestimos();
                break;
            case 3:
                listar_emprestimos_ativos();
                break;
            case 4:
                devolver_livro();
                break;
            case 5:
                relatorio_livro_mais_emprestado();
                break;
            case 6:
                relatorio_usuarios_em_atraso();
                break;
            case 7:
                renovar_emprestimo();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida!\n");
        }

        if (op != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }

    } while (op != 0);
}

void pesquisar_livro() {

    int opcao;

    if (qtd_livros == 0) {
        printf("Nenhum livro cadastrado.\n");
        getchar();
        return;
    }

    do {
        int encontrou = 0;
        int codigo;
        char texto[101];

        printf("\n=== PESQUISAR LIVRO ===\n");
        printf("1 - Por codigo\n");
        printf("2 - Por titulo\n");
        printf("3 - Por autor\n");
        printf("0 - Voltar\n");

        opcao = ler_inteiro("Opcao: ", 0, 3);

        if (opcao == 0) return;

        if (opcao == 1) {
            codigo = ler_inteiro("Digite o codigo do livro: ", 1, 999999);

            for (int i = 0; i < qtd_livros; i++) {
                if (livros[i].codigo == codigo) {
                    imprimir_livro_completo(&livros[i]);
                    encontrou = 1;
                    break;
                }
            }

        } else if (opcao == 2) {

            ler_string("Digite parte do titulo: ", texto, sizeof(texto));

            for (int i = 0; i < qtd_livros; i++) {
                if (strstr(livros[i].titulo, texto)) {
                    imprimir_livro_completo(&livros[i]);
                    encontrou = 1;
                }
            }

        } else if (opcao == 3) {

            ler_string("Digite parte do autor: ", texto, sizeof(texto));

            for (int i = 0; i < qtd_livros; i++) {
                if (strstr(livros[i].autor, texto)) {
                    imprimir_livro_completo(&livros[i]);
                    encontrou = 1;
                }
            }
        }

        if (!encontrou) {
            printf("\nNenhum livro encontrado.\n");
        }

        printf("\nPressione ENTER para continuar...");
        getchar();

    } while (1);
}


/* ========= FUNCOES DE ARQUIVOS ========= */

void salvar_livros() {
    FILE *arq = fopen("livros.txt", "w");

    if (arq == NULL) {
        printf("Erro ao salvar livros.\n");
        return;
    }

    fprintf(arq, "%d\n", qtd_livros);

    for (int i = 0; i < qtd_livros; i++) {
        fprintf(arq,
            "%d;%s;%s;%s;%d;%d;%d\n",
            livros[i].codigo,
            livros[i].titulo,
            livros[i].autor,
            livros[i].editora,
            livros[i].ano_publicacao,
            livros[i].exemplares_total,
            livros[i].exemplares_disponiveis
        );
    }

    fclose(arq);
}

void salvar_usuarios() {
    FILE *arq = fopen("usuarios.txt", "w");

    if (arq == NULL) {
        printf("Erro ao salvar usuarios.\n");
        return;
    }

    fprintf(arq, "%d\n", qtd_usuarios);

    for (int i = 0; i < qtd_usuarios; i++) {
        fprintf(arq,
            "%d;%s;%s;%s;%d;%d;%d\n",
            usuarios[i].matricula,
            usuarios[i].nome,
            usuarios[i].curso,
            usuarios[i].telefone,
            usuarios[i].data_cadastro.dia,
            usuarios[i].data_cadastro.mes,
            usuarios[i].data_cadastro.ano
        );
    }

    fclose(arq);
}

void salvar_emprestimos() {
    FILE *arq = fopen("emprestimos.txt", "w");

    if (arq == NULL) {
        printf("Erro ao salvar emprestimos.\n");
        return;
    }

    fprintf(arq, "%d\n", qtd_emprestimos);

    for (int i = 0; i < qtd_emprestimos; i++) {
        fprintf(arq,
            "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
            emprestimos[i].codigo,
            emprestimos[i].matricula_usuario,
            emprestimos[i].codigo_livro,
            emprestimos[i].data_emprestimo.dia,
            emprestimos[i].data_emprestimo.mes,
            emprestimos[i].data_emprestimo.ano,
            emprestimos[i].data_prevista_devolucao.dia,
            emprestimos[i].data_prevista_devolucao.mes,
            emprestimos[i].data_prevista_devolucao.ano,
            emprestimos[i].status
        );
    }

    fclose(arq);
}

void carregar_livros() {
    FILE *arq = fopen("livros.txt", "r");
    if (arq == NULL) return;

    fscanf(arq, "%d\n", &qtd_livros);

    for (int i = 0; i < qtd_livros; i++) {
        fscanf(arq,
            "%d;%100[^;];%80[^;];%60[^;];%d;%d;%d\n",
            &livros[i].codigo,
            livros[i].titulo,
            livros[i].autor,
            livros[i].editora,
            &livros[i].ano_publicacao,
            &livros[i].exemplares_total,
            &livros[i].exemplares_disponiveis
        );
    }

    fclose(arq);
}

void carregar_usuarios() {
    FILE *arq = fopen("usuarios.txt", "r");
    if (arq == NULL) return;

    fscanf(arq, "%d\n", &qtd_usuarios);

    for (int i = 0; i < qtd_usuarios; i++) {
        fscanf(arq,
            "%d;%100[^;];%50[^;];%15[^;];%d;%d;%d\n",
            &usuarios[i].matricula,
            usuarios[i].nome,
            usuarios[i].curso,
            usuarios[i].telefone,
            &usuarios[i].data_cadastro.dia,
            &usuarios[i].data_cadastro.mes,
            &usuarios[i].data_cadastro.ano
        );
    }

    fclose(arq);
}

void carregar_emprestimos() {
    FILE *arq = fopen("emprestimos.txt", "r");
    if (arq == NULL) return;

    fscanf(arq, "%d\n", &qtd_emprestimos);

    for (int i = 0; i < qtd_emprestimos; i++) {
        fscanf(arq,
            "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
            &emprestimos[i].codigo,
            &emprestimos[i].matricula_usuario,
            &emprestimos[i].codigo_livro,
            &emprestimos[i].data_emprestimo.dia,
            &emprestimos[i].data_emprestimo.mes,
            &emprestimos[i].data_emprestimo.ano,
            &emprestimos[i].data_prevista_devolucao.dia,
            &emprestimos[i].data_prevista_devolucao.mes,
            &emprestimos[i].data_prevista_devolucao.ano,
            &emprestimos[i].status
        );
    }

    fclose(arq);
}

void backup_arquivos() {
    system("copy livros.txt backup_livros.txt");
    system("copy usuarios.txt backup_usuarios.txt");
    system("copy emprestimos.txt backup_emprestimos.txt");
}

/* ========= MAIN ========= */

int main() {

    carregar_livros();
    carregar_usuarios();
    carregar_emprestimos();

    int opcao;

    do {
        printf("\n=== SISTEMA DE GERENCIAMENTO DE BIBLIOTECA ===\n");
        printf("1 - Controle de Usuarios\n");
        printf("2 - Controle de Livros\n");
        printf("3 - Emprestimos / Devolucoes\n");
        printf("0 - Encerrar programa\n");
        opcao = ler_inteiro("Opcao: ", 0, 3);


        switch (opcao) {
            case 1:
                menu_usuarios();
                break;
            case 2:
                menu_livros();
                break;
            case 3:
                menu_emprestimos();
                break;
            case 0:
                backup_arquivos();
                salvar_livros();
                salvar_usuarios();
                salvar_emprestimos();
                printf("Encerrando sistema...\n");
                break;
            default:
                printf("Opcao invalida!\n");
                printf("Pressione ENTER para continuar...");
                getchar();
        }

    } while (opcao != 0);

    return 0;
}
