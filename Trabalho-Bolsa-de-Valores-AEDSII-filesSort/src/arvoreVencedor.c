#include "headers_Aux.h"

extern int globalComparacoes;

///***** INICIO ARVORE **** //
// cria um no de arvore vazio
TnoArvore *createNoArvore() {
    TnoArvore *new = (TnoArvore *) malloc(sizeof(TnoArvore));
    new->user = NULL;
    new->f = NULL;
    new->endfolhaItem = NULL;
    new->parent = NULL;
    new->left = NULL;
    new->right = NULL;
    return new;
}

//controi arvore
TnoArvore* montaArvoreDeVencedores(int numerodeParticoes, char* nameBase){

    //cria lista
    TnoList *lista;
    lista = createList();

    char namePartition[40];
    
    // faz os nós folhas
    for(int i = 0; i < numerodeParticoes; i++){
        sprintf(namePartition, "%s%s_partition_%d",PATHTEMP, nameBase, i+1); // nome da particao
        TnoArvore *folha = createNoArvore(); // cria um no folha da arvore

        folha->f = fopen(namePartition, "rb");  //atrela aquivo ao no folha
        if(!(folha->f)){
            perror("Erro ao abrir arquivo particao");
            exit(EXIT_FAILURE);
        }
       folha->user = readUser(folha->f); // coloca o user daquele arquivo
       folha->endfolhaItem = folha; // atrela o endereço do no folha a variavel especifica ao cria a lista de no folhas
       insertItemList(lista, folha); //insere o no da arvore na lista
    }

    // /*DEBUG*/
    // printList(lista); // imprimi lista
    // printf("quant itens: %d\n", sizeList(lista));
    // /*DEBUG*/

    int currentSizeList = sizeList(lista);
    TnoArvore *filho1, *filho2, *pai, *filhoMenor; // nos filhos de arvore para criar pai
    
    while(currentSizeList > 1){// enquanto a total tiver mais de um item

        //executa em pares, e para quantidade de lista impares joga ultimo elemento pra ultimo
        //proximo nivel de lista/arvore
        for(int i = 0; i < (currentSizeList/2); i++){
            filho1 = removeItemList(lista);
            filho2 = removeItemList(lista);
            globalComparacoes++;
            if(filho1->user->cod < filho2->user->cod){
                filhoMenor = filho1;
            }else{
                filhoMenor = filho2;
            }

            //cria e ajusta campos do no pai
            pai = createNoArvore();
            pai->user = filhoMenor->user;
            pai->f = filhoMenor->f;
            pai->endfolhaItem = filhoMenor->endfolhaItem;
            pai->left = filho1;
            pai->right = filho2;
            
            //ajusta filhos apontando pra o pai
            filho1->parent = filho2->parent = pai;

            // /*DEBUG*/
            // printf("pai: %02d, filho1: %02d, filho2 %02d, folha: %02d-->%p\n", pai->user->cod,   filho1->user->cod, filho2->user->cod, pai->endfolhaItem->user->cod, pai->endfolhaItem);
            // /*DEBUG*/
            
            //insere no pai criado atual no final da lista
            insertItemList(lista, pai);
        }

        //verifica se e uma lista impar para jogar o que era ultimo inicial pra ultimo na lista atual do proximo nivel
        if(currentSizeList & 1){
            TnoArvore *aux = removeItemList(lista);
            insertItemList(lista, aux);
        }

        currentSizeList = sizeList(lista);

    }

    TnoArvore *raiz = removeItemList(lista);
    
    return raiz;
    //fecha arquivos
    // TnoList *aux = lista->prox;
    // while(filhoMenor){
    //     fclose(filhoMenor->noTopoFile->f);
    //     filhoMenor = filhoMenor->prox;
    // }

    // deallocateList(lista);

}

//imprimi um ramo de no folha
void printRamo(TnoArvore *parent){
    if(parent == NULL){
        return;
    }
        
    printf("Esquerdo: %d\n", parent->left->user->cod);
    printf("pai: %d\n", parent->user->cod);
    printf("Direito: %d\n", parent->right->user->cod);
    printRamo(parent->parent);
}

// recebe o no pai da folha e recalcula o ramo da arvore(Obs: substitua/exclua primeiro o no folha)
void recalcula(TnoArvore *parent){
    if(parent == NULL){
        return;
    }

    TnoArvore *aux;
    globalComparacoes++;
    if(parent->left->user->cod < parent->right->user->cod){
        aux = parent->left;
    }else{
        aux = parent->right;
    }

    //parent->user->cod = aux->user->cod;
    parent->user = aux->user;
    parent->f = aux->f;
    parent->endfolhaItem = aux->endfolhaItem;

    recalcula(parent->parent);
}

///***** FIM ARVORE **** //



///***** INICIO LISTA **** //
TnoList *createList(){
    TnoList *new = (TnoList *)malloc(sizeof(TnoList)); //cabeca
    if(!new)
        return NULL;
    new->noTopoFile = NULL; //esse nao e usado, pois fica na cabeca
    new->prox = NULL; //sera o primeiro elemento
    return new;
}

int isempty(TnoList *list){
    return (list->prox == NULL);
}

void insertItemList(TnoList *list, TnoArvore *x){
    TnoList *new = (TnoList *)malloc(sizeof(TnoList));
    new->noTopoFile = x;
    new->prox = NULL;

    TnoList *aux = list;
    while(aux->prox){
        aux = aux->prox;
    }
    aux->prox = new;
}

TnoArvore* removeItemList(TnoList *list){
     if(isempty(list))
        return NULL;

    TnoArvore *x = list->prox->noTopoFile; //primeiro elemento da lista
    TnoList *aux = list->prox;
    list->prox = aux->prox;
    free(aux);
    return x;
}

void printList(TnoList *list){
    TnoList *aux = list->prox;
    if(isempty(list)){
        puts("Lista vazia...\n");
        return;
    }    
    while(aux){
        printf("%d\n", aux->noTopoFile->user->cod);
        aux = aux->prox;
    }
}

int sizeList(TnoList *list){
    if(isempty(list))
        return 0;
    TnoList *aux = list->prox;
    int count = 0;
    while(aux){
        count++;
        aux = aux->prox;
    }
    return count;
}

void deallocateList(TnoList *list){
    TnoList *aux = list, *aux2;
    while(aux){
        aux2 = aux->prox;
        free(aux);
        aux = aux2;
    }
}

///***** FIM LISTA **** //


