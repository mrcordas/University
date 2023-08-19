#include "hash.h"

//salva em um arquivo o usuario e status e endereço do proximo da lista encadeada
void writeDataHash(TUserDataHash *userDataHash, FILE *out){
    writeUser(userDataHash->user, out);
    fwrite(&userDataHash->prox, sizeof(int), 1, out);
    fwrite(&userDataHash->oculpado, sizeof(bool), 1, out);
}

//le um usuario do arquivo e info da dataHash
TUserDataHash *readDataHash(FILE *in) {
    TUserDataHash *userDataHash = (TUserDataHash *) malloc(sizeof(TUserDataHash));
    userDataHash->user = readUser(in);
    fread(userDataHash->prox, sizeof(int), 1, in);
    fread(userDataHash->oculpado, sizeof(bool), 1, in);

    return userDataHash;
}