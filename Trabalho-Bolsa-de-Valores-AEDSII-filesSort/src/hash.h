#ifndef HASH_H
#define HASH_H


void criaTabelaHash(int compartimentos, char *tabelaHash);

void montaTabelaHash(char *baseDeDados, char *tabelaHash);

int calculaHash(int cod, int compatimentos);

TUserInvest *buscaInHash(int cod,  FILE *fileBase, FILE *fileHash);

void printTabela(char *baseDeDados, char *tabelaHash);

int countCompartimento(FILE *file);
#endif