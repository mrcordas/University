#ifndef INVEST_H
#define INVEST_H

#define DATABASE "base_dados.dat" // nome do arquivo da base de dados

#define MAX_REG_PARTITION 10 //maximo registros por partição

typedef struct{
    int prox; //armazena a proxima posição fseek ou -1 no ultimo nulo da lista encadeada em disco
    bool oculpado; // armazena se o registro ja foi apagado ou nao
}infoHash;

typedef struct Investidor {// tamenho completo 299bytes
    int cod; //codigo do usuario investidor
    char nome[50];
    char cpf[20];
    TAtivo carteira[5]; // simula cada investidor com 5 ativos na carteira
    infoHash hash;
} TUserInvest;

void writeUser(TUserInvest *user, FILE *out);

TUserInvest *readUser(FILE *in);

void printUser(TUserInvest user);
void printUserDisco(TUserInvest user, FILE *f);

int sizeRegUser();

int countRegUser(FILE *file);

/*Bunca sequencial*/
TUserInvest *itemSearchSequencial(int cod, FILE *file, int tam);

/*Bunca binaria*/
TUserInvest *itemSearchBin(int cod, FILE *file, int tam);

void insertion_sort_ram(FILE *file, int tam);

void insertion_sort_disco(FILE *file, int tam);

void selection_sort_disco(FILE *file, int tam);

void insertionSort(TUserInvest *vetUser[], int tam);

#endif
