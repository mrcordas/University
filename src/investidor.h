#ifndef INVEST_H
#define INVEST_H

#define DATABASE "base_dados.dat" // nome do arquivo da base de dados

#define MAX_REG_PARTITION 10 //maximo registros por partição

typedef struct Investidor {
    int cod; //codigo do usuario investidor
    char nome[50];
    char cpf[20];
    TAtivo carteira[5]; // simula cada investidor com 5 ativos na carteira
} TUserInvest;

void writeUser(TUserInvest *user, FILE *out);

TUserInvest *readUser(FILE *in);

void printUser(TUserInvest user);

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
