#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int quantum;

typedef struct processo
{
    char nome[2];
    int tempo;
    struct processo *proximo;
} Processo;

typedef struct
{
    char nome[42];
    Processo *inicio;
    Processo *fim;
} Fila;

Fila *inicializaFila(char *nome)
{
    Fila *f = malloc(sizeof(Fila));
    f->inicio = NULL;
    f->fim = NULL;
    strcpy(f->nome, nome);

    return f;
}

void remover(Fila *f, Processo *p)
{
    if (f->inicio == p)
    {
        f->inicio = p->proximo;
    }
    else
    {
        Processo *aux = f->inicio;
        while (aux->proximo != p)
        {
            aux = aux->proximo;
        }
        aux->proximo = aux->proximo->proximo;
    }
}

void adicionaProcesso(Fila *f, char *nome, int tempo)
{
    Processo *novo = (Processo *)malloc(sizeof(Processo));
    Processo *aux;
    strcpy(novo->nome, nome);
    novo->tempo = tempo;
    novo->proximo = NULL;
    if (f->inicio == NULL)
    {
        f->inicio = novo;
        f->fim = novo;
    }
    else
    {
        aux = f->inicio;
        while (aux->proximo)
        {
            aux = aux->proximo;
        }
        aux->proximo = novo;
        f->fim = novo;
    }
}

void tranfereProcesso(Fila *f, Processo *p, Fila *fe)
{
    if (p->tempo - quantum < 0)
    {
        adicionaProcesso(f, p->nome, 0);
    }
    else
    {
        adicionaProcesso(f, p->nome, p->tempo - quantum);
    }
    remover(fe, fe->inicio);
}

void imprimeLista(char *fila, Fila *f)
{
    if (f->inicio == NULL)
        printf("%s: vazia\n", fila);
    else
    {
        printf("%s:", fila);
        Processo *aux = f->inicio;
        while (aux)
        {
            if (aux->proximo == NULL)
                printf(" %s = %d", aux->nome, aux->tempo);
            else
                printf(" %s = %d <-", aux->nome, aux->tempo);
            aux = aux->proximo;
        }
        printf("\n");
    }
}

void imprime(Fila *prontos, Fila *espera, Fila *execucao, Fila *finalizados)
{
    printf("Estado atual das filas de processos:\n");
    imprimeLista("Pronto", prontos);
    imprimeLista("Espera", espera);
    imprimeLista("Execucao", execucao);
    imprimeLista("Finalizados", finalizados);
}

void tranfereTudo(Fila *origem, Fila *destino)
{
    Processo *aux = origem->inicio;
    while (aux)
    {
        adicionaProcesso(destino, aux->nome, aux->tempo);
        remover(origem, aux);
        aux = aux->proximo;
    }
}

void simulacao(Fila *prontos, Fila *espera, Fila *execucao, Fila *finalizados)
{
    Processo *aux = execucao->inicio;
    while (execucao->inicio != NULL)
    {
        imprime(prontos, espera, execucao, finalizados);
        printf("%s esta executando...\n", aux->nome);
        if (aux->tempo <= quantum)
        {
            tranfereProcesso(finalizados, aux, execucao);
            printf("%s terminou a execucao, %s foi adicionado a fila de finalizados\n\n", aux->nome, aux->nome);
        }
        else
        {
            tranfereProcesso(espera, aux, execucao);
            printf("Quantum expirou, %s sofreu preempcao\n\n", aux->nome);
            printf("%s foi adicionado a fila de espera\n\n", aux->nome);
        }
        printf("-------------------------------------\n");
        aux = aux->proximo;
    }
    if (execucao->inicio == NULL && espera->inicio == NULL)
        return;
    printf("A fila de execucao esta vazia, movendo processos para a fila de prontos\n");
    tranfereTudo(espera, prontos);
    imprime(prontos, espera, execucao, finalizados);
    printf("\n-------------------------------------\n");
    printf("Processos foram adicionados a fila de execucao\n");
    tranfereTudo(prontos, execucao);
    simulacao(prontos, espera, execucao, finalizados);
}

int main()
{
    Fila *prontos = inicializaFila("Prontos");
    Fila *espera = inicializaFila("Espera");
    Fila *execucao = inicializaFila("Execução");
    Fila *finalizados = inicializaFila("Finalizados");

    int p;
    char c[3];
    FILE *fp;

    char nome_arquivo[50];

    scanf("%s", nome_arquivo);
    fp = fopen(nome_arquivo, "rb");
    if (fp == NULL)
    {
        printf("Arquivo nao encontrado.");
        return 1;
    }
    fscanf(fp, "%d", &quantum);
    while (fscanf(fp, "%s %d", c, &p) != EOF)
    {
        adicionaProcesso(prontos, c, p);
    }
    imprime(prontos, espera, execucao, finalizados);

    printf("\nQuantum: %d\n", quantum);
    printf("\n----------> Iniciando execucao\n");
    printf("Processos foram adicionados a fila de execucao\n");
    tranfereTudo(prontos, execucao);
    simulacao(prontos, espera, execucao, finalizados);
    imprime(prontos, espera, execucao, finalizados);
    printf("Nao ha mais processos a serem executados\n----------> Simulacao finalizada");
    return 0;
}
