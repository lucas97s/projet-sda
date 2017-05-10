/**
 * \file pc.c
 * \Author Lucas SCHOTT
 */


#define TAILLE_MAX_EXPRESSION 4096
#define TAILLE_MAX_MOT 256


#include "abin.h"
#include <string.h>
#include <math.h>
#include <float.h>



//OPERATEURS//////////////////////////////////////
//////////////////////////////////////////////////



char * op_tab[16]={"\0","+","-","*","/","min","max","sqrt","exp","ln","abs","floor","ceil","=","(",")"};

enum {UNITE,PLUS,MOINS,FOIS,DIVISE,MIN,MAX,SQRT,EXP,LN,ABS,FLOOR,CEIL,EGALE,PAG,PAD,NOTOP};

/**
 * \fn bool est_un_operateur(int op)
 * \a id de l'operateur
 * \return true si c'est un operateur
 */
bool est_un_operateur(int op)
{
    return (op>UNITE && op<=PAD);
}

/**
 * \fn bool est_binaire(int op)
 * \a id de l'operateur
 * \return tue si l'operateur est binaire
 */
bool est_binaire(int op)
{
    return (op>=UNITE && op<=MAX);
}

//résultat d'evaluation
typedef struct {
    double nombre;
    bool valide;
} resultat;

/**
 * \fn int op_to_id(char* op)
 * \brief donne l'id de l'operateur en fonction de la chaine de caractères
 * représentant l'opérateur
 * \a op la chaine de caractere représentant l'opérateur
 * \return l'identifiant de l'operateur:
 * NOTOP si l'operateur n'est pas reconnu
 */
int op_to_id(char* op)
{
    int i;
    for(i=0;i<NOTOP;i++)
    {
        if(strcmp(op,op_tab[i])==0)
            return i;
    }
    return NOTOP;
}

/**
 * \fn double elt_neutre(int id_op)
 * \brief donne l'element neutre de l'operateur binaire
 * en argument
 * \a id_op l'id de l'operateur
 * \return element neutre
 */
double elt_neutre(int id_op)
{
    if(id_op==PLUS || id_op==MOINS)
        return 0;
    if(id_op==FOIS || id_op==DIVISE)
        return 1;
    if(id_op==MIN)
        return DBL_MAX;
    if(id_op==MAX)
        return DBL_MIN;
    else return 0;
}



//ANALYSE DE LA CHAINE DE CARACTERE///////////////
//////////////////////////////////////////////////


char * token=NULL;
const char * delim = " \n";

//gestion des variables 

typedef struct {
    double valeur;
    bool utilise;
} variable;

variable var_tab[26];

/**
 * \fn void ini_var_tab(void)
 * \brief initialisation de variables
 */
void init_var_tab(void)
{
    int i;
    for(i=0;i<26;i++)
    {
        var_tab[i].utilise = false;
    }
}

/**
 * \fn bool est_une_variable(char var)
 * \a var caractere representant la variable
 * \return true si var est une variable
 */
//une variable est un caractere alphabetique minuscule
bool est_une_variable(char var)
{
    return (var>=97 && var<=122);
}



//FONCTION DE LECTURE DE CHAINE ET////////////////
//CREATION D'ARBRE BINAIRE////////////////////////



abin lecture_expression();
abin lecture_parenthese();
resultat calculer_resultat(abin);

/**
 * \fn abin affectation(void)
 * \brief fonction qui affecte une sous expression à une variable
 * \return arbre binaire de la sous expression à stocker dans la variable
 * \post lecture de la chaine de caractere de l'expression
 * avancement dans la chaine token
 */
