// Progetto API
// Simulatore di macchine di Turing non deterministiche

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define DIM_TABELLA_HASH_STATI 512
#define NUM_STATI_ACCETTAZIONE 15

// Definizione strutture dati
struct transizione {
    struct transizione * succ;
    int statoPartenza;
    int statoArrivo;
    char carScrittura;
    short int spostamento;
};
typedef struct transizione transizione;

struct pilaTransizioni {
    //struct codaTransizioni * prec;
    struct pilaTransizioni * succ;
    transizione * trans;
    unsigned int numMosse;
    char * nastroDx;
    char * nastroSx;
    int posTestina;
};
typedef struct pilaTransizioni pilaTransizioni;

// Variabili globali
char * nastroDx;
char * nastroSx;
unsigned int maxMosse;
unsigned int mosseCorrenti;
int posTestina = 0;
transizione ***tabellaHashCaratteriLetti;
int stato;
pilaTransizioni * testaPilaTransizioni = NULL;
//codaTransizioni * codaCodaTransizioni = NULL;
int lenDx=0;
int lenSx=-1;
int statiAcc[NUM_STATI_ACCETTAZIONE];

// Prototipi delle funzioni
void LetturaInput();
int CaricaNastro();
int simulaMT();
void incodaTransizione(transizione *);
void impilaTransizione(transizione *);
void cowDx();
void cowSx();
void estendiNastroDx();
void estendiNastroSx();
void pulisciStrutture();
int diAccettazione(transizione *);

// MAIN
int main() {
    LetturaInput();
    // sono arrivato all'input

    while(CaricaNastro()) {

        switch (simulaMT()) {
            case 1:
                printf("1\n");
                break;
            case 0:
                printf("0\n");
                break;
            case -1:
                printf("U\n");
        }
        pulisciStrutture();
    }
    return 0;
}

// Implementazione funzioni
void LetturaInput(){
    char stringa[32];
    int i, j, statoPart, acc, statoArrivo;
    char lett, scr, mov;
    transizione * temp;

    //inizializzo vettore caratteri letti
    tabellaHashCaratteriLetti = (transizione ***) malloc(123* sizeof(transizione**));
    for (i = 0; i < 123; i++) tabellaHashCaratteriLetti[i] = NULL;
    fscanf(stdin, "%s", stringa);
    if (strcmp(stringa, "tr") == 0){
        while(fscanf(stdin, "%d %c %c %c %d", &statoPart, &lett, &scr, &mov, &statoArrivo ) == 5){
            if(tabellaHashCaratteriLetti[lett] == NULL){
                // devo inizializzare tabella hash stati
                tabellaHashCaratteriLetti[lett] = (transizione **)
                        malloc(DIM_TABELLA_HASH_STATI* sizeof(transizione *));
                for (i = 0; i < DIM_TABELLA_HASH_STATI; i++) tabellaHashCaratteriLetti[lett][i] = NULL;
            }

            // la tabella esiste, inserisco in testa
            temp = tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI];
            tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI] = (transizione *)
                    malloc(sizeof(transizione));

            tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI]->succ = temp;
            tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI]->statoPartenza = statoPart;
            tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI]->statoArrivo = statoArrivo;
            tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI]->carScrittura = scr;
            switch (mov){
                case 'S':
                    tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI]->spostamento = 0;
                    break;
                case 'L':
                    tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI]->spostamento = -1;
                    break;
                case 'R':
                    tabellaHashCaratteriLetti[lett][statoPart%DIM_TABELLA_HASH_STATI]->spostamento = 1;
                    break;

            }
        }
        // ho letto le transizioni
        fscanf(stdin, "%s", stringa);
        // stati di accettazione
        for (i = 0; i < NUM_STATI_ACCETTAZIONE; i++) statiAcc[i] = 0;
        while(fscanf(stdin, "%d", &acc) == 1){
            for (i = 0; i < NUM_STATI_ACCETTAZIONE; i++){
                if (statiAcc [i] == 0){
                    statiAcc[i] = acc;
                    break;
                }
            }
        }
        // ho letto stati accettazione
        // max mosse
        fscanf(stdin, "%s", stringa);
        fscanf(stdin, "%d", &maxMosse);
        fscanf(stdin, "%s", stringa);
    }

}

