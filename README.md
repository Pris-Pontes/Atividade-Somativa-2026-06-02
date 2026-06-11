# Sistema de Manutenção de Contas em C

Sistema de gerenciamento de contas bancárias utilizando arquivo binário em linguagem C.

---

## Funcionalidades

| Opção | Descrição |
|-------|-----------|
| 1 | Cadastrar cliente em posição específica |
| 2 | Consultar cliente pelo número da conta |
| 3 | Atualizar saldo do cliente |
| 4 | Encerrar conta (marcar como inativa) |
| 5 | Listar todos os clientes ativos |
| 6 | Relistar com `rewind()` |
| 7 | Encerrar o programa |

---

## Estrutura do Registro

```c
typedef struct {
    int    conta;           // número da conta / posição no arquivo
    char   nome[50];        // nome do titular
    char   cpf[15];         // CPF do titular
    double saldo;           // saldo atual
    int    ativo;           // 1 = ativo | 0 = encerrado
} Cliente;
```

Cada registro ocupa exatamente `sizeof(Cliente)` bytes no arquivo binário `contas.dat`.

---

## Funções de I/O Binárias Utilizadas

### `fseek()`
Acessa qualquer posição do arquivo diretamente, sem leitura sequencial:
```c
// Para acessar a posição N:
fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
```

### `fread()` / `fwrite()`
Lê e grava registros de tamanho fixo:
```c
fread(&c,  sizeof(Cliente), 1, fp);   // lê 1 registro
fwrite(&c, sizeof(Cliente), 1, fp);   // grava 1 registro
```

### `rewind()`
Posiciona o ponteiro de leitura de volta ao início do arquivo:
```c
rewind(fp);   // equivale a fseek(fp, 0L, SEEK_SET)
```

---

## Organização do Arquivo Binário

```
Byte 0                        Byte sizeof(Cliente)-1
┌──────────────────────────────────────────────────┐
│  Registro 0  (posição 0)                         │
├──────────────────────────────────────────────────┤
│  Registro 1  (posição 1)                         │
├──────────────────────────────────────────────────┤
│  Registro 2  (posição 2)                         │
└──────────────────────────────────────────────────┘
```

O "encerramento" de conta **não remove** o registro fisicamente — ele apenas marca `ativo = 0`. Isso preserva a integridade dos índices das demais posições.

---

## Como Compilar e Executar

```bash
# Compilar
gcc -o contas contas.c

# Executar
./contas
```

> Requer gcc instalado. Compatível com Linux, macOS e Windows (MinGW/MSYS2).

---

## Exemplo de Uso

```
╔══════════════════════════════════╗
║   SISTEMA DE MANUTENÇÃO DE CONTAS ║
╠══════════════════════════════════╣
║  1. Cadastrar cliente             ║
║  2. Consultar cliente             ║
║  3. Atualizar saldo               ║
║  4. Encerrar conta                ║
║  5. Listar todos os clientes      ║
║  6. Relistar (rewind)             ║
║  7. Encerrar programa             ║
╚══════════════════════════════════╝
Opção: 1

--- CADASTRAR CLIENTE ---
Posição (índice a partir de 0): 0
Nome: João da Silva
CPF:  123.456.789-00
Saldo inicial: R$ 1500.00

Cliente cadastrado com sucesso na posição 0!
```

---

## Arquivos do Projeto

```
.
├── contas.c       ← código-fonte principal
├── contas.dat     ← arquivo binário gerado em tempo de execução
└── README.md      ← este arquivo
```

---

## Conceitos Abordados

- Manipulação de arquivos binários em C (`fopen`, `fclose`, `fread`, `fwrite`)
- Acesso aleatório com `fseek()` e `ftell()`
- Reinício de leitura com `rewind()`
- Registros de tamanho fixo com `struct`
- Exclusão lógica (soft delete) vs. exclusão física
