#ifndef UTIL_H
#define UTIL_H

//armazena um arquivo e um regitro correspondente a esse arquivo
typedef struct _FileXUser{
    FILE *file; //arquivo da partiçao 
    TUserInvest *userTopo; //registro lido do topo partição
}TFileStack;

int menuPrincipal();

int createDatabase(char *nameBase);

void initializeDataBase(FILE* file, int numberRecords);

int carregaRegistroParaMemoria(FILE *file, int quant_M, TUserInvest *vetUser[]);

int geraParticaoArquivoInterna(FILE *fullDataBasefile, int quant_M, char* nameBase);

int geraParticaoArquivoSubstituicao(FILE *fullDataBasefile, int quant_M, char* nameBase);

void intercala_simple(int numerodeParticoes, char* nameBase);

void intercala_ArvoreVencedor(int numerodeParticoes, char* nameBase);

void menuOrdena(char *fileNameBase);

void printFileBase(const char  *fileNameBase);

void menuBusca(const char *fileNameBase);

void menuHash(char *fileBase, char *fileHash);

void strClock(char *sClock);

void txtLogo();
#endif