abin affectation(void)
{
    char mot[TAILLE_MAX_MOT];
    char var;
    int var_no;
    abin sous_exp;
    if( token != NULL )
    {
        snprintf(mot, TAILLE_MAX_MOT, "%s", token );
        token = strtok(NULL, delim);
        //lecture variable
        if(strlen(mot)==1)
        {
            var=mot[0];
            if(est_une_variable(var))
            {
                var_no=var-97;
                sous_exp = lecture_expression();
                resultat r = calculer_resultat(sous_exp);
                liberer_arbre(sous_exp);
                if(r.valide==false)
                    return enraciner(NOTOP,0,NULL,NULL);
                else
                {
                    var_tab[var_no].valeur = r.nombre;
                    var_tab[var_no].utilise=true;
                    return enraciner(UNITE,r.nombre,NULL,NULL);
                }
            }
            else
            {
                fprintf(stderr,"\"%c\" n'est pas une variable\n", var);
                return enraciner(NOTOP,0,NULL,NULL);
            }
        }
        else
        {
            fprintf(stderr,"\"%s\" n'est pas une variable\n", mot);
            return enraciner(NOTOP,0,NULL,NULL);
        }
    }
    fprintf(stderr,"operateur \"=\" necessite deux opérandes\n");
    return enraciner(NOTOP,0,NULL,NULL);
}

/**
 * \fn abin aux_lecture_parenthese(int id_op)
 * \brief fonction auxiliaire de lecture_parenthese
 * qui permet d'enraciner le meme operateur avec les opérandes
 * juqu'à la parenthese fermante
 * \return arbre binaire de l'expression entre parenthese
 * \post avancement dans token
 */
abin aux_lecture_parenthese(int id_op)
{
    abin n = enraciner(id_op,0,NULL,NULL);
    if(est_un_operateur(id_op))
    {
        n->fd = lecture_expression();
    }
    if(n->fd->op==PAD)
    {
        free(n->fd);
        n->op = UNITE;
        n->val = elt_neutre(id_op); 
        n->fd = NULL;
    }
    else
    {
        n->fg = aux_lecture_parenthese(id_op);
    }
    return n;
}

/**
 * \fn abin lecture_parenthese(void)
 * \brief creation de l'arbre de l'expression en prenant en compte
 * l'augmentation de l'aritée de l'opérateur en début de parenthese
 * \return l'arbre binaire de l'expression entre parenthese
 * \post avancement dans la chaine token
 */
abin lecture_parenthese(void)
{
    double nombre;
    int count;
    char mot[TAILLE_MAX_MOT];
    char mot2[TAILLE_MAX_MOT];
    char op[TAILLE_MAX_MOT];
    int id_op;
    int id_op2;
    char var;
    int var_no;
    if( token != NULL ) 
    {
        snprintf(mot, TAILLE_MAX_MOT, "%s", token );
        token = strtok(NULL, delim);
        //lecture d'un nombre
        if((count=sscanf(mot,"%lf", &nombre))!=EOF && count>0)
        {
            if(token != NULL)
            {
                snprintf(mot2, TAILLE_MAX_MOT, "%s", token );
                token = strtok(NULL, delim);
                id_op2 = op_to_id(mot2);
                if(id_op2==PAD)
                    return enraciner(UNITE,nombre,NULL,NULL);
                else
                {
                    fprintf(stderr,"\"%s\" inatendu\n", token);
                    return enraciner(NOTOP,0,NULL,NULL);
                }
            }
        }
        //lecture d'un operateur
        else if((count=sscanf(mot,"%s", op))!=EOF && count>0)
        {
            id_op = op_to_id(op);
            if(est_un_operateur(id_op))
            {
                if(id_op==PAD)
                {
                    fprintf(stderr,"parenthese vide\n");
                    return enraciner(PAD,0,NULL,NULL);
                }
                if(!est_binaire(id_op))
                {
                    fprintf(stderr,"%s inatendu\n", op);
                    return enraciner(NOTOP,0,NULL,NULL);
                }
                return aux_lecture_parenthese(id_op);
            }
            //lecture d'une variable
            else if(strlen(op)==1)
            {
                var=op[0];
                if(est_une_variable(var))
                {
                    var_no = var-97;
                    if(var_tab[var_no].utilise==true)
                    {
                        return enraciner(UNITE,var_tab[var_no].valeur,NULL,NULL);
                    }
                    fprintf(stderr,"variable %c non initialisée", var);
                    return enraciner(NOTOP,0,NULL,NULL);
                }
                else
                {
                    fprintf(stderr,"%c non reconnu\n", var);
                    return enraciner(NOTOP,0,NULL,NULL);
                }
            }
            else
            {
                fprintf(stderr,"operateur %s inconnu\n", op);
                return enraciner(NOTOP,0,NULL,NULL);
            }
        }
        else
        {
            return enraciner(NOTOP,0,NULL,NULL);
        }
    }
    fprintf(stderr,"manque une operande\n");
    return NULL;
}

