#ifndef HEADERS_AUX
#define HEADERS_AUX
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "carteira.h"
#include "investidor.h"
#include "utilitarios.h"
#include "arvoreVencedor.h"
#include "hash.h"

#define LOGFILE "log.txt"

#ifdef _WIN32 
    #define CLEAR "cls"
    #define CREATEDIR(strdir) "md " #strdir " >nul 2>&1"
    #define DELDIR(strdir) "rmdir /S /Q " #strdir " >nul 2>&1"
    #define PATHTEMP "temp\\"
#elif __linux__
    #define CLEAR "clear"
    #define CREATEDIR(strdir) "mkdir " #strdir " >/dev/null 2>&1"
    #define DELDIR(strdir) "rm -rf" #strdir " >/dev/null 2>&1"
    #define PATHTEMP "temp/"
#else
    #error "sistema nao compatível"
#endif
#endif