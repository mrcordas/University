#include "headers_Aux.h"
extern int globalComparacoes;
//criar uma tabela hash vazia com m compartimentos
void criaTabelaHash(int compartimentos, char *tabelaHash){
    FILE *file = fopen(tabelaHash, "w+b");
    int ponteiroValor = -1; // representa o ponteiro pra lista  e/ou endereço a buscar no outro arquivo
    if(!file){
        perror("erro");
        return;   
    }
    //escreve -1(hash vazia)
    for(int i = 0; i < compartimentos; i++)
        fwrite(&ponteiroValor, sizeof(int), 1, file);

    fclose(file);      
}

//cria a extrutura hash da base de dados
void montaTabelaHash(char *baseDeDados, char *tabelaHash){

    FILE *dados = fopen(baseDeDados, "r+b");
    if(!dados){
        fputs("Voce nao criou a base de dados ainda!", stdout);
        getchar();
        return;   
    }

    FILE *hash = fopen(tabelaHash, "r+b");
    if(!hash){
        fputs("tabela nao inicializada!", stdout);
        getchar();
        return;   
    }

    TUserInvest *x;
    int sizeTable = countCompartimento(hash);
    int posDados, posHash;
    int ponteiroValor;

    while(true){
        posDados = ftell(dados); // armazena a posição original da base dados
        //printf("%d ", ftell(dados)); //debug
        x = readUser(dados);// le do base pra associar e calcular posicao da hash
        if(x){
            posHash = calculaHash(x->cod, sizeTable);
            //printf("poshash = %d, ", posHash); //debug
            fseek(hash, posHash*sizeof(int), SEEK_SET);
            fread(&ponteiroValor, sizeof(int), 1, hash); // le o valor da tabela hash
            fseek(hash, posHash*sizeof(int), SEEK_SET); // volta na posicao antes da leitura;
            
            /*Aqui pecorre a tabela e a base da dados para fazer a lista encadeada em disco*/
            if(ponteiroValor != -1){
                int posTemp;
                while(ponteiroValor != -1){
                    //printf("ponteiro valor %d ", ponteiroValor); //debug
                    fseek(dados, ponteiroValor, SEEK_SET);
                    posTemp = ftell(dados); //armazena a posição para sobreescrer o satus do ponteiro prox
                    x = readUser(dados);
                    
                    //printf("user %d ", x->cod); //debug
                    ponteiroValor = x->hash.prox;
                }
                x->hash.prox = posDados;
                //rescreve atualizando a base de dados com ponteiro prox
                fseek(dados, posTemp, SEEK_SET); 
                writeUser(x, dados);
                fflush(dados);
                //volta ao arquivo base na posição pronta pra continuar a leitura
                fseek(dados, posDados + sizeRegUser(), SEEK_SET); 
            }else{
                fwrite(&posDados, sizeof(int), 1, hash);
            }
           
            //printf("%d\n", x->cod); //debug
        }else{
            break;
        }
          
        free(x);
    }

    fclose(dados);
    fclose(hash);
}

//formula de calcula da hash - metodo da divisao
int calculaHash(int cod, int compatimentos){
    return (cod % compatimentos);
}
TUserInvest *buscaInHash(int cod, FILE *fileBase, FILE *fileHash){
    
    TUserInvest *user = NULL;
    int sizeCompTable = countCompartimento(fileHash);
    int posHash = calculaHash(cod, sizeCompTable);
    int valorPosHash;

    fseek(fileHash, posHash*sizeof(int), SEEK_SET);
    if(!fread(&valorPosHash, sizeof(int), 1, fileHash))
        perror("Erro");

    while(valorPosHash != -1){
        globalComparacoes++;
        fseek(fileBase, valorPosHash, SEEK_SET);
        user = readUser(fileBase);
        if(user->cod == cod)
            break;

       
        valorPosHash = user->hash.prox;
        user = NULL;
    }   

    return user; 
}

void printTabela(char *baseDeDados, char *tabelaHash){
    int p, pos=0;
    TUserInvest *x = NULL;
    FILE *base = fopen(baseDeDados, "rb");
    if(!base){
        perror("Erro");
        getchar();
        return;   
    }
    FILE *hash = fopen(tabelaHash, "rb");
    if(!hash){
        fputs("tabela nao inicializada!", stdout);
        getchar();
        return;   
    }

    FILE *debugHash = fopen("debugTabHash.txt", "wb");
    if(!debugHash){
        perror("Erro:");
        getchar();
        return;   
    }

    fprintf(debugHash, "[Compartimento] ->  Lista encadeada com codigo e (endereco na base dados)\n\n");
    //printf("[Compartimento] ->  Lista encadeada com codigo e (endereco na base dados)\n\n");
    while(true){
        if(!fread(&p, sizeof(int), 1, hash))
            break;
        //printf("    [ %02d ]      -> ", pos);
        fprintf(debugHash, "    [ %02d ]      -> ", pos);
        if(p != -1){
             while(p != -1){
                fseek(base, p, SEEK_SET);
                x = readUser(base);
                //printf("%3d(%d)\t-> ", x->cod, p);
                fprintf(debugHash,"%3d(%d)\t-> ", x->cod, p);
                p = x->hash.prox;
                free(x);
             }
             //printf("%d", p);
             fprintf(debugHash, "%d", p);
        }else{
            //printf("%3d -> ", p);
            fprintf(debugHash, "%3d -> ", p);
        }

        //puts("\n");
        fputs("\n", debugHash);
        pos++;

    }

    fclose(base);
    fclose(hash);
    fclose(debugHash);
}

int countCompartimento(FILE *file){
    rewind(file);
    char byte;
    while(!feof(file)){
        fread(&byte, sizeof(char), 1, file);
    }  

    int tam = round(ftell(file) / (double)sizeof(int));
    rewind(file);
    return tam;
}