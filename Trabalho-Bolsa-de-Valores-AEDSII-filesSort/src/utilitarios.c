#include "headers_Aux.h"

int globalComparacoes = 0;

int menuPrincipal(){
    char choice[5];
    int invalidChoiceFlag = 1; // se for escolha invalida
    
    do{
        system(CLEAR); system(CLEAR);

        txtLogo();

        puts("1. Criar\\recriar base dados");
        puts("2. Ordenar base dados");
        puts("3. Imprimir base de dados(demorado dependendo do tamanho)");
        puts("4. Buscar usuario investidor");
        puts("5. Sair");
        printf("\n\nEscolha uma opcao: ");

        fgets(choice, 4, stdin);
        invalidChoiceFlag = (choice[0] == '\n' || strlen(choice) > 2 || choice[0] < '\x31'|| choice[0] > '\x35');

        if(invalidChoiceFlag){
            fputs("\nOpcao invalida ... ", stdout);
            choice[0] = 0; //coloca um caracter fora do range pra while continuar
            getchar();
        }
           
        
    }while(invalidChoiceFlag);

    return atoi(choice);
}

// funcao que verifica e e chama a funcao initializeDataBase
int createDatabase(char *nameBase){
    unsigned int quant_Reg = 0;
    char answer = 'n';
    int sinalcreate = 1;
    
    FILE *file = fopen(DATABASE, "r+b"); 

    if(file){ //verifica se ja existe
        fputs("\nBase de dados ja existe, reescrever[s/n]? ", stdout);
        scanf("%c", &answer);
        printf("\n");
        if(answer != 's' && answer != 'S')
            sinalcreate = 0;
        else
            fclose(file);
    }
    
    if(sinalcreate){
        fputs("\nDigite a quantidade de registro a criar: ", stdout);
        scanf("%u", &quant_Reg);
        
        file = fopen(DATABASE, "w+b");
        if(file){
            initializeDataBase(file, quant_Reg);//fun��o geradora do banco dados
        }else{
            perror("Erro");
            exit(EXIT_FAILURE);
        }

        FILE *test = NULL;
        char fileNameOrdered[40];
        sprintf(fileNameOrdered, "%s_ordered.dat", nameBase);
        test = fopen(fileNameOrdered, "rb");
        if(test){
            fclose(test);
            remove(fileNameOrdered);
        }

        test = fopen("tabHash.dat", "rb");
        if(test){
            fclose(test);
            remove("tabHash.dat");
        }
        
    }

    rewind(file);
    quant_Reg = countRegUser(file); //verifica se foi criado corretamente a quantidade de registro tambem retorna
    
    if(quant_Reg > 0){
        fputs("Base de dados OK!\n", stdout);
        printf("%d registros no arquivo...", quant_Reg);
    }else{
        //printf("Algum erro na base de dados! Delete-o e inicie o programa novamente..\n");
        // no caso por alguma raz�o o arquivo for criado mas o programa for interrompido
        //ex: usuario apertar ctrl+c nesse ponto
        fclose(file);
        printf("\nBase de dados com erro! deletando, tente criar novamente...\n\n");
        if(remove(DATABASE)){
            perror("Falha na base de dados");
            exit(EXIT_FAILURE);
        }
            
        file = NULL;
    }
    setbuf(stdin, NULL);
    getchar();

    fclose(file);
    return quant_Reg;
}

