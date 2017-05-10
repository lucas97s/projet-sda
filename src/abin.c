/**
 * \file abin.c
 */

#include "abin.h"

/*
#define N int
#define F double

typedef struct  s_noeud {
    N op;
    F val;
    bool unaire;
    struct s_noeud *fg, *fd;
} noeud, *abin;
*/

/**
 * \fn abin enraciner(T x, abin g, abin d)
 * \brief enraciner deux arbres
 * \return la racine
 */
abin enraciner(N op, F val, abin g, abin d)
{
    abin n = (abin) malloc(sizeof(noeud));
    n->val=val;
    n->op=op;
    n->fg=g;
    n->fd=d;
    n->unaire=false;
    return n;
}

/**
 * \fn abin sag(abin a)
 * \return sous arbre gauche
 */
abin sag(abin a)
{
    return a->fg;
}

/**
 * \fn abin sad(abin a)
 * \return sous arbre droit
 */
abin sad(abin a)
{
    return a->fd;
}

/**
 * \fn T racine(abin a)
 * \return val de la racine
 */
N racine(abin a)
{
    return a->op;
}

/**
 * \fn bool vide(abin a)
 * \return true s'il est vide, false sinon
 */
bool vide(abin a)
{
    return a==NULL;
}

/**
 * \fn abin copier_arbre(abin a)
 * \return un nouvel arbre
 */
abin copier_arbre(abin a)
{
    if(vide(a))
        return NULL;
    else
        return enraciner(a->op,a->val,copier_arbre(a->fg),copier_arbre(a->fd));
}

/**
 * \fn void liberer_arbre(abin a)
 * \brief libere l'arbre
 */
void liberer_arbre(abin a)
{
    if(a->fg!=NULL)
        liberer_arbre(a->fg);
    if(a->fd!=NULL)
        liberer_arbre(a->fd);
    free(a);
    return;
}
