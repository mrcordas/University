#ifndef ARV_
#define ARV_

//no de arvore
typedef struct noArv{
    TUserInvest *user; //armazena o menor valor da subarvore
    FILE *f; // arquivo do atrelado ao no vencedor
    struct noArv *endfolhaItem; // endereço do no folha do que tem o itemvencedor
    struct noArv *parent; // nó pai
    struct noArv *left; // filho esquerdo
    struct noArv *right; // filho direita
}TnoArvore;

TnoArvore *createNoArvore();

TnoArvore* montaArvoreDeVencedores(int numerodeParticoes, char* nameBase);

void printRamo(TnoArvore *parent);

void recalcula(TnoArvore *parent);


/*-------------------------------------------------------------------------------*/
///***** INICIO LISTA **** //
//lista com nós da arvore com o topo da pilha dos arquivos
// lista com cabeça
typedef struct _noList{
    TnoArvore *noTopoFile; // um no
    struct _noList *prox;
}TnoList;

TnoList *createList();
void insertItemList(TnoList *list, TnoArvore *x);
TnoArvore* removeItemList(TnoList *list);
void printList(TnoList *list);
int isempty(TnoList *list);
void deallocateList(TnoList *list);
int sizeList(TnoList *list);

///***** FIM LISTA **** //
/*-------------------------------------------------------------------------------*/


#endif