//gera o base de dados desordenado
void initializeDataBase(FILE* file, int numberRecords) {
    TUserInvest user;
    srand(time(NULL));

    //cria um array com a quantidade de registro informada
    //esta etapa gera um array com ids aleatorio para posteriormente gerar uma base de dados desordenada 
    int *vControlCod = (int*)malloc(numberRecords * sizeof(int)); 
    int nRand = (rand() + 1)%numberRecords + 1;
    vControlCod[0] = nRand;
    
    //preenche o array com codigos nao repetidos
    for(int i = 1;i < numberRecords;i++){
        nRand = (rand() + 1)%numberRecords + 1;
        for(int j = 0; j < i; j++){
            if(nRand == vControlCod[j]){ //2� for e if reinicia nrand at� ser gerado um numero nao repetido
                nRand = (rand() + 1)%numberRecords + 1;
                j = -1; //-1 por que ao voltar no for j incrementa pra ser 0;
            }   
        }
        vControlCod[i] = nRand;
    }

    //preenche registros
    for (int i = 0; i < numberRecords; i++) {
        user.cod = vControlCod[i]; 
        sprintf(user.cpf, "%03d.%03d.%03d-%02d",(vControlCod[i]%1000), (vControlCod[i]%1000), (vControlCod[i]%1000), (vControlCod[i]%100));
        sprintf(user.nome, "Investidor %02d", vControlCod[i]);
        for(int i=0;i<5;i++){
            sprintf(user.carteira[i].tag, "IVT%d", i);
            sprintf(user.carteira[i].nome, "investimento %d", i);
            user.carteira[i].classe = rand() % 7; // gera entre 0 e 6
            user.carteira[i].quantidade = rand()%1000; // entra 0 e 1000 ativos
            
        }
        user.hash.prox = -1;
        user.hash.oculpado = true;

        fseek(file, i * sizeRegUser(), SEEK_SET);
        writeUser(&user, file);
    }

    free(vControlCod);


    /*-------------------------- LOG ---------------------------------------*/

    FILE *logfile = fopen(LOGFILE, "w");
    char sClock[30];
    strClock(sClock);
    fprintf(logfile, "## %s ##\n", sClock);
    fprintf(logfile,"Nova base de dados gerada:\n");
    fprintf(logfile,"Quantidade: %d usuarios Investidores.\n", numberRecords);
    fprintf(logfile, "----------------------------------------------------\n");
    fclose(logfile);

    /*-------------------------- LOG ---------------------------------------*/
}

/*
le M registros->quant_M para memoria e prenche vetor de ponteiros vetfunc
*/ 
int carregaRegistroParaMemoria(FILE *file, int quant_M, TUserInvest *vetUser[]){
    int i;
    //int sizeUser = sizeRegUser();//**************
    for(i = 0; i < quant_M; i++){
         //fseek(file, i*sizeUser, SEEK_SET);//************
         vetUser[i] = readUser(file);
         if(feof(file)){
            break;
         }
            
    }

    return i; // confirma a quantidade de registros lidos
}

//ordena toda a base de daddos utilizando classificacao interna e intercalacao simples 
//retorna a quantidade de particoes geradas
int geraParticaoArquivoInterna(FILE *fullDataBasefile, int quant_M, char* nameBase){

    TUserInvest *vetUser[quant_M];
    int idPartition = 0;

    //nRegistrosEfetivo e MAX_REG_PARTITION pode diferir somente na ultima parti��o
    int nRegistrosEfetivo; // controla e armazena os registros carregados

    //int quantidadeDeArquivosParticao = (int)ceil(count_Reg(fullDataBasefile)/(double)quant_M);
    char namePartition[40];
 
    FILE *partFile; // ponteiro para arquivo da parti�ao a ser criada

    rewind(fullDataBasefile);
    while(!feof(fullDataBasefile)){
        
        globalComparacoes++;
        //nRegistrosEfetivo e MAX_REG_PARTITION pode diferir somente na ultima particao
        nRegistrosEfetivo = carregaRegistroParaMemoria(fullDataBasefile, quant_M, vetUser);
        if(nRegistrosEfetivo == 0)
            break;
        
        //ordena os M registro na memoria
        insertionSort(vetUser, nRegistrosEfetivo);
        
        //gera particao com nome personalizado
        sprintf(namePartition, "%s%s_partition_%d", PATHTEMP, nameBase, idPartition + 1);

        partFile = fopen(namePartition, "w+b");
        if(!partFile){
            perror("Erro ao criar arquivo de particao. Tente novamente");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nRegistrosEfetivo; i++) {
            fseek(partFile, i * sizeRegUser(), SEEK_SET);//************
            writeUser(vetUser[i], partFile);
        }
       
        fclose(partFile);
        
        //desaloca os ponteiros do vetor dos registros lidos para as particoes
        for(int i = 0; i < nRegistrosEfetivo; i++){
            free(vetUser[i]);
        }    

        idPartition++;
    }
    printf("particao geradas ...\n");
    return idPartition;
}

