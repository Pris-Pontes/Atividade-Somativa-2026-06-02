#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "contas.dat"
#define MAX_NOME 50
#define MAX_CPF  15

typedef struct {
    int    conta;
    char   nome[MAX_NOME];
    char   cpf[MAX_CPF];
    double saldo;
    int    ativo;
} Cliente;

void   cadastrar(FILE *fp);
void   consultar(FILE *fp);
void   atualizar(FILE *fp);
void   encerrar_conta(FILE *fp);
void   listar(FILE *fp);
void   listar_rewind(FILE *fp);
int    total_registros(FILE *fp);
void   limpar_buffer(void);
void   exibir_cliente(const Cliente *c, int pos);

int main(void) {
    FILE *fp = fopen(ARQUIVO, "r+b");

    if (!fp) {
        fp = fopen(ARQUIVO, "w+b");
        if (!fp) {
            fprintf(stderr, "Erro ao abrir/criar arquivo.\n");
            return EXIT_FAILURE;
        }
        printf("[INFO] Arquivo '%s' criado com sucesso.\n\n", ARQUIVO);
    }

    int opcao;
    do {
        printf("SISTEMA DE MANUTENÇÃO DE CONTAS\n");
        printf("1. Cadastrar cliente \n");
        printf("2. Consultar cliente\n");
        printf("3. Atualizar saldo\n");
        printf("4. Encerrar conta\n");
        printf("5. Listar todos os clientes\n");
        printf("6. Relistar (rewind)\n");
        printf("7. Encerrar programa\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        limpar_buffer();

        switch (opcao) {
            case 1: cadastrar(fp);      break;
            case 2: consultar(fp);      break;
            case 3: atualizar(fp);      break;
            case 4: encerrar_conta(fp); break;
            case 5: listar(fp);         break;
            case 6: listar_rewind(fp);  break;
            case 7: printf("\nPrograma encerrado.\n"); break;
            default: printf("\nOpção inválida!\n");
        }
    } while (opcao != 7);

    fclose(fp);
    return EXIT_SUCCESS;
}

void cadastrar(FILE *fp) {
    Cliente c;
    int pos;

    printf("\n--- CADASTRAR CLIENTE ---\n");
    printf("Posição (índice a partir de 0): ");
    scanf("%d", &pos);
    limpar_buffer();

    if (pos < 0) {
        printf("Posição inválida.\n");
        return;
    }

    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
    Cliente tmp;
    if (fread(&tmp, sizeof(Cliente), 1, fp) == 1 && tmp.ativo == 1) {
        printf("Posição %d já possui cliente ativo (conta %d - %s).\n",
               pos, tmp.conta, tmp.nome);
        printf("Deseja sobrescrever? (1=Sim / 0=Não): ");
        int resp;
        scanf("%d", &resp);
        limpar_buffer();
        if (resp != 1) return;
    }

    c.conta = pos;
    printf("Nome: ");
    fgets(c.nome, MAX_NOME, stdin);
    c.nome[strcspn(c.nome, "\n")] = '\0';

    printf("CPF:  ");
    fgets(c.cpf, MAX_CPF, stdin);
    c.cpf[strcspn(c.cpf, "\n")] = '\0';

    printf("Saldo inicial: R$ ");
    scanf("%lf", &c.saldo);
    limpar_buffer();

    c.ativo = 1;

    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
    if (fwrite(&c, sizeof(Cliente), 1, fp) != 1) {
        printf("Erro ao gravar registro.\n");
        return;
    }
    fflush(fp);
    printf("\nCliente cadastrado com sucesso na posição %d!\n", pos);
}

void consultar(FILE *fp) {
    int pos;
    printf("\n--- CONSULTAR CLIENTE ---\n");
    printf("Número da conta (posição): ");
    scanf("%d", &pos);
    limpar_buffer();

    int total = total_registros(fp);
    if (pos < 0 || pos >= total) {
        printf("Posição %d inexistente no arquivo.\n", pos);
        return;
    }

    Cliente c;
    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
    if (fread(&c, sizeof(Cliente), 1, fp) != 1) {
        printf("Erro ao ler registro.\n");
        return;
    }

    if (!c.ativo) {
        printf("Conta na posição %d está ENCERRADA.\n", pos);
        return;
    }
    exibir_cliente(&c, pos);
}

void atualizar(FILE *fp) {
    int pos;
    printf("\n--- ATUALIZAR SALDO ---\n");
    printf("Número da conta (posição): ");
    scanf("%d", &pos);
    limpar_buffer();

    int total = total_registros(fp);
    if (pos < 0 || pos >= total) {
        printf("Posição %d inexistente.\n", pos);
        return;
    }

    Cliente c;
    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
    if (fread(&c, sizeof(Cliente), 1, fp) != 1) {
        printf("Erro ao ler registro.\n");
        return;
    }

    if (!c.ativo) {
        printf("Conta encerrada. Operação negada.\n");
        return;
    }

    printf("Saldo atual: R$ %.2f\n", c.saldo);
    printf("Novo saldo:  R$ ");
    scanf("%lf", &c.saldo);
    limpar_buffer();

    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
    fwrite(&c, sizeof(Cliente), 1, fp);
    fflush(fp);
    printf("Saldo atualizado com sucesso!\n");
}

void encerrar_conta(FILE *fp) {
    int pos;
    printf("\n--- ENCERRAR CONTA ---\n");
    printf("Número da conta (posição): ");
    scanf("%d", &pos);
    limpar_buffer();

    int total = total_registros(fp);
    if (pos < 0 || pos >= total) {
        printf("Posição %d inexistente.\n", pos);
        return;
    }

    Cliente c;
    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
    if (fread(&c, sizeof(Cliente), 1, fp) != 1) {
        printf("Erro ao ler registro.\n");
        return;
    }

    if (!c.ativo) {
        printf("Conta já está encerrada.\n");
        return;
    }

    printf("Confirma encerramento da conta de %s? (1=Sim / 0=Não): ", c.nome);
    int resp;
    scanf("%d", &resp);
    limpar_buffer();
    if (resp != 1) {
        printf("Operação cancelada.\n");
        return;
    }

    c.ativo = 0;
    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
    fwrite(&c, sizeof(Cliente), 1, fp);
    fflush(fp);
    printf("Conta encerrada com sucesso.\n");
}

void listar(FILE *fp) {
    printf("\n--- LISTA DE CLIENTES ---\n");
    int total = total_registros(fp);
    if (total == 0) {
        printf("Nenhum registro encontrado.\n");
        return;
    }

    fseek(fp, 0L, SEEK_SET);
    Cliente c;
    int encontrou = 0;
    for (int i = 0; i < total; i++) {
        if (fread(&c, sizeof(Cliente), 1, fp) == 1 && c.ativo) {
            exibir_cliente(&c, i);
            encontrou = 1;
        }
    }
    if (!encontrou)
        printf("Nenhuma conta ativa encontrada.\n");
}

void listar_rewind(FILE *fp) {
    printf("\n--- RELISTAR COM rewind() ---\n");
    rewind(fp);   /* posiciona o ponteiro no início do arquivo */

    Cliente c;
    int i = 0, encontrou = 0;
    while (fread(&c, sizeof(Cliente), 1, fp) == 1) {
        if (c.ativo) {
            exibir_cliente(&c, i);
            encontrou = 1;
        }
        i++;
    }
    if (!encontrou)
        printf("Nenhuma conta ativa encontrada.\n");
}

int total_registros(FILE *fp) {
    fseek(fp, 0L, SEEK_END);
    long bytes = ftell(fp);
    return (int)(bytes / sizeof(Cliente));
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exibir_cliente(const Cliente *c, int pos) {
    printf("Posição : %-4d\n", pos);
    printf("Conta   : %-4d\n", c->conta);
    printf("Nome    : %-25s\n", c->nome);
    printf("CPF     : %-15s\n", c->cpf);
    printf("Saldo   : R$ %12.2f\n", c->saldo);
    printf(" Status  : %-7s\n", c->ativo ? "ATIVO" : "ENCERRADO");
}