/**
 * \fn abin lecture_expression(void)
 * \brief creation de l'arbre binaire représentant l'expression
 * issu de la chaine de caractere lu sur l'entree standard
 * \return arbre binaire 
 * \post lecture de la chaine, avancement dans la chaine token
 */
abin lecture_expression(void)
{
    abin n;
    double nombre;
    char var;
    int var_no;
    int count;
    char mot[TAILLE_MAX_MOT];
    char op[TAILLE_MAX_MOT];
    int id_op;
    if( token != NULL ) 
    {
        snprintf(mot, TAILLE_MAX_MOT, "%s", token );
        token = strtok(NULL, delim);
        //lecture d'un nombre
        if((count=sscanf(mot,"%lf", &nombre))!=EOF && count>0)
        {
            return enraciner(UNITE,nombre,NULL,NULL);
        }
        //lecture d'un operateur
        else if((count=sscanf(mot,"%s", op))!=EOF && count>0)
        {
            id_op = op_to_id(op);
            if(est_un_operateur(id_op))
            {
                if(id_op==PAD)
                    return enraciner(PAD,0,NULL,NULL);
                if(id_op==PAG)
                {
                    return lecture_parenthese();
                }
                if(id_op==EGALE)
                {
                    return affectation();
                }
                n = enraciner(id_op,0,NULL,NULL);
                if(est_un_operateur(id_op))
                    n->fg = lecture_expression();
                if(est_binaire(id_op))
                    n->fd = lecture_expression();
                return n;
            }
            //lecture d'une variable
            else if(strlen(op)==1)
            {
                var=op[0];
                if(est_une_variable(var))
                {
                    var_no = var-97;
                    if(var_tab[var_no].utilise==true)
                    {
                        return enraciner(UNITE,var_tab[var_no].valeur,NULL,NULL);
                    }
                    fprintf(stderr,"variable %c non initialisée\n", var);
                    return enraciner(NOTOP,0,NULL,NULL);
                }
                fprintf(stderr,"%c non reconnu\n", var);
                return enraciner(NOTOP,0,NULL,NULL);
            }
            else
            {
                fprintf(stderr,"operateur %s inconnu\n", op);
                return enraciner(NOTOP,0,NULL,NULL);
            }
        }
        else
        {
            return enraciner(NOTOP,0,NULL,NULL);
        }
    }
    fprintf(stderr,"manque une operande\n");
    return NULL;
}



//FONCTION D'EVALUATION D'ARBRE BINAIRE///////////
//ET CALCUL DU RESULTAT///////////////////////////



/**
 * \fn resultat calculer_resultat(abin expr)
 * \brief fonction qui évalue le résultat de l'arbre binaire
 * representant l'expression
 * \a expr l'arbre binaire a evaluer
 * \return struct resultat avec la valeur du ressultat et
 * la validite du resultat
 */
