
#include "headers_Aux.h"

extern int globalComparacoes;

//salva um usuario no arquivo
void writeUser(TUserInvest *user, FILE *out) { 
    fwrite(&user->cod, sizeof(int), 1, out);
    fwrite(user->nome, sizeof(char), sizeof(user->nome), out);
    fwrite(user->cpf, sizeof(char), sizeof(user->cpf), out);
    for(int i = 0; i < 5; i++){
        fwrite(user->carteira[i].tag, sizeof(char), sizeof(user->carteira[i].tag), out);
        fwrite(user->carteira[i].nome, sizeof(char), sizeof(user->carteira[i].nome), out);
        fwrite(&user->carteira[i].classe, sizeof(user->carteira[i].classe), 1, out);
        fwrite(&user->carteira[i].quantidade, sizeof(int), 1, out);
    }
}

//le um usuario do arquivo
TUserInvest *readUser(FILE *in) {
    TUserInvest *user = (TUserInvest *) malloc(sizeof(TUserInvest));
    if(0 >= fread(&user->cod, sizeof(int), 1, in)) {
        free(user);
        return NULL;
    }
    fread(user->nome, sizeof(char), sizeof(user->nome), in);
    fread(user->cpf, sizeof(char), sizeof(user->cpf), in);
    for(int i = 0; i < 5; i++){
        fread(user->carteira[i].tag, sizeof(char), sizeof(user->carteira[i].tag), in);
        fread(user->carteira[i].nome, sizeof(char), sizeof(user->carteira[i].nome), in);
        fread(&user->carteira[i].classe, sizeof(user->carteira[i].classe), 1, in);
        fread(&user->carteira[i].quantidade, sizeof(int), 1, in);
    };
    return user;
}

//imprimir um usuario e carteiras de investimentos
void printUser(TUserInvest user) {
  printf("\nCodigo: %02d", user.cod);
  printf("\nCPF: %s", user.cpf);
  printf("\nNome: %s\n", user.nome);
  printf("Ativos na carteira:\n");
  char strClasse[15];
  for(int i = 0; i < 5; i++){
    switch (user.carteira[i].classe){
        case DOLAR:
            sprintf(strClasse, "DOLAR");
            break;
        case ETF:
            sprintf(strClasse, "ETF");
            break;
        case FII:
            sprintf(strClasse, "FII");
            break;
        case MULTIMERCADOS:
            sprintf(strClasse, "MULTIMERCADO");
            break;
        case CRIPTOMOEDAS:
            sprintf(strClasse, "CRIPTOMOEDA");
            break;
        case OURO:
            sprintf(strClasse, "OURO");
            break;    
        default:
            sprintf(strClasse, "ACAO");
            break;
    }
    printf("%3d de %s|%s|%s|\n", user.carteira[i].quantidade, user.carteira[i].tag, user.carteira[i].nome, strClasse);
  }
  printf("\n");
}

//tamanho da estrutura de usuario
int sizeRegUser(){
    return sizeof(TUserInvest) - 17; //18 é o alinhamento da estrutura para arquivo
}

//contagem de usuarios
int countRegUser(FILE *file) {
    //fseek(file, 0, SEEK_END);

    /*
        melhorar a compatibilidade com outros sistemas e biblioteca
        não utilizei fseek com SEEK_END
    */
    rewind(file);
    char byte;
    while(!feof(file)){
        fread(&byte, sizeof(char), 1, file);
    }  

    int tam = round(ftell(file) / (double)sizeRegUser());
    rewind(file);
    return tam;
}

