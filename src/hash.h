#ifndef HASH_H
#define HASH_H

//#include "investidor.h"
#include "headers_Aux.h"
//usado pra criar a estrutura de manipulação da informação principal(TuserInvest)
//em conjunto do controle da tabela hash a ser criada apartir do banco de dados original
typedef struct{
    TUserInvest *user;
    int prox; // armazena a proxima posição fseek ou -1 no ultimo nulo da lista encadeada em disco
    bool oculpado; // armazena se o registro ja foi apagado ou nao
}TUserDataHash;

void writeDataHash(TUserDataHash *userDataHash, FILE *out);

TUserDataHash *readDataHash(FILE *in);

#endif