resultat calculer_resultat(abin expr)
{
    resultat r;
    resultat a;
    resultat b;
    r.nombre=0;
    r.valide=true;
    if(expr==NULL)
    {
        r.valide=false;
        return r;
    }
    switch(expr->op)
    {
        case UNITE: //nombre
            r.nombre = expr->val;
            return r;
            break;
        case PLUS: //addition
            a = calculer_resultat(expr->fg);
            b = calculer_resultat(expr->fd);
            r.nombre =  a.nombre + b.nombre;
            r.valide = a.valide && b.valide;
            return r;
            break;
        case MOINS: //soustraction
            a = calculer_resultat(expr->fg);
            b = calculer_resultat(expr->fd);
            r.nombre =  a.nombre - b.nombre;
            r.valide = a.valide && b.valide;
            return r;
            break;
        case FOIS: //multiplication
            a = calculer_resultat(expr->fg);
            b = calculer_resultat(expr->fd);
            r.nombre =  a.nombre * b.nombre;
            r.valide = a.valide && b.valide;
            return r;
            break;
        case DIVISE: //division
            a = calculer_resultat(expr->fg);
            b = calculer_resultat(expr->fd);
            r.nombre =  a.nombre / b.nombre;
            r.valide = a.valide && b.valide;
            return r;
            break;
        case MIN: //minimum
            a = calculer_resultat(expr->fg);
            b = calculer_resultat(expr->fd);
            r.nombre = fmin(a.nombre,b.nombre);
            r.valide = a.valide && b.valide;
            return r;
            break;
        case MAX: //maximum
            a = calculer_resultat(expr->fg);
            b = calculer_resultat(expr->fd);
            r.nombre = fmax(a.nombre,b.nombre);
            r.valide = a.valide && b.valide;
            return r;
            break;
        case SQRT: //racine carre
            a=calculer_resultat(expr->fg);
            r.nombre = sqrt(a.nombre);
            r.valide = a.valide;
            return r;
            break;
        case EXP: //exponentiel
            a=calculer_resultat(expr->fg);
            r.nombre = exp(a.nombre);
            r.valide = a.valide;
            return r;
            break;
        case LN: //logarithme neperien
            a=calculer_resultat(expr->fg);
            r.nombre = log(a.nombre);
            r.valide = a.valide;
            return r;
            break;
        case ABS: //valeur absolue
            a=calculer_resultat(expr->fg);
            r.nombre = abs(a.nombre);
            r.valide = a.valide;
            return r;
            break;
        case FLOOR: //plancher
            a=calculer_resultat(expr->fg);
            r.nombre = floor(a.nombre);
            r.valide = a.valide;
            return r;
            break;
        case CEIL: //plafond
            a=calculer_resultat(expr->fg);
            r.nombre = ceil(a.nombre);
            r.valide = a.valide;
            return r;
            break;
        case NOTOP: //operateur inconnue
            r.valide=false;
            return r;
            break;
        default: //operateur inconnue
            r.valide=false;
            return r;
            break;
    }
    r.valide=false;
    return r;
}



//FONCTION MAIN///////////////////////////////////
//////////////////////////////////////////////////



/**
 * \fn int main(void)
 * \brief fonction main, initialisation des variables, lecture
 * de l'entree std, creation de l'arbre binaire, evaluation du resultat.
 */
int main(void)
{
    abin expression;
    resultat r;
    char * expr=(char*) calloc(TAILLE_MAX_EXPRESSION,1);
    init_var_tab();
    while(true)
    {
        //lecture de l'entree std
        if(fgets(expr, TAILLE_MAX_EXPRESSION, stdin)==NULL)
        {
            free(expr);
            exit(EXIT_SUCCESS);
        }
        token = strtok(expr, delim);
        
        //si l'expression est un commentaire, passer à l'expression suivante
        if(token==NULL || token[0]=='#')
            continue;
        
        //creation de l'arbre binaire representant l'expression
        expression = lecture_expression();
        
        if(token!=NULL && token[0]!='#')
        {
            fprintf(stderr,"\"%s\" inatendu\n", token);
            fprintf(stderr,"ERROR\n");
        }
        else
        {
            //calcul du resultat
            r = calculer_resultat(expression);
            if(r.valide)
                printf("%.4lf\n", r.nombre);
            else
                printf("ERROR\n");
        }
        liberer_arbre(expression);
    }
    return 1;
}
