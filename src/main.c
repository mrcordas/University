#include <locale.h>
#include "headers_Aux.h"

extern int globalComparacoes;

int main(int argc, char const *argv[]){
    /*
    setlocale(LC_ALL, "");

    int tamanhoNomeBase = (int)(strlen(DATABASE)-4);// pega só a parte sem extensão do nome da base dados
    char nameBase[30]; //prefixo das partiçoes
    sprintf(nameBase, "%.*s", tamanhoNomeBase, DATABASE);

    int choice;
    do{
        choice = menuPrincipal();

        switch (choice){
            case 1:
                createDatabase();            
                break;
            case 2:            
                menuOrdena(nameBase);
                break;
            case 3:
                printFileBase(nameBase);
                break;
            case 4:
                menuBusca(nameBase);
                break;
            default:
                break;
        }
    }while(choice != 5);    

    fputs("\n\nObrigado por testar o programa...", stdout);
    */
    printf("size %d\n", sizeof(TUserInvest));
    printf("size %d\n", sizeof(TDataHash));
    getchar();
    return 0;
}
