/**
 * \file abin.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N int
#define F double

typedef struct  s_noeud {
    N op;
    F val;
    bool unaire;
    struct s_noeud *fg, *fd;
} noeud, *abin;

abin enraciner(N op, F val, abin g, abin d);
abin sag(abin a);
abin sad(abin a);
N racine(abin a);
bool vide(abin a);
abin copier_arbre(abin a);
void liberer_arbre(abin a);