int geraParticaoArquivoSubstituicao(FILE *fullDataBasefile, int quant_M, char* nameBase){

    TUserInvest *vetUser[quant_M];
    int idPartition = 1;
    int nRegistrosEfetivo; // controla e armazena os M registros carregados
    char namePartition[40];
 
    FILE *partFile; // ponteiro para arquivo da particao a ser criada

    rewind(fullDataBasefile);
    //le M registros e armazena em vetUser
    nRegistrosEfetivo = carregaRegistroParaMemoria(fullDataBasefile, quant_M, vetUser);
 
    // array de flags todas as posicao recebe 1(congelado) ou 0(nao congelado)
    int posCongelada[nRegistrosEfetivo];
    int allCongelados = 0;
    int ultimasPos = 0;
    
    //inicializa todos flags em zero(nao congelados)
    for(int i = 0; i < nRegistrosEfetivo; i++)
        posCongelada[i] = false;

    //abri/cria a primeira particao de saida
    sprintf(namePartition, "%s%s_partition_%d", PATHTEMP, nameBase, idPartition);
    partFile = fopen(namePartition, "w+b");
    if(!partFile){
        perror("Erro ao criar arquivo de particao. Tente novamente");
        exit(EXIT_FAILURE);
    }
    

    TUserInvest fMenor = *vetUser[0]; // em vez de apontar faz copia para melhor controle e facilidade de codificar
    int posMenor = 0;
    while(ultimasPos < nRegistrosEfetivo){
        
        //seleciona o menor
        for(int i = 0; i < nRegistrosEfetivo; i++){
            globalComparacoes++;
            if(!posCongelada[i] && vetUser[i] != NULL){ // se nao estiver congelada
                if(vetUser[i]->cod < fMenor.cod){
                    fMenor = *vetUser[i];
                    posMenor = i;
                }     
            }         
        }
    
        writeUser(&fMenor, partFile);
       
        //pega o proximo do arquivo de entrada e substitui o menor por ele
        free(vetUser[posMenor]);
        vetUser[posMenor] = readUser(fullDataBasefile);
        if(vetUser[posMenor] != NULL){//testa se recebe null arquivo de entrada no fim
            globalComparacoes++;
            if(vetUser[posMenor]->cod < fMenor.cod){
                posCongelada[posMenor] = true; //considera a posicao congelada
                allCongelados +=1;
            }
            
        }else{
            posCongelada[posMenor] = true;
            allCongelados +=1;
            ultimasPos += 1;             
        }
        
        fMenor.cod = INT_MAX;

        if((allCongelados == nRegistrosEfetivo) && ultimasPos < nRegistrosEfetivo){
            //fecha a particao
            fclose(partFile);
            
            //descongela os registros   
            for(int i = 0; i < nRegistrosEfetivo; i++)
                posCongelada[i] = false;
        
            allCongelados = 0;

            //abri mais uma particao
            sprintf(namePartition, "%s%s_partition_%d", PATHTEMP, nameBase, ++idPartition);
            partFile = fopen(namePartition, "w+b");
            if(!partFile){
                perror("Erro ao criar arquivo de particao. Tente novamente");
                exit(EXIT_FAILURE);
            }
        }

    }

    fclose(partFile);
    printf("particao geradas ...\n");
    return idPartition;
}

