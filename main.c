#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1000
#define FALSE 0
#define TRUE 1

typedef struct cellStruct Cell; /* Questa struttura rappresenta una cella dell'albero contenente tutti i percorsi da una variabile ad un altra */

struct cellStruct {
    char isRoot;
    Cell** next;
    int value;
    /* number of variables */
    int n;
    char visited;
};

/* Questo puntatore a puntatori, è in realtà un array di Cell* e conterrà l'elenco di tutte le celle per relative ad ogni variabile. */
Cell **allCells;

/* Crea una nuova cella dell'albero
 * variableCount: il numero di variabili delle disequazioni. Identifica anche il numero massimo di figli di ogni cella.
 * value: il valore della cella corrente */
Cell* newCell(int variableCount, int value) {
    int i;
    Cell* cell = (Cell*)malloc(sizeof(Cell));
    cell->isRoot = FALSE;
    cell->value = value;
    cell->next = malloc(variableCount * sizeof(Cell*));
    for (i = 0; i < variableCount; i++) {
        cell->next[i] = NULL;
    }
    cell->n = variableCount;
    cell->visited = FALSE;
    return cell;
}

/* Crea una cella radice per l'albero. Si tratta di un entrypoint */
Cell* newRootCell(int variableCount) {
    int i;
    Cell* root = newCell(variableCount, 0);
    root->isRoot = TRUE;
    allCells = malloc(variableCount * sizeof(Cell*));
    for (i = 0; i < variableCount; i++) {
        allCells[i] = NULL;
    }
    return root;
}

/* Si tratta di una funzione ricorsiva che aggiunge una nuova diramazione che parte da `from` e va a `to`
 * Ad es. Da X1 a X4
 */
void addNextCell(Cell* cell, int from, int to) {
    if (cell->isRoot) {
        Cell* fromCell = allCells[from];
        if (fromCell == NULL) {
            fromCell = newCell(cell->n, from);
            allCells[from] = fromCell;
            cell->next[from] = fromCell;
        }
        if (allCells[to] == NULL) {
            allCells[to] = newCell(cell->n, to);
        }
        addNextCell(fromCell, from, to);
    } else {
        if (cell->next[to] == NULL) {
            cell->next[to] = allCells[to];
        }
    }
}

/* Utilizzata internamente. Cerca nell'albero una cella con valore `value` a partire dalla cella puntata da `*cell`.
 * cell: la cella da cui iniziare la ricerca
 * value: il valore da cercare
 * currentDepth: numero di ricorsioni effettuate. Il numero di ricorsioni può essere al massimo pari a `numOfVariables`
 * numOfVariables: il numero di variabili delle disequazioni
 */
char _find(Cell *cell, int value, int numOfVariables) {
    int i;

    if (cell->visited) {
        return FALSE;
    }

    cell->visited = TRUE;

    if (cell->value == value) {
        return TRUE;
    }
    for (i = 0; i < numOfVariables; i++) {
        if (cell->next[i] != NULL) {
            if (_find(cell->next[i], value, numOfVariables)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* Verifica se, a partire dalla cella specificata, è possibile raggiungere una cella con valuew `value` */
char find (Cell *cell, int value, int numOfVariables) {
    int i;
    Cell *xcell;
    /* reset visited flag */
    for (i = 0; i < numOfVariables; i++) {
        xcell = allCells[i];
        if (xcell != NULL) {
            xcell -> visited = FALSE;
        }
    }

    return _find(cell, value, numOfVariables);
}

/* Restituisce `TRUE` se X(x) = a X(y).
 * X(x) risulta uguale a X(y) se a partire da X(x) è possibile arrivare fino a X(y) e a partire da X(y) è possibile arrivare fino a X(x).
 */
char areEqual(int x, int y, int numOfVariables) {
    Cell *xCell, *yCell;

    if (x == y) {
        return TRUE;
    }
    xCell = allCells[x];
    yCell = allCells[y];
    if (xCell == NULL || yCell == NULL) {
        return FALSE;
    }
    return find(xCell, y, numOfVariables) && find(yCell, x, numOfVariables);
}

/* Libera la memoria occupata dall'albero */
void freeMem(Cell* root, int variableCount) {
    int i;
    for (i = 0; i < variableCount; i++) {
        free(allCells[i]->next);
        free(allCells[i]);
    }

    free(allCells);
    free(root->next);
    free(root);
}

int main(int argc, char *argv[]) {
    FILE    *textfile;
    char    line[MAX_LINE_LENGTH];
    int i, n, m;
    int x,y;
    Cell *root;

    if (argc != 2) {
        printf("Per favore, specificare il file da leggere\n");
        return 1;
    }

    textfile = fopen(argv[1], "r");
    if(textfile == NULL)
        return 1;

    /* read n and m */
    fgets(line, MAX_LINE_LENGTH, textfile);

    sscanf(line, "%d %d", &n, &m);

    /* skip empty line */
    fgets(line, MAX_LINE_LENGTH, textfile);

    root = newRootCell(n);
    for (i = 0; i < m; i++) {
        fgets(line, MAX_LINE_LENGTH, textfile);

        sscanf(line, "%d %d", &x, &y);
        addNextCell(root, x, y);
    }
    /* skip empty line */
    fgets(line, MAX_LINE_LENGTH, textfile);

    while(fgets(line, MAX_LINE_LENGTH, textfile)) {
        sscanf(line, "%d %d", &x, &y);
        printf("%s\n", areEqual(x, y, n) ? "true" : "false");
    }

    fclose(textfile);

    freeMem(root, n);
    return 0;
}