//busca sequencial
TUserInvest *itemSearchSequencial(int cod, FILE *file, int tam){ 
    TUserInvest *userSearch;
    int sizeUser = sizeRegUser();//*****************
    bool encontrado = false;
    
    int comparcoes = 0;  
    clock_t marcaTempo;
    marcaTempo = clock();

    for(int i = 0; i < tam; i++){
        fseek(file, i*sizeUser, SEEK_SET);//************
        userSearch = readUser(file);
        comparcoes++; 
        if(userSearch->cod == cod){
            encontrado = true;
            break;
        }
    }

    marcaTempo = clock() - marcaTempo; //contagem do tempo decorrido
    printf("\nIdentificador procurado: %d\n", cod);
    printf("Numero de comparacoes: %d\n", comparcoes);
    printf("Tempo de busca: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));

    /*-------------------------- LOG ---------------------------------------*/
    FILE *logfile = fopen(LOGFILE, "a");
    fprintf(logfile,"Identificador procurado: %d\n", cod);
    fprintf(logfile,"Numero de comparacoes: %d\n", comparcoes);
    fprintf(logfile,"Tempo de busca: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
    fclose(logfile);
    /*-------------------------- LOG ---------------------------------------*/
   
    if(encontrado)
        return userSearch;

    return NULL;  //se não encotrar
}

//busca binária
TUserInvest *itemSearchBin(int cod, FILE *file, int tam) {

    TUserInvest* userSearch;

    int comparcoes = 0;
    bool encontrado = false;  
    clock_t marcaTempo;
    marcaTempo = clock();

    int left = 0, right = tam - 1;
    while(left <= right){
        
        int middle = (left + right) / 2;
        fseek(file, middle * sizeRegUser(), SEEK_SET);
        userSearch = readUser(file);

        comparcoes++;
        if(cod == userSearch->cod) {
            encontrado = true;
            break;
        } else if(userSearch->cod < cod) {
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }
    marcaTempo = clock() - marcaTempo; //contagem do tempo decorrido

    printf("\nIdentificador procurado: %d\n", cod);
    printf("Numero de comparacoes: %d\n", comparcoes);
    printf("Tempo de busca: %.3f segundos\n\n", ((double)marcaTempo / CLOCKS_PER_SEC));

    /*-------------------------- LOG ---------------------------------------*/
    FILE *logfile = fopen(LOGFILE, "a");
    fprintf(logfile,"Identificador procurado: %d\n", cod);
    fprintf(logfile,"Numero de comparacoes: %d\n", comparcoes);
    fprintf(logfile, "Tempo de busca: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
    fclose(logfile);
    /*-------------------------- LOG ---------------------------------------*/
    
    if(encontrado)
        return userSearch;

    return NULL;  //se não encotrar
}


void insertion_sort_ram(FILE *file, int tam) {
    TUserInvest *v[tam];
    //readUser o arquivo e coloca no vetor
    rewind(file); //posiciona cursor no inicio do arquivo
    TUserInvest *u = readUser(file);
    int i = 0;
    while (!feof(file)) {
        v[i] = u;
        u = readUser(file);
        i++;
    }
    //faz o insertion sort
    for (int j = 1; j < tam; j++) {
        TUserInvest *u = v[j];
        i = j - 1;
        while ((i >= 0) && (v[i]->cod > u->cod)) {
            v[i + 1] = v[i];
            i = i - 1;
        }
        v[i+1] = u;
    }
    //grava vetor no arquivo, por cima do conteúdo anterior
    rewind(file);
    for (int i = 0; i < tam; i++) {
        writeUser(v[i], file);
    }
    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(file);
}

void insertion_sort_disco(FILE *file, int tam) {
    int comparcoes = 0;
    clock_t marcaTempo;
    marcaTempo = clock();

    int i;
    //faz o insertion sort
    for (int j = 2; j <= tam; j++) {
        //posiciona o arquivo no registro j
        fseek(file, (j-1) * sizeRegUser(), SEEK_SET);
        TUserInvest *uj = readUser(file);
        //printf("\n********* investidor atual: %d\n", uj->cod);
        i = j - 1;
        //posiciona o cursor no registro i
        fseek(file, (i-1) * sizeRegUser(), SEEK_SET);
        TUserInvest *ui = readUser(file);
        //printf("ui = %d\n", ui->cod);
        while ((i > 0) && (ui->cod > uj->cod)) {
            comparcoes++;
            //posiciona o cursor no registro i+1
            fseek(file, i * sizeRegUser(), SEEK_SET);
            //printf("Salvando investidor %d na posicao %d\n", ui->cod, i+1);
            writeUser(ui, file);
            i = i - 1;
            //lÃª registro i
            fseek(file, (i-1) * sizeRegUser(), SEEK_SET);
            ui = readUser(file);
            //printf("ui = %d; i = %d\n", ui->cod, i);
        }
        //posiciona cursor no registro i + 1
        fseek(file, (i) * sizeRegUser(), SEEK_SET);
        //printf("*** Salvando investidor %d na posicao %d\n", uj->cod, i+1);
        //writeUser registro j na posiÃ§Ã£o i
        writeUser(uj, file);
    }


    marcaTempo = clock() - marcaTempo; //contagem do tempo decorrido

    printf("\n\nNumero de comparacoes: %d\n", comparcoes);
    printf("Tempo de ordenacao: %.3f segundos\n\n", ((double)marcaTempo / CLOCKS_PER_SEC));

    /*-------------------------- LOG ---------------------------------------*/
    FILE *logfile = fopen(LOGFILE, "a");
    fprintf(logfile,"Numero de comparacoes: %d\n", comparcoes);
    fprintf(logfile, "Tempo de ordenacao: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
    fprintf(logfile,"--------------------------------------\n");
    fclose(logfile);
    /*-------------------------- LOG ---------------------------------------*/

    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(file);
}

void selection_sort_disco(FILE *file, int tam) {
    rewind(file); //posiciona cursor no inicio do arquivo
    //faz o selection sort
    for (int i = 1; i < tam; i++) {
        //posiciona cursor no registro i
        fseek(file, (i - 1) * sizeRegUser(), SEEK_SET);
        TUserInvest *ui = readUser(file);
        printf("\n******** Registro atual: %d, pos = %d\n", ui->cod, i);
        //procura menor elemento do restante do arquivo (registros i+1 atÃ© tam)
        //Assume que menor Ã© o prÃ³ximo (i + 1)
        // i + 1 sempre existe pois for vai atÃ© tam - 1
        fseek(file, i * sizeRegUser(), SEEK_SET);
        TUserInvest *fmenor = readUser(file);
        int posmenor = i + 1;
        for (int j = i + 2; j <= tam; j++) {
            TUserInvest *uj = readUser(file);
            if ((uj->cod) < (fmenor->cod)) {
                fmenor = uj;
                posmenor = j;
            }
        }
        //Troca fmenor de posiÃ§Ã£o com ui, se realmente for menor
        if (fmenor->cod < ui->cod) {
            printf("Troca %d na posiÃ§Ã£o %d por %d na posiÃ§Ã£o %d\n", ui->cod, i, fmenor->cod, posmenor);
            fseek(file, (posmenor - 1) * sizeRegUser(), SEEK_SET);
            writeUser(ui, file);
            fseek(file, (i - 1) * sizeRegUser(), SEEK_SET);
            writeUser(fmenor, file);
        } else printf("NÃ£o troca...");
    }

    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(file);
}
/*
InsertionSort classico
vetor de ponteiros pra TUserInvest's alocadas
*/
void insertionSort(TUserInvest *vetUser[], int tam){
    TUserInvest *user;
    int j;
    for(int i = 1; i < tam; i++){
        user = vetUser[i];
        j = i - 1;
        globalComparacoes++;
        while((j>=0) && vetUser[j]->cod > user->cod){
            vetUser[j+1] = vetUser[j];
            j--;
        }
        vetUser[j + 1] = user;
    }
}