void intercala_simple(int numerodeParticoes, char* nameBase){

    char nameFileOut[30];
    char namePartition[30];
    FILE *out;
    TFileStack vet[numerodeParticoes]; // vetor que armazena struct registro e seu respectivo arquivo
    int flagFullOrdered = 0;

    sprintf(nameFileOut, "%s_ordered.dat", nameBase); // arquivo de saida

    // abre cada arquivo de particao
    for(int i = 0; i < numerodeParticoes; i++){
        sprintf(namePartition, "%s%s_partition_%d",PATHTEMP, nameBase, i+1); // nome da particao
        vet[i].file = fopen(namePartition, "rb");
        if(!vet[i].file){
            perror("Erro ao abrir arquivo particao");
            exit(EXIT_FAILURE);
        }
        //amazena o primeiro registro de cada arquivo  
        vet[i].userTopo = readUser(vet[i].file);       
    }

    out = fopen(nameFileOut, "wb");
    if(!out){
        perror("Erro ao criar arquivo de final");
        exit(EXIT_FAILURE);
    }


    //descobre o menor atual
    TUserInvest fMenor = *vet[0].userTopo;
    int posMenor = 0;

    while(flagFullOrdered < numerodeParticoes){
        
        for(int i = 0; i < numerodeParticoes; i++){
            globalComparacoes++;
            if(vet[i].file == NULL){
                continue;
            }

            if(vet[i].userTopo->cod < fMenor.cod){
                fMenor = *vet[i].userTopo;
                posMenor = i;
            }        
        }
        //print(fMenor);
        writeUser(&fMenor, out); // escreve o menor da memoria no arquivo final da saida
        free(vet[posMenor].userTopo);
        vet[posMenor].userTopo = readUser(vet[posMenor].file); //le o proximo do arquivo do menor achado
        //fMenor = *vet[posMenor].userTopo;
        
        globalComparacoes++;
        if(vet[posMenor].userTopo == NULL){
            fMenor.cod = INT_MAX; // coloca um userTopo inexistente com cod grande pra comparacao
            fclose(vet[posMenor].file);
            vet[posMenor].file = NULL;
            flagFullOrdered += 1; // significa a a cada arquivo de particao que acaba fica proximo da ordenacao final
            
        }else{
            fMenor = *vet[posMenor].userTopo;
        }

        
        
    }
    fclose(out);
    
    for(int i = 0; i < numerodeParticoes; i++){
        fclose(vet[i].file);
        free(vet[i].userTopo);
    }
    
}

void intercala_ArvoreVencedor(int numerodeParticoes, char* nameBase){

    char nameFileOut[30];
    FILE *out;

    sprintf(nameFileOut, "%s_ordered.dat", nameBase); // arquivo de saida
    out = fopen(nameFileOut, "wb");
    if(!out){
        perror("Erro ao criar arquivo de final");
        exit(EXIT_FAILURE);
    }
    
    TnoArvore *arv = montaArvoreDeVencedores(numerodeParticoes, nameBase);

    //int itemTemp;
    TUserInvest *userTemp;
    while(arv->user->cod != INT_MAX){
        userTemp = arv->user;
        writeUser(userTemp, out);
        if(arv->f){
            userTemp = readUser(arv->f);
            globalComparacoes++;
            if(userTemp != NULL){
                arv->endfolhaItem->user = userTemp;
            }else{
                fclose(arv->f);
                arv->endfolhaItem->user->cod = INT_MAX;
            }
            recalcula(arv->endfolhaItem->parent);
        }
        
    }

    fclose(out);
}