int CaricaNastro(){
    scanf("%ms", &nastroDx);
    if (nastroDx == NULL) return 0;
    lenDx = strlen(nastroDx);
    nastroSx = (char *) malloc(2);
    nastroSx[0] = '_';
    nastroSx[1] = '\0';
    lenSx = 1;
    posTestina = 0;
    return 1;
}

int simulaMT(){
    transizione * temp = NULL;
    pilaTransizioni * tempPila = NULL;
    bool und = false, simula = true;
    pilaTransizioni transizioneDaFare;
    short int estSx = 0;
    int numTransizioniCorr;
    do {
        //Prendo transizoni
        numTransizioniCorr = 0;
        if (mosseCorrenti < maxMosse) {
            if (posTestina >= 0) {
                if (posTestina >= lenDx) estendiNastroDx();
                if (tabellaHashCaratteriLetti[nastroDx[posTestina]] != NULL)
                    temp = tabellaHashCaratteriLetti[nastroDx[posTestina]][stato%DIM_TABELLA_HASH_STATI];
            } else {
                if (abs(posTestina + 1) >= lenSx){
                    estendiNastroSx();
                    estSx = 1;
                }
                if (tabellaHashCaratteriLetti[nastroSx[abs(posTestina + 1)]] != NULL)
                    temp = tabellaHashCaratteriLetti[nastroSx[abs(posTestina + 1)]][stato%DIM_TABELLA_HASH_STATI];
            }

            //metto in coda
            numTransizioniCorr = 0;
            while (temp != NULL) {
                if (temp->statoPartenza == stato) {
                    if (diAccettazione(temp))return 1;
                    if ((temp->statoArrivo == stato &&
                         temp->spostamento == 0 && posTestina >= 0 &&
                         nastroDx[posTestina] == temp->carScrittura) ||
                        (temp->statoArrivo == stato &&
                         temp->spostamento == 0 && posTestina <0 &&
                         nastroSx[abs(posTestina + 1)] == temp->carScrittura)
                            ){

                        und = true;

                    } else if (estSx == 1 &&
                               temp->statoArrivo == stato &&
                               temp->spostamento == -1 &&
                               posTestina <0 &&
                               nastroSx[abs(posTestina + 1)] == '_' ){
                        und = true;
                        estSx = 0;
                    } else if (numTransizioniCorr > 0){
                        impilaTransizione(temp);
                    } else {
                        transizioneDaFare.trans = temp;
                        transizioneDaFare.nastroSx = NULL;
                        transizioneDaFare.nastroDx = NULL;
                        transizioneDaFare.numMosse = mosseCorrenti;
                        transizioneDaFare.posTestina = posTestina;
                        numTransizioniCorr++;
                    }
                }
                temp = temp->succ;
            }
        } else {
            und = true;
        }

        if (numTransizioniCorr ==0) {
            if (testaPilaTransizioni == NULL) break;
            transizioneDaFare.trans = testaPilaTransizioni->trans;
            transizioneDaFare.numMosse = testaPilaTransizioni->numMosse;
            transizioneDaFare.posTestina = testaPilaTransizioni->posTestina;
            transizioneDaFare.nastroSx = testaPilaTransizioni->nastroSx;
            transizioneDaFare.nastroDx = testaPilaTransizioni->nastroDx;
        }
        //prendo dalla testa
        stato = transizioneDaFare.trans->statoArrivo;
        mosseCorrenti = transizioneDaFare.numMosse + 1;
        posTestina = transizioneDaFare.posTestina;
        if (transizioneDaFare.nastroDx != NULL) {
            cowDx();
            free(nastroDx);
            lenDx = strlen(transizioneDaFare.nastroDx);
            nastroDx = (char *) malloc(lenDx+1);
            strcpy(nastroDx, transizioneDaFare.nastroDx);
        }
        if (transizioneDaFare.nastroSx != NULL) {
            cowSx();
            free(nastroSx);
            lenSx = strlen(transizioneDaFare.nastroSx);
            nastroSx = (char *) malloc(lenSx+1);
            strcpy(nastroSx, transizioneDaFare.nastroSx);
        }

        // opero sul nastro
        if (posTestina >= 0) {
            if (nastroDx[posTestina] != transizioneDaFare.trans->carScrittura) {
                // Copia in scrittura
                cowDx();
                // Modifico nastro
                nastroDx[posTestina] = transizioneDaFare.trans->carScrittura;
            }
        } else {
            if (nastroSx[abs(posTestina + 1)] != transizioneDaFare.trans->carScrittura) {
                // Copia in scrittura
                cowSx();

                nastroSx[abs(posTestina + 1)] = transizioneDaFare.trans->carScrittura;
            }
        }

        // muovo testina
        posTestina += transizioneDaFare.trans->spostamento;

        // elimino testa
        if (numTransizioniCorr == 0) {
            tempPila = testaPilaTransizioni;
            testaPilaTransizioni = testaPilaTransizioni->succ;
            if (tempPila->succ == NULL){
                if (tempPila->nastroSx != NULL)free(tempPila->nastroSx);
                if (tempPila->nastroDx != NULL)free(tempPila->nastroDx);
            }else{
                if (tempPila->nastroDx != NULL && tempPila->nastroDx != tempPila->succ->nastroDx)
                    free(tempPila->nastroDx);
                if (tempPila->nastroSx != NULL && tempPila->nastroSx != tempPila->succ->nastroSx)
                    free(tempPila->nastroSx);
            }

            free(tempPila);
        }

    } while (simula);
    if (und) return -1;
    return  0;

}

