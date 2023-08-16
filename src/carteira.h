#ifndef CART_H
#define CART_H

//constantes tipos de ativos
typedef enum {
    ACOES, //açoes em bolsa de valores
    ETF, //fundos de indices, acoes, renda fixa
    FII, // fundo imobiliario
    MULTIMERCADOS,
    CRIPTOMOEDAS,
    DOLAR,
    OURO 
}ClasseAtivo; //representa valores entre 0 e 6 

//representa as caracteristicas do ativo alocado do investidor
typedef struct{
    char tag[7]; // tag
    char nome[30];
    ClasseAtivo classe;
    int quantidade;
}TAtivo;

#endif