void menuOrdena(char *fileNameBase){
    system(CLEAR);

    FILE *logfile;
    
    char choice[5];
    int invalidChoiceFlag = 1; // se for escolha invalida

    do{
        txtLogo();

        puts("Qual tipo de ordenacao?\n");
        puts("1. Insertion sort em disco");
        puts("2. Geracao de particao interna - Interc. Simples");
        puts("3. Geracao de particao interna - Interc. Arvore Vencedor");
        puts("4. Geracao de particao substituicao - Interc. Simples");
        puts("5. Geracao de particao substituicao - Interc. Arvore Vencedor");
        puts("6. retornar menu principal");
        printf("\n\nEscolha uma opcao: ");

        fgets(choice, 4, stdin);
        invalidChoiceFlag = (choice[0] == '\n' || strlen(choice) > 2 || choice[0] < '\x31'|| choice[0] > '\x36');

        if(invalidChoiceFlag){
            fputs("\nOpcao invalida ... ", stdout);
            choice[0] = 0; //coloca um caracter fora do range pra while continuar
            getchar();
            system(CLEAR);
            system(CLEAR);
        }
           
        
    }while(invalidChoiceFlag);


    if(atoi(choice) == 6)
        return;

    FILE *fullDataBasefile = fopen(DATABASE, "r+b");

    if(!fullDataBasefile){

        fputs("\nbase de dados inexistente! crie na opcao 1 menu principal...", stdout);
        getchar();
        return;
    }

    globalComparacoes = 0;
    clock_t marcaTempo;

    if(atoi(choice) == 1){
        printf("\naguarde ...");
        system(DELDIR("temp"));
        char fileNameOrdered[40], strcop[100];
        sprintf(fileNameOrdered, "%s_ordered.dat", fileNameBase); //forma o nome ordered
        #ifdef _WIN32
            sprintf(strcop, "copy /Y %s %s >nul 2>&1", DATABASE, fileNameOrdered);
        #elif __linux__
            sprintf(strcop, "cp -rf %s %s >/dev/null 2>&1", DATABASE, fileNameOrdered);
        #else
            printf("sistema nao compatível");
        #endif
        system(strcop);

        FILE *out = fopen(fileNameOrdered, "r+b");

        /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "Ordenado por InsertionSort em disco\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/

        insertion_sort_disco(out, countRegUser(out));
        fclose(out);

    }else if(atoi(choice) == 2){
        marcaTempo = clock();
        
        system(DELDIR("temp"));
        int numeroDeParticoes;
        system(CREATEDIR("temp"));
        numeroDeParticoes = geraParticaoArquivoInterna(fullDataBasefile, MAX_REG_PARTITION, fileNameBase);
        intercala_simple(numeroDeParticoes, fileNameBase);

        marcaTempo = clock() - marcaTempo; //contagem do tempo decorrido

         /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "Ordenado por Classif. Interna | Interc. Simples\n");
        fprintf(logfile, "Numero de comparacoes: %d\n", globalComparacoes);
        fprintf(logfile, "Tempo de ordenacao: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
        fprintf(logfile, "----------------------------------------------------\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/
    }else if(atoi(choice) == 3){
        marcaTempo = clock();
        
        system(DELDIR("temp"));
        int numeroDeParticoes;
        system(CREATEDIR("temp"));
        numeroDeParticoes = geraParticaoArquivoInterna(fullDataBasefile, MAX_REG_PARTITION, fileNameBase);
        intercala_ArvoreVencedor(numeroDeParticoes, fileNameBase);

        marcaTempo = clock() - marcaTempo; //contagem do tempo decorrido

         /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "Ordenado por Classif. Interna | Interc. Arvores de vencedor\n");
        fprintf(logfile, "Numero de comparacoes: %d\n", globalComparacoes);
        fprintf(logfile, "Tempo de ordenacao: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
        fprintf(logfile, "----------------------------------------------------\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/
    }else if(atoi(choice) == 4){
        marcaTempo = clock();
        
        system(DELDIR("temp"));
        int numeroDeParticoes;
        system(CREATEDIR("temp"));
        numeroDeParticoes = geraParticaoArquivoSubstituicao(fullDataBasefile, MAX_REG_PARTITION, fileNameBase);
        intercala_simple(numeroDeParticoes, fileNameBase);
        //intercala_ArvoreVencedor(numeroDeParticoes, fileNameBase);

        marcaTempo = clock() - marcaTempo; //contagem do tempo decorrido

         /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "Ordenado por Classif. Substituicao | Interc. Simples\n");
        fprintf(logfile, "Numero de comparacoes: %d\n", globalComparacoes);
        fprintf(logfile, "Tempo de ordenacao: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
        fprintf(logfile, "----------------------------------------------------\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/
    }else{
         marcaTempo = clock();

        system(DELDIR("temp"));
        int numeroDeParticoes;
        system(CREATEDIR("temp"));
        numeroDeParticoes = geraParticaoArquivoSubstituicao(fullDataBasefile, MAX_REG_PARTITION, fileNameBase);
        intercala_ArvoreVencedor(numeroDeParticoes, fileNameBase);

        marcaTempo = clock() - marcaTempo; //contagem do tempo decorrido

        /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "Ordenado por Classif. Substituicao | Interc. Arvores de vencedor\n");
        fprintf(logfile, "Numero de comparacoes: %d\n", globalComparacoes);
        fprintf(logfile, "Tempo de ordenacao: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
        fprintf(logfile, "----------------------------------------------------\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/
    }

    fclose(fullDataBasefile);
    
    fputs("\nArquivo ordenado..\n", stdout);

    fputs("\n\nTudo certo, pressione alguma tecla para continuar...", stdout);
    getchar();


    menuOrdena(fileNameBase); // repete a função se quiser fazer algo a mais
}
//essas duas funcoes abaixo imprimem o banco de dados ordenado ou desordenado
static int menuPrintFileBase(){
    char choice[5];
    int invalidChoiceFlag = 1; // se for escolha invalida
    do{
        txtLogo();
        puts("Qual voce quer imprimir?\n");
        puts("1. Ordenada");
        puts("2. Original");
        puts("3. retornar menu principal");
        printf("\n\nEscolha uma opcao: ");

        fgets(choice, 4, stdin);
        invalidChoiceFlag = (choice[0] == '\n' || strlen(choice) > 2 || choice[0] < '\x31'|| choice[0] > '\x33');

        if(invalidChoiceFlag){
            fputs("\nOpcao invalida ... ", stdout);
            choice[0] = 0; //coloca um caracter fora do range pra while continuar
            getchar();
            system(CLEAR);
            system(CLEAR);
        }
           
        
    }while(invalidChoiceFlag);

    return atoi(choice);
}

void printFileBase(const char *fileNameBase){
    system(CLEAR);
    FILE *file, *debugFile;
    int choice = menuPrintFileBase();
    char fileNameOrdered[40], debugFileName[40];
    sprintf(fileNameOrdered, "%s_ordered.dat", fileNameBase);

    switch (choice){
    case 1:
        sprintf(debugFileName, "debug_%s_ordered.txt", fileNameBase);
        file = fopen(fileNameOrdered, "rb");
        //file = fopen("temp\\base_dados_partition_5", "rb");
        if(!file){
            fputs("Voce nao ordenou ou criou a base de dados ainda!", stdout);
            getchar();
            return;
        }

        break;
    case 2:
        sprintf(debugFileName, "debug_%s.txt", fileNameBase);
        file = fopen(DATABASE, "rb");
        if(!file){
            fputs("\nbase de dados inexistente! crie na opcao 1 menu principal...", stdout);
            getchar();
            return;
        }
        break;
    default:
        return;
        break;
    }

    debugFile = fopen(debugFileName, "w");

    TUserInvest *user;
    user = readUser(file);
    while(!feof(file)){
        //printUser(*user);
        printUserDisco(*user, debugFile);
        puts("-----//-----------//");
        free(user);
        user = readUser(file);
    }

    fclose(file);
    fclose(debugFile);
    fputs("\n\nTudo certo, pressione alguma tecla para continuar...", stdout);
    getchar();

    printFileBase(fileNameBase);
}

// essas dua funcoes faz as busca binária ou sequencial
void menuBusca(const char *fileNameBase){
    system(CLEAR); 
    char fileNameOrdered[40];
    sprintf(fileNameOrdered, "%s_ordered.dat", fileNameBase);

    char choice[5];
    int invalidChoiceFlag = 1; // se for escolha invalida
    do{
        txtLogo();

        puts("Qual tipo de busca?\n");
        puts("1. Sequencial no arquivo original(nao ordenado)");
        puts("2. Sequencial no arquivo ordenado");
        puts("3. Binaria no arquivo ordenado");
        puts("4. Tabela Hash");
        puts("5. retornar menu principal");
        printf("\n\nEscolha uma opcao: ");

        fgets(choice, 4, stdin);
        invalidChoiceFlag = (choice[0] == '\n' || strlen(choice) > 2 || choice[0] < '\x31'|| choice[0] > '\x35');

        if(invalidChoiceFlag){
            fputs("\nOpcao invalida ... ", stdout);
            choice[0] = 0; //coloca um caracter fora do range pra while continuar
            getchar();
            system(CLEAR);
            system(CLEAR);
        }
           
        
    }while(invalidChoiceFlag);

    if(atoi(choice) == 5)
        return;

    if(atoi(choice) == 4){
        menuHash(DATABASE, "tabHash.dat");
        return;
    }

    FILE *file;
    FILE *logfile;
    int codSearch;
    TUserInvest *userFound = NULL;
    fputs("digite o codido do investidor a procurar: ", stdout);
    scanf("%d", &codSearch);

    setbuf(stdin, NULL);

    if(atoi(choice) == 1){
        file = fopen(DATABASE, "rb");
         if(!file){
            fputs("Voce nao criou a base de dados ainda!", stdout);
            getchar();
            return;
        }
        /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "busca sequencial no arquivo nao ordenado\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/
        userFound = itemSearchSequencial(codSearch, file, countRegUser(file));

    }else if(atoi(choice) == 2){
        file = fopen(fileNameOrdered, "rb");
         if(!file){
            fputs("Voce nao criou ou ordenou a base de dados ainda!", stdout);
            getchar();
            return;
        }
         /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "busca sequencial no arquivo ordenado\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/
        userFound = itemSearchSequencial(codSearch, file, countRegUser(file));

    }else if(atoi(choice) == 3){
        file = fopen(fileNameOrdered, "rb");
         if(!file){
            fputs("Voce nao criou ou ordenou a base de dados ainda!", stdout);
            getchar();
            return;
        }
         /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "busca binaria\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/
        userFound = itemSearchBin(codSearch, file, countRegUser(file));

    }

    if(userFound){
        fputs("Investidor encontrado!\n", stdout);
        printUser(*userFound);
        /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "Encontrado: Sim\n");
        fprintf(logfile, "----------------------------------------------------\n");
        fclose(logfile);;
        /*-------------------------- LOG ---------------------------------------*/
    }else{
        fputs("Investidor nao encontrado! ", stdout);
        /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "Encontrado: Nao\n");
        fprintf(logfile, "----------------------------------------------------\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/
    }

    getchar();
    fclose(logfile);
    fclose(file);
    menuBusca(fileNameBase);
}

void menuHash(char *fileBaseName, char *fileHashName){
    system(CLEAR);
    char choice[5];
    int invalidChoiceFlag = 1; // se for escolha invalida
    do{
        txtLogo();
        puts("1. Montar tabela");
        puts("2. buscar registro");
        puts("3. imprimir tabela");
        puts("4. retornar menu principal");
        printf("\n\nEscolha uma opcao: ");

        fgets(choice, 4, stdin);
        invalidChoiceFlag = (choice[0] == '\n' || strlen(choice) > 2 || choice[0] < '\x31'|| choice[0] > '\x34');

        if(invalidChoiceFlag){
            fputs("\nOpcao invalida ... ", stdout);
            choice[0] = 0; //coloca um caracter fora do range pra while continuar
            getchar();
            system(CLEAR);
            system(CLEAR);
        }
           
        
    }while(invalidChoiceFlag);

    if(atoi(choice) == 4)
        return;

    if(atoi(choice) == 1){
        FILE *test = fopen(fileHashName, "rb"); //verifica se existe
        if(!test){
            FILE *test2 = fopen(fileBaseName, "rb");
            if(!test2){
                fputs("Voce nao criou a base de dados ainda!", stdout);
                getchar();
                return;  
            }else{
                fclose(test2);
            }
            int qtd_compart;
            printf("Quantos compartimentos quer na tabela?: ");
            scanf("%d", &qtd_compart);
            setbuf(stdin, NULL);
            criaTabelaHash(qtd_compart, fileHashName);
            montaTabelaHash(fileBaseName, fileHashName);
            fputs("tabela criada e montada com sucesso!\n", stdout);
        }else{
            fputs("Tabela para o base atual ja existe OK!\n", stdout);
        }
        fclose(test);
    }else if(atoi(choice) == 2){
        globalComparacoes = 0;

        FILE *fileBase, *fileHash, *logfile;
        int codSearch;
        TUserInvest *userFound;

        fputs("digite o codido do investidor a procurar: ", stdout);
        scanf("%d", &codSearch);
        setbuf(stdin, NULL);

        fileBase = fopen(fileBaseName, "rb");
        fileHash = fopen(fileHashName, "rb");
        if(!fileBase || !fileHash){
            fclose(fileBase);
            fclose(fileHash);
            fputs("Voce nao criou a base de dados ou tabela hash!", stdout);
            getchar();
            return;
        }

         /*-------------------------- LOG ---------------------------------------*/
        logfile = fopen(LOGFILE, "a");
        fprintf(logfile, "busca por tabela Hash\n");
        fclose(logfile);
        /*-------------------------- LOG ---------------------------------------*/

        clock_t marcaTempo;
        marcaTempo = clock();

        userFound = buscaInHash(codSearch, fileBase, fileHash);

        marcaTempo = clock() - marcaTempo;

        if(userFound){
            fputs("Investidor encontrado!\n", stdout);
            printUser(*userFound);
            /*-------------------------- LOG ---------------------------------------*/
            logfile = fopen(LOGFILE, "a");;
            fprintf(logfile,"Identificador procurado: %d\n", codSearch);
            fprintf(logfile, "Numero de comparacoes: %d\n", globalComparacoes);
            fprintf(logfile, "Tempo de busca: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
            fprintf(logfile, "Encontrado: Sim\n");
            fprintf(logfile, "----------------------------------------------------\n");
            fclose(logfile);;
            /*-------------------------- LOG ---------------------------------------*/
        }else{
            fputs("Investidor nao encontrado! ", stdout);
            /*-------------------------- LOG ---------------------------------------*/
            logfile = fopen(LOGFILE, "a");
            fprintf(logfile,"Identificador procurado: %d\n", codSearch);
            fprintf(logfile, "Numero de comparacoes: %d\n", globalComparacoes);
            fprintf(logfile, "Tempo de busca: %.3f segundos\n", ((double)marcaTempo / CLOCKS_PER_SEC));
            fprintf(logfile, "Encontrado: Nao\n");

            fprintf(logfile, "----------------------------------------------------\n");
            fclose(logfile);
            /*-------------------------- LOG ---------------------------------------*/
        }
        
        fclose(fileBase);
        fclose(fileHash);
    }else{
        printf("\n");
        printTabela(fileBaseName, fileHashName);
    }

    getchar();

    menuHash(fileBaseName, fileHashName); 
}

void strClock(char *sClock){
    time_t hora = time(NULL);//pode ser tambem time(&hora)
    struct tm *timeInfo = localtime(&hora);
    memset(sClock, 0, strlen(sClock));
    strftime(sClock, 30, "%x %X", timeInfo);
}

void txtLogo(){
    /*------ relogio -------*/
    char sClock[30];
    strClock(sClock);
    printf("\nHorario: %s\n\n", sClock);
    /*------ relogio -------*/

    puts("-------------------------------------------");
    puts("## Sistema de controle de Investimentos ##");
    puts("-------------------------------------------\n");
}