void impilaTransizione(transizione * trans){
    pilaTransizioni * temp = testaPilaTransizioni;
    testaPilaTransizioni = (pilaTransizioni *) malloc(sizeof(pilaTransizioni));
    testaPilaTransizioni->succ = temp;
    testaPilaTransizioni->nastroDx = NULL;
    testaPilaTransizioni->nastroSx = NULL;
    testaPilaTransizioni->trans = trans;
    testaPilaTransizioni->posTestina = posTestina;
    testaPilaTransizioni->numMosse = mosseCorrenti;
}

void cowDx() {
    pilaTransizioni * tempCoda = testaPilaTransizioni;
    int cont = 0;
    char * stringa = malloc(lenDx +1);
    strcpy(stringa, nastroDx);
    while (tempCoda != NULL && tempCoda->nastroDx == NULL){
        tempCoda->nastroDx = stringa;
        tempCoda = tempCoda->succ;
        cont ++;
    }
    if (cont == 0) free(stringa);
}

void cowSx() {
    pilaTransizioni * tempCoda = testaPilaTransizioni;
    int cont = 0;
    char * stringa = malloc(lenSx +1);
    strcpy(stringa, nastroSx);
    while (tempCoda != NULL && tempCoda->nastroSx == NULL){
        tempCoda->nastroSx = stringa;
        tempCoda = tempCoda->succ;
        cont ++;
    }
    if (cont == 0)free(stringa);
}

void estendiNastroDx(){
    char blankStr[1024];
    int i;
    for (i = 0; i < 1023; i++) blankStr[i] = '_';
    blankStr[1023] = '\0';
    nastroDx = (char *) realloc(nastroDx, lenDx + 1024);
    strcat(nastroDx, blankStr);
    lenDx += 1023;
}

void estendiNastroSx(){
    char blankStr[2048];
    int i;
    for (i = 0; i < 2047; i++) blankStr[i] = '_';
    blankStr[2047] = '\0';
    nastroSx = (char *) realloc(nastroSx, lenSx + 2048);
    strcat(nastroSx, blankStr);
    lenSx += 2047;
}

void pulisciStrutture(){
    pilaTransizioni * temp;
    // pulisci nastro (dx e sx)
    free(nastroSx);
    nastroSx = NULL;
    free(nastroDx);
    nastroDx = NULL;

    //pulisci coda
    while (testaPilaTransizioni != NULL){
        if (testaPilaTransizioni->nastroDx != NULL){
            if (testaPilaTransizioni->succ == NULL ) free(nastroDx);
            else if (testaPilaTransizioni->succ->nastroDx != testaPilaTransizioni->nastroDx) free(nastroDx);
        }
        if (testaPilaTransizioni->nastroSx != NULL){
            if (testaPilaTransizioni->succ == NULL ) free(nastroSx);
            else if (testaPilaTransizioni->succ->nastroSx != testaPilaTransizioni->nastroSx) free(nastroSx);
        }
        temp = testaPilaTransizioni;
        testaPilaTransizioni = testaPilaTransizioni->succ;
        free(temp);
    }
    testaPilaTransizioni = NULL;

    mosseCorrenti = 0;
    stato = 0;

}

int diAccettazione(transizione * tr){
    int i;
    for (i = 0; i < NUM_STATI_ACCETTAZIONE && statiAcc[i] != 0; i++){
        if (tr->statoArrivo == statiAcc[i]) return 1;
    }
    return 0;
}