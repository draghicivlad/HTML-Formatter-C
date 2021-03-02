/* DRAGHICI Vlad Matei - 312CB */
#include "utils.h"
#include <ctype.h>
#include <stdlib.h>

// Functii stiva si coada
void* InitS(size_t d)
{
    ASt s;
    s = (ASt)malloc(sizeof(TStiva));
    if(!s)
        return NULL;

    s->dime = d;
    s->vf = NULL;
    return (void*)s;
}

int Push(void* a, void* ae)
{
    TList aux = (TList)malloc(sizeof(TCel));
    if(!aux)
        return 0;
    
    aux->info = (void *)malloc(((TStiva*)a)->dime);
    if(!aux->info)
    {
        free(aux);
        return 0;
    }
    memcpy(aux->info, ae, ((TStiva*)a)->dime);
    aux->urm = ((TStiva*)a)->vf;
    ((TStiva*)a)->vf = aux;
    return 1;
}

int Pop(void* s, void* ae)
{
    if(((TStiva*)s)->vf == NULL)
        return 0;
    
    memcpy(ae, ((TStiva*)s)->vf->info, ((TStiva*)s)->dime);
    TList aux = ((TStiva*)s)->vf;
    ((TStiva*)s)->vf = aux->urm;

    free((((TList)aux)->info));
    free(aux);
    return 1;
}

void DistrS(void** a)
{
    TList t = ((TStiva*)(*a))->vf, aux;

    while(t != NULL)
    {
        aux = t;
        t = t->urm;
        free(aux->info);
        free(aux);
    }
    free(*a);
}

void* InitQ(size_t d)
{
    AQ c;
    c = (AQ)malloc(sizeof(TCoada));
    if(!c)
        return NULL;

    c->dime = d;
    c->ic = NULL;
    c->sc = NULL;
    return (void*)c;
}

int IntrQ(void* a, void* ae)
{
    TList aux = (TList)malloc(sizeof(TCel));
    if(!aux)
        return 0;
    
    aux->info = (void *)malloc(((TCoada*)a)->dime);
    if(!aux->info)
    {
        free(aux);
        return 0;
    }
    memcpy(aux->info, ae, ((TCoada*)a)->dime);
    if(((TCoada*)a)->ic == NULL && ((TCoada*)a)->sc == NULL)
    {
        aux->urm = NULL;
        ((TCoada*)a)->ic = ((TCoada*)a)->sc = aux;
    }
    else
    {
        aux->urm = NULL;
        ((TCoada*)a)->sc->urm = aux;
        ((TCoada*)a)->sc = aux;
    }
    return 1;
}

int ExtrQ(void* a, void* ae)
{
    if((((TCoada*)a)->ic == NULL) && (((TCoada*)a)->sc == NULL))
        return 0;
    
    TList aux;

    if(((TCoada*)a)->ic != ((TCoada*)a)->sc)
    {
        aux = ((TCoada*)a)->ic;
        ((TCoada*)a)->ic = aux->urm;
        memcpy(ae, aux->info, ((TCoada*)a)->dime);
        free(aux->info);
        free(aux);
    }
    else
    {
        aux = ((TCoada*)a)->ic;
        ((TCoada*)a)->ic = ((TCoada*)a)->sc = NULL;
        memcpy(ae, aux->info, ((TCoada*)a)->dime);
        free(aux->info);
        free(aux);
    }
    return 1;
}

void DistrQ(void **a)
{
    TList cel = ((TCoada*)(*a))->ic, aux;
    ((TCoada*)(*a))->ic = ((TCoada*)(*a))->sc = NULL;
    while(cel)
    {
        aux = cel;
        cel = cel->urm;
        free(aux->info);
        free(aux);
    }
    free(((TCoada*)(*a)));
    *a = NULL;
}

// Functii HTML
TArb CitireHTML(FILE *html_file)
{
    TArb arb = NULL, root = NULL;
    TParseState stare = PARSE_CONTENTS;
    void *stiva_arb = InitS(sizeof(TArb));
    int deschis = 0;
    int next = 0;
    TAttr attr;

    char *line = calloc(10240, sizeof(char));
    if(!line)
        return NULL;

    while(fgets(line, 10240, html_file))
    {
        unsigned int i;
        if(line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        for(i = 0; i < strlen(line); i++)
        {
            stare = Interpret(stare, line[i], &arb, &deschis, stiva_arb, &root,
             &next, &attr);
        }
    }
    free(line);
    DistrS(&stiva_arb);
    return root;
}

// Creare type pentru nodul curent
TArb CreateTagNode(char c)
{
    TArb arb = CreateNode();
    arb->info->type = calloc(N, sizeof(char));
    if(!arb->info->type)
        return NULL;
    
    strncpy(arb->info->type, &c, 1);
    return arb;
}

// Creare nod si info pentru acesta
TArb CreateNode()
{
    TArb arb;
    arb = calloc(1, sizeof(TNodArb));
    if(!arb)
        return NULL;
    
    arb->info = calloc(1, sizeof(TNodInfo));
    if(!arb->info)
    {
        free(arb);
        return NULL;
    }
    arb->info->contents = calloc(N, sizeof(char));
    if(!arb->info->contents)
    {
        free(arb->info);
        free(arb);
        return NULL;
    }
    arb->info->id = calloc(N, sizeof(char));
    if(!arb->info->id)
    {
        free(arb->info->contents);
        free(arb->info);
        free(arb);
        return NULL;
    }
    return arb;
}

// Formatare valoarea stilului 
void StyleValueFormat(TAttr *attr)
{
    unsigned int i;
    char *str = calloc(N, sizeof(char));
    char *aux = calloc(N, sizeof(char));
    if(!str || !aux)
        return;
    strcpy(str, (*attr)->value);
    free((*attr)->value);

    for(i = 0; i < strlen(str); i++)
    {
        if(str[i] == ':')
        {
            strncat(aux, ": ", 2);
        }
        else if(str[i] == ';')
        {
            strncat(aux, "; ", 2);
        }
        else if(str[i] != ' ')
        {
            strncat(aux, &str[i], 1);
        }
    }
    aux[strlen(aux) - 1] = '\0';
    
    (*attr)->value = aux;
    free(str);
}

// Formatarea si afisarea codul HTML
void PrintHTML(TArb arb, int nivel, FILE *out_file)
{
    TAttr aux;
    int i;
    if(arb == NULL)
        return;

    for(i = 0; i < nivel; i++)
        fprintf(out_file, "\t");
    fprintf(out_file, "<%s", arb->info->type);

    if(arb->info->style)
        fprintf(out_file, " %s=\"%s\"", arb->info->style->name,
         arb->info->style->value);

    for(aux = arb->info->otherAttributes; aux != NULL; aux = aux->next)
    {
        fprintf(out_file, " %s=\"%s\"", aux->name, aux->value);
    }

    if(arb->info->isSelfClosing == 1)
    {
        fprintf(out_file, "/>\n");
    }
    else
    {
        fprintf(out_file, ">\n");
        
        if(strcmp(arb->info->contents, ""))
        {
            for(i = 0; i <= nivel; i++)
                fprintf(out_file, "\t");
            fprintf(out_file, "%s\n", arb->info->contents);
        }
        // Trece la primul copil    
        if(arb->firstChild != NULL)
            PrintHTML(arb->firstChild, nivel + 1, out_file);

        for(i = 0; i < nivel; i++)
            fprintf(out_file, "\t");
        fprintf(out_file, "</%s>\n", arb->info->type);
    }
    // Trece la frate
    PrintHTML(arb->nextSibling, nivel, out_file);
}

// Distrugere in adancime nod si succesorii acestuia (recursiv)
void DistNod(TArb *arb)
{
    TAttr p, aux;
    if((*arb)->firstChild != NULL)
        DistNod(&(*arb)->firstChild);
    if((*arb)->nextSibling != NULL)
        DistNod(&(*arb)->nextSibling);
    free((*arb)->info->type);
    free((*arb)->info->id);
    if((*arb)->info->style != NULL)
    {
        free((*arb)->info->style->name);
        free((*arb)->info->style->value);
        free((*arb)->info->style);
    }
    if((*arb)->info->otherAttributes != NULL)
        for(aux = (*arb)->info->otherAttributes; aux != NULL;)
        {
            free(aux->name);
            free(aux->value);
            p = aux;
            aux = aux->next;
            free(p);
        }
    if((*arb)->info->contents != NULL)
        free((*arb)->info->contents);
    free((*arb)->info);
    free(*arb);
}

// Adauga nod dupa selector
void AddNod(TArb arb, char *id, char *tag, FILE *out_file)
{
    TArb aux;
    unsigned int i;
    int deschis, next, caz;
    void *stiva_arb;
    char *id_aux = strdup(id);
    TArb nodCautat = CautaNodDupaID(arb, id, &aux, &caz);
    if(nodCautat == NULL)
    {
        fprintf(out_file, "Add tag failed: node with id %s not found!\n", id_aux);
        free(id_aux);
        return;
    }
    // Aplic Interpret-ul pentru tag-ul dat ca parametru
    TAttr attr;
    stiva_arb = InitS(sizeof(TArb));
    if(nodCautat->firstChild == NULL)
    {
        deschis = 1;
        next = 0;
    }
    else
    {
        deschis = 0;
        next = 1;
        nodCautat = nodCautat->firstChild;
        for(;nodCautat->nextSibling != NULL; nodCautat = nodCautat->nextSibling);
    }
    TParseState stare = PARSE_CONTENTS;
    for(i = 0; i < strlen(tag); i++)
    {
        stare = Interpret(stare, tag[i], &nodCautat, &deschis, stiva_arb, &nodCautat,
         &next, &attr);
    }
    DistrS(&stiva_arb);
    free(id_aux);
}

// Caut nodul dupa id in mod eficient
TArb CautaNodDupaID(TArb arb, char *id, TArb *fam, int *caz)
{
    char *aux = strtok(id, ".");
    int x, i;
    while(aux != NULL)
    {
        x = atoi(aux);
        if(arb->firstChild != NULL)
        {
            (*fam) = arb;
            arb = arb->firstChild;
            (*caz) = 0;
        }
        else
            return NULL;
        for(i = 1; i < x; i++)
            if(arb->nextSibling != NULL)
            {
                (*fam) = arb;
                arb = arb->nextSibling;
                (*caz) = 1;
            }
            else
                return NULL;
        aux = strtok(NULL, ".");
    }
    return arb;
}

// Functia de add apelata din main
void Add(TArb arb, FILE *in_file, FILE *out_file)
{
    char *id, *tag;
    char *aux, *line_aux;
    char *line = calloc(N, sizeof(char));
    fgets(line, N, in_file);
    if(line[strlen(line) - 2] == 13)
        line[strlen(line) - 2] = '\0';

    line_aux = strdup(line);

    aux = strtok(line_aux, "= ");
    aux = strtok(NULL, "= ");
    id = strdup(aux);

    free(line_aux);
    line_aux = strdup(line);
    aux = strtok(line_aux, "\"");
    aux = strtok(NULL, "\"");
    tag = strdup(aux);

    AddNod(arb, id, tag, out_file);

    free(id);
    free(tag);
    free(line);
    free(line_aux);
}

// Functia de DeleteRecursively apelata din main care prelucreaza si input-ul
// pentru a-si da seama de tipul selectorului
void DeleteRecursively(TArb arb, FILE *in_file, FILE *out_file)
{
    unsigned int i;
    char *line = calloc(N, sizeof(char));
    char *aux, *p, *el_de_caut_1 = calloc(N, sizeof(char));
    int caz;

    fgets(line, N, in_file);
    if(line[strlen(line) - 2] == 13)
        line[strlen(line) - 2] = '\0';
    
    aux = strtok(line, "\"");
    aux = strtok(NULL, "\"");
    p = strdup(aux);

    if(p[0] == '#')
    {
        strcpy(el_de_caut_1, p + 1);
        DeleteRecursivelyTag(arb, el_de_caut_1, out_file);
    }
    else if(p[0] == '.')
    {
        strcpy(el_de_caut_1, p + 1);
        if(DeleteRecursivelySelector(arb, el_de_caut_1, cmpClass) == 1)
        {
            fprintf(out_file, "Delete recursively failed: no node found for");
            fprintf(out_file, " selector .%s!\n", el_de_caut_1);
        }
    }
    else
    {
        pereche_char *per = calloc(1, sizeof(pereche_char));
        per->el_1 = calloc(N, sizeof(char));
        per->el_2 = calloc(N, sizeof(char));
        caz = 0;
        for(i = 1; i < strlen(p); i++)
        {
            if(isalpha(p[i]) == 0)
            {
                if(p[i] == '.')
                    caz = 1;
                if(p[i] == '>')
                    caz = 2;
            }
        }
        aux = strtok(p, " .>");
        strcpy(el_de_caut_1, aux);
        aux = strtok(NULL, " .>");
        if(aux != NULL)
        {
            strcpy(per->el_1, el_de_caut_1);
            strcpy(per->el_2, aux);
            if(caz == 0)
            {
                if(DeleteRecursivelySelector(arb, per, cmpTag_Stramos) == 1)
                {
                    fprintf(out_file, "Delete recursively failed: no node found");
                    fprintf(out_file, " for selector %s %s!\n", per->el_1, per->el_2);
                }
            }
            if(caz == 1)
            {
                if(DeleteRecursivelySelector(arb, per, cmpTag_Class) == 1)
                {
                    fprintf(out_file, "Delete recursively failed: no node found");
                    fprintf(out_file, " for selector %s.%s!\n", per->el_1, per->el_2);
                }
            }
            if(caz == 2)
            {
                if(DeleteRecursivelySelector(arb, per, cmpTag_Parent) == 1)
                {
                    fprintf(out_file, "Delete recursively failed: no node found");
                    fprintf(out_file, " for selector %s>%s!\n", per->el_1, per->el_2);
                }
            }
        }
        else
        {
            if(DeleteRecursivelySelector(arb, el_de_caut_1, cmpTag) == 1)
            {
                fprintf(out_file, "Delete recursively failed: no node found");
                fprintf(out_file, " for selector %s!\n", el_de_caut_1);
            }
        }
        free(per->el_1);
        free(per->el_2);
        free(per);
    }
    RemakeID(arb);

    free(line);
    free(p);
    free(el_de_caut_1);
}

// Cauta nodul dupa id si apoi il sterge recursiv
void DeleteRecursivelyTag(TArb arb, char *tag, FILE *out_file)
{
    int caz = 0;
    TArb arbCautat, fam;
    char *tag_aux = strdup(tag);

    arbCautat = CautaNodDupaID(arb, tag, &fam, &caz);
    if(arbCautat == NULL)
    {
        fprintf(out_file, "Delete recursively failed: no node found for");
        fprintf(out_file, " selector #%s!\n", tag_aux);
        free(tag_aux);
        return;
    }
    if(caz == 0)
    {
        fam->firstChild = arbCautat->nextSibling;
        arbCautat->nextSibling = NULL;
        DistNod(&arbCautat);
    }
    else
    {
        fam->nextSibling = arbCautat->nextSibling;
        arbCautat->nextSibling = NULL;
        DistNod(&arbCautat);
    }
    free(tag_aux);
}

//cauta nodul dupa selector si apoi il sterge recursiv
int DeleteRecursivelySelector(TArb arb, void *selector, CMP cmp)
{
    int nrNoduriGasite = 0, i;
    TinfoNod vecNoduriGasite = CautaNoduriDupaSelector(arb, arb, selector,
     &nrNoduriGasite, cmp);
    if(nrNoduriGasite == 0)
    {
        free(vecNoduriGasite);
        return 1;
    }
    for(i = nrNoduriGasite - 1; i >= 0; i--)
    {
        if(vecNoduriGasite[i].caz == 0)
        {
            vecNoduriGasite[i].familie->firstChild = vecNoduriGasite[i].
            nodCautat->nextSibling;
            vecNoduriGasite[i].nodCautat->nextSibling = NULL;
            DistNod(&vecNoduriGasite[i].nodCautat);
        }
        else
        {
            vecNoduriGasite[i].familie->nextSibling = vecNoduriGasite[i].
            nodCautat->nextSibling;
            vecNoduriGasite[i].nodCautat->nextSibling = NULL;
            DistNod(&vecNoduriGasite[i].nodCautat);
        }        
    }
    free(vecNoduriGasite);
    return 0;
}

// Functiile de comparare pentru selectori
int cmpClass(TinfoNod pr, void *clasa, TArb arb)
{
    TAttr aux;
    (void)arb;
    if(pr->nodCautat->info->otherAttributes != NULL)
        for(aux = pr->nodCautat->info->otherAttributes; aux != NULL;
         aux = aux->next)
            if(strcmp(aux->name, "class") == 0)
                if(strcmp(aux->value, (char *)clasa) == 0)
                    return 1;
    return 0;
}

int cmpTag(TinfoNod pr, void *tag, TArb arb)
{
    (void)arb;
    if(strcmp(pr->nodCautat->info->type, (char *)tag) == 0)
        return 1;
    return 0;
}

int cmpTag_Class(TinfoNod pr, void *per, TArb arb)
{
    TAttr aux;
    (void)arb;
    if(strcmp(pr->nodCautat->info->type, ((pereche_char *)per)->el_1) == 0)
        if(pr->nodCautat->info->otherAttributes != NULL)
            for(aux = pr->nodCautat->info->otherAttributes; aux != NULL;
             aux = aux->next)
                if(strcmp(aux->name, "class") == 0)
                    if(strcmp(aux->value, ((pereche_char *)per)->el_2) == 0)
                        return 1;
    return 0;
}

int cmpTag_Parent(TinfoNod pr, void *per, TArb arb)
{
    TArb parinte, bunic;
    int caz;
    char *id_par = strdup(pr->nodCautat->info->id);
    if(strlen(id_par) > 2)
        if(id_par[strlen(id_par) - 2] == '.')
            id_par[strlen(id_par) - 2] = '\0';
    parinte = CautaNodDupaID(arb, id_par, &bunic, &caz);

    if((strcmp(parinte->info->type, ((pereche_char *)per)->el_1) == 0) &&
     (strcmp(pr->nodCautat->info->type, ((pereche_char *)per)->el_2) == 0))
    {
        free(id_par);
        return 1;
    }
    free(id_par);
    return 0;
}

int cmpTag_Stramos(TinfoNod pr, void *per, TArb arb)
{
    int i = 0;
    TArb parinte, bunic;
    int caz;
    char *id_par = strdup(pr->nodCautat->info->id), *aux = calloc(N, sizeof(char));
    if(strcmp(pr->nodCautat->info->type, ((pereche_char *)per)->el_2) == 0)
    {
        while(strlen(id_par) >= 2)
        {
            while(id_par[strlen(id_par) - 1] != '.')
                id_par[strlen(id_par) - 1] = '\0';
            id_par[strlen(id_par) - 1] = '\0';
            i++;
            strcpy(aux, id_par);
            parinte = CautaNodDupaID(arb, id_par, &bunic, &caz);
            strcpy(id_par, aux);
            if(strcmp(parinte->info->type, ((pereche_char *)per)->el_1) == 0)
            {
                free(id_par);
                free(aux);
                return 1;
            }
        }
    }
    free(id_par);
    free(aux);
    return 0;
}

// Functia de cautare noduri dupa un selector
TinfoNod CautaNoduriDupaSelector(TArb arb, TArb root, void *selector,
 int *nrNoduriGasite, CMP cmp)
{
    AQ c = InitQ(sizeof(infoNod));
    TinfoNod pr = calloc(1, sizeof(infoNod));

    TinfoNod vecNoduriGasite = calloc(N, sizeof(infoNod));
    pr->caz = 0;
    pr->familie = arb;
    pr->nodCautat = root;
    IntrQ(c, pr);
    while(c->ic != NULL)
    {
        ExtrQ(c, pr);
        if(cmp(pr, selector, root) == 1)
        {
            vecNoduriGasite[*nrNoduriGasite].nodCautat = pr->nodCautat;
            vecNoduriGasite[*nrNoduriGasite].familie = pr->familie;
            vecNoduriGasite[*nrNoduriGasite].caz = pr->caz;
            (*nrNoduriGasite)++;
        }
        if(pr->nodCautat->firstChild != NULL)
        {
            pr->familie = pr->nodCautat;
            pr->nodCautat = pr->nodCautat->firstChild;
            pr->caz = 0;
            IntrQ(c, pr);
            while(pr->nodCautat->nextSibling != NULL)
            {
                pr->familie = pr->nodCautat;
                pr->nodCautat = pr->nodCautat->nextSibling;
                pr->caz = 1;
                IntrQ(c, pr);
            }
        }
    }
    free(pr);
    DistrQ((void **)(&c));
    return vecNoduriGasite;
}

// Refacere ID pentru arbore
void RemakeID(TArb arb)
{
    int nr_ordin;
    char nr_ordin_char[100];
    char id_parinte[100];
    AQ c = InitQ(sizeof(TArb));

    IntrQ(c, &arb);
    while(c->ic != NULL)
    {
        ExtrQ(c, &arb);

        if(arb->firstChild != NULL)
        {
            nr_ordin = 1;

            if(strcmp(arb->info->id, "") != 0)
            {
                strcpy(id_parinte, arb->info->id);
                strcpy(arb->firstChild->info->id, id_parinte);
                strcat(arb->firstChild->info->id, ".");
            }
            else
            {
                strcpy(id_parinte, "");
                strcpy(arb->firstChild->info->id, id_parinte);
            }
            strcat(arb->firstChild->info->id, "1");
            arb = arb->firstChild;

            IntrQ(c, &arb);

            while(arb->nextSibling != NULL)
            {
                nr_ordin++;
                sprintf(nr_ordin_char, "%d", nr_ordin);
                strcpy(arb->nextSibling->info->id, id_parinte);
                if(strcmp(id_parinte, "") != 0)
                    strcat(arb->nextSibling->info->id, ".");
                strcat(arb->nextSibling->info->id, nr_ordin_char);
                arb = arb->nextSibling;
                IntrQ(c, &arb);
            }
        }
    }
    DistrQ((void **)(&c));
}

int OverrideStyleSelector(TArb arb, void *selector, CMP cmp, char *style)
{
    int nrNoduriGasite = 0, i;
    TinfoNod vecNoduriGasite = CautaNoduriDupaSelector(arb, arb, selector,
     &nrNoduriGasite, cmp);
    if(nrNoduriGasite == 0)
    {
        free(vecNoduriGasite);
        return 1;
    }
    for(i = nrNoduriGasite - 1; i >= 0; i--)
    {
        if(vecNoduriGasite[i].nodCautat->info->style != NULL)
        {
            TAttr attr = calloc(1, sizeof(TNodAttr));
            attr->name = strdup(vecNoduriGasite[i].nodCautat->info->style->name);
            attr->value = strdup(style);
            StyleValueFormat(&attr);
            free(vecNoduriGasite[i].nodCautat->info->style->name);
            free(vecNoduriGasite[i].nodCautat->info->style->value);
            free(vecNoduriGasite[i].nodCautat->info->style);
            vecNoduriGasite[i].nodCautat->info->style = attr;
        }
        else
        {
            TAttr attr = calloc(1, sizeof(TNodAttr));
            attr->name = strdup("style");
            attr->value = strdup(style);
            StyleValueFormat(&attr);
            vecNoduriGasite[i].nodCautat->info->style = attr;
        }
        
    }
    free(vecNoduriGasite);
    return 0;
}

void OverrideStyleTag(TArb arb, char *tag, FILE *out_file, char *style)
{
    int caz = 0;
    TArb arbCautat, fam;
    char *tag_aux = strdup(tag);

    arbCautat = CautaNodDupaID(arb, tag, &fam, &caz);
    if(arbCautat == NULL)
    {
        fprintf(out_file, "Override style failed: no node found for selector");
        fprintf(out_file, " #%s!\n", tag_aux);
        free(tag_aux);
        return;
    }
    if(arbCautat->info->style != NULL)
    {
        TAttr attr = calloc(1, sizeof(TNodAttr));
        attr->name = strdup(arbCautat->info->style->name);
        attr->value = strdup(style);
        StyleValueFormat(&attr);
        free(arbCautat->info->style->name);
        free(arbCautat->info->style->value);
        free(arbCautat->info->style);
        arbCautat->info->style = attr;
    }
    else
    {
        TAttr attr = calloc(1, sizeof(TNodAttr));
        attr->name = strdup("style");
        attr->value = strdup(style);
        StyleValueFormat(&attr);
        arbCautat->info->style = attr;
    }
    free(tag_aux);
}

// Functia de OverrideStyle apelata din main care prelucreaza si input-ul
// pentru a-si da seama de tipul selectorului
void OverrideStyle(TArb arb, FILE *in_file, FILE *out_file)
{
    unsigned int i;
    char *line = calloc(N, sizeof(char));
    char *aux, *p, *el_de_caut_1 = calloc(N, sizeof(char)), *style;
    int caz;

    fgets(line, N, in_file);
    if(line[strlen(line) - 2] == 13)
        line[strlen(line) - 2] = '\0';
    
    aux = strtok(line, "\"");
    aux = strtok(NULL, "\"");
    p = strdup(aux);
    aux = strtok(NULL, "\"");
    aux = strtok(NULL, "\"");
    style = strdup(aux);
    if(p[0] == '#')
    {
        strcpy(el_de_caut_1, p + 1);
        OverrideStyleTag(arb, el_de_caut_1, out_file, style);
    }
    else if(p[0] == '.')
    {
        strcpy(el_de_caut_1, p + 1);
        if(OverrideStyleSelector(arb, el_de_caut_1, cmpClass, style) == 1)
        {
           fprintf(out_file, "Override style failed: no node found for selector");
           fprintf(out_file, " .%s!\n", el_de_caut_1); 
        }
    }
    else
    {
        pereche_char *per = calloc(1, sizeof(pereche_char));
        per->el_1 = calloc(N, sizeof(char));
        per->el_2 = calloc(N, sizeof(char));
        caz = 0;
        for(i = 1; i < strlen(p); i++)
        {
            if(isalpha(p[i]) == 0)
            {
                if(p[i] == '.')
                    caz = 1;
                if(p[i] == '>')
                    caz = 2;
            }
        }
        aux = strtok(p, " .>");
        strcpy(el_de_caut_1, aux);
        aux = strtok(NULL, " .>");
        if(aux != NULL)
        {
            strcpy(per->el_1, el_de_caut_1);
            strcpy(per->el_2, aux);
            if(caz == 0)
            {
                if(OverrideStyleSelector(arb, per, cmpTag_Stramos, style) == 1)
                {
                    fprintf(out_file, "Override style failed: no node found for");
                    fprintf(out_file, " selector %s %s!\n", per->el_1, per->el_2);
                }
            }
            if(caz == 1)
            {
                if(OverrideStyleSelector(arb, per, cmpTag_Class, style) == 1)
                {
                    fprintf(out_file, "Override style failed: no node found for");
                    fprintf(out_file, " selector %s.%s!\n", per->el_1, per->el_2);
                }
            }
            if(caz == 2)
            {
                if(OverrideStyleSelector(arb, per, cmpTag_Parent, style) == 1)
                {
                    fprintf(out_file, "Override style failed: no node found for");
                    fprintf(out_file, " selector %s>%s!\n", per->el_1, per->el_2);
                }
            }
        }
        else
        {
            if(OverrideStyleSelector(arb, el_de_caut_1, cmpTag, style) == 1)
            {
                fprintf(out_file, "Override style failed: no node found for");
                fprintf(out_file, " selector %s!\n", el_de_caut_1);
            }
        }
        free(per->el_1);
        free(per->el_2);
        free(per);
    }
    RemakeID(arb);
    free(style);
    free(line);
    free(p);
    free(el_de_caut_1);
}

void Append(char *init_value, char *style)
{
    char *p = calloc(N, sizeof(char));
    int nr_atr_1 = 0, nr_atr_2 = 0, ok = 0, i, j;
    pereche_char *vec_init = calloc(50, sizeof(pereche_char));
    pereche_char *vec_style = calloc(50, sizeof(pereche_char));
    unsigned int k;

    // creare vector pentru stilul initial
    for(k = 0; k < strlen(init_value); k++)
    {
        if(init_value[k] == ' ');
        else if(init_value[k] == ':')
        {
            vec_init[nr_atr_1].el_1 = calloc(N, sizeof(char));
            strcpy(vec_init[nr_atr_1].el_1, p);
            p[0] = '\0';
        }
        else if(init_value[k] == ';')
        {
            vec_init[nr_atr_1].el_2 = calloc(N, sizeof(char));
            strcpy(vec_init[nr_atr_1++].el_2, p);
            p[0] = '\0';
        }
        else
        {
            strncat(p, &init_value[k], 1);
        }
    }
    p[0] = '\0';
    // creare vector pentru stilul de append
    for(k = 0; k < strlen(style); k++)
    {
        if(style[k] == ' ');
        else if(style[k] == ':')
        {
            vec_style[nr_atr_2].el_1 = calloc(N, sizeof(char));
            strcpy(vec_style[nr_atr_2].el_1, p);
            p[0] = '\0';
        }
        else if(style[k] == ';')
        {
            vec_style[nr_atr_2].el_2 = calloc(N, sizeof(char));
            strcpy(vec_style[nr_atr_2++].el_2, p);
            p[0] = '\0';
        }
        else
        {
            strncat(p, &style[k], 1);
        }
    }

    // concatenarea celor 2 vectori
    for(i = 0; i < nr_atr_2; i++)
    {
        ok = 0;
        for(j = 0; j < nr_atr_1; j++)
        {
            if(strcmp(vec_style[i].el_1, vec_init[j].el_1) == 0)
            {
                strcpy(vec_init[j].el_2, vec_style[i].el_2);
                ok = 1;
                break;
            }
        }
        if(ok == 0)
        {
            vec_init[nr_atr_1].el_1 = calloc(N, sizeof(char));
            vec_init[nr_atr_1].el_2 = calloc(N, sizeof(char));
            strcpy(vec_init[nr_atr_1].el_1, vec_style[i].el_1);
            strcpy(vec_init[nr_atr_1].el_2, vec_style[i].el_2);
            nr_atr_1++;
        }
    }
    strcpy(init_value, "");
    // refacerea stilului sub forma de string
    for(i = 0; i < nr_atr_1; i++)
    {
        strcat(init_value, vec_init[i].el_1);
        strcat(init_value, ": ");
        strcat(init_value, vec_init[i].el_2);
        strcat(init_value, "; ");
    }
    init_value[strlen(init_value) - 1] = '\0';

    for(i = 0; i < nr_atr_1; i++)
    {
        free(vec_init[i].el_1);
        free(vec_init[i].el_2);
    }
    for(i = 0; i < nr_atr_2; i++)
    {
        free(vec_style[i].el_1);
        free(vec_style[i].el_2);
    }
    free(vec_init);
    free(vec_style);
    free(p);
}

void AppendStyleTag(TArb arb, char *tag, FILE *out_file, char *style)
{
    int caz = 0;
    TArb arbCautat, fam;
    char *tag_aux = strdup(tag);

    arbCautat = CautaNodDupaID(arb, tag, &fam, &caz);
    if(arbCautat == NULL)
    {
        fprintf(out_file, "Override style failed: no node found for selector");
        fprintf(out_file, " #%s!\n", tag_aux);
        free(tag_aux);
        return;
    }
    if(arbCautat->info->style != NULL)
    {
        TAttr attr = calloc(1, sizeof(TNodAttr));
        attr->name = strdup(arbCautat->info->style->name);
        char *aux = calloc(N, sizeof(char));
        strcpy(aux, arbCautat->info->style->value);
        Append(aux, style);
        attr->value = strdup(aux);
        free(aux);
        free(arbCautat->info->style->name);
        free(arbCautat->info->style->value);
        free(arbCautat->info->style);
        arbCautat->info->style = attr;
    }
    else
    {
        TAttr attr = calloc(1, sizeof(TNodAttr));
        attr->name = strdup("style");
        attr->value = strdup(style);
        StyleValueFormat(&attr);
        arbCautat->info->style = attr;
    }
    free(tag_aux);
}

int AppendStyleSelector(TArb arb, void *selector, CMP cmp, char *style)
{
    int nrNoduriGasite = 0, i;
    char *aux_style = calloc(N, sizeof(char));
    TinfoNod vecNoduriGasite = CautaNoduriDupaSelector(arb, arb, selector,
     &nrNoduriGasite, cmp);
    if(nrNoduriGasite == 0)
    {
        free(vecNoduriGasite);
        free(aux_style);
        return 1;
    }
    for(i = nrNoduriGasite - 1; i >= 0; i--)
    {
        if(vecNoduriGasite[i].nodCautat->info->style != NULL)
        {
            TAttr attr = calloc(1, sizeof(TNodAttr));
            attr->name = strdup(vecNoduriGasite[i].nodCautat->info->style->name);
            char *aux = calloc(N, sizeof(char));
            strcpy(aux, vecNoduriGasite[i].nodCautat->info->style->value);
            strcpy(aux_style, style);
            Append(aux, style);
            strcpy(style, aux_style);
            attr->value = strdup(aux);
            free(aux);
            free(vecNoduriGasite[i].nodCautat->info->style->name);
            free(vecNoduriGasite[i].nodCautat->info->style->value);
            free(vecNoduriGasite[i].nodCautat->info->style);
            StyleValueFormat(&attr);
            vecNoduriGasite[i].nodCautat->info->style = attr;
        }
        else
        {
            TAttr attr = calloc(1, sizeof(TNodAttr));
            attr->name = strdup("style");
            attr->value = strdup(style);
            StyleValueFormat(&attr);
            vecNoduriGasite[i].nodCautat->info->style = attr;
        }
        
    }
    free(vecNoduriGasite);
    free(aux_style);
    return 0;
}

// Functia de AppendStyle apelata din main care prelucreaza si input-ul
// pentru a-si da seama de tipul selectorului
void AppendStyle(TArb arb, FILE *in_file, FILE *out_file)
{
    unsigned int i;
    char *line = calloc(N, sizeof(char));
    char *aux, *p, *el_de_caut_1 = calloc(N, sizeof(char)), *style;
    int caz;

    fgets(line, N, in_file);
    if(line[strlen(line) - 2] == 13)
        line[strlen(line) - 2] = '\0';
    
    aux = strtok(line, "\"");
    aux = strtok(NULL, "\"");
    p = strdup(aux);
    aux = strtok(NULL, "\"");
    aux = strtok(NULL, "\"");
    style = strdup(aux);
    if(p[0] == '#')
    {
        strcpy(el_de_caut_1, p + 1);
        AppendStyleTag(arb, el_de_caut_1, out_file, style);
    }
    else if(p[0] == '.')
    {
        strcpy(el_de_caut_1, p + 1);
        if(AppendStyleSelector(arb, el_de_caut_1, cmpClass, style) == 1)
        {
            fprintf(out_file, "Append to style failed: no node found for");
            fprintf(out_file, " selector .%s!\n", el_de_caut_1);
        }
    }
    else
    {
        pereche_char *per = calloc(1, sizeof(pereche_char));
        per->el_1 = calloc(N, sizeof(char));
        per->el_2 = calloc(N, sizeof(char));
        caz = 0;
        for(i = 1; i < strlen(p); i++)
        {
            if(isalpha(p[i]) == 0)
            {
                if(p[i] == '.')
                    caz = 1;
                if(p[i] == '>')
                    caz = 2;
            }
        }
        aux = strtok(p, " .>");
        strcpy(el_de_caut_1, aux);
        aux = strtok(NULL, " .>");
        if(aux != NULL)
        {
            strcpy(per->el_1, el_de_caut_1);
            strcpy(per->el_2, aux);
            if(caz == 0)
            {
                if(AppendStyleSelector(arb, per, cmpTag_Stramos, style) == 1)
                {
                    fprintf(out_file, "Append to style failed: no node found for");
                    fprintf(out_file, " selector %s %s!\n", per->el_1, per->el_2);
                }
            }
            if(caz == 1)
            {
                if(AppendStyleSelector(arb, per, cmpTag_Class, style) == 1)
                {
                    fprintf(out_file, "Append to style failed: no node found for");
                    fprintf(out_file, " selector %s.%s!\n", per->el_1, per->el_2);
                }
            }
            if(caz == 2)
            {
                if(AppendStyleSelector(arb, per, cmpTag_Parent, style) == 1)
                {
                    fprintf(out_file, "Append to style failed: no node found for");
                    fprintf(out_file, " selector %s>%s!\n", per->el_1, per->el_2);
                }
            }
        }
        else
        {
            if(AppendStyleSelector(arb, el_de_caut_1, cmpTag, style) == 1)
            {
                fprintf(out_file, "Append to style failed: no node found for");
                fprintf(out_file, " selector %s!\n", el_de_caut_1);
            }
        }
        free(per->el_1);
        free(per->el_2);
        free(per);
    }
    RemakeID(arb);
    free(style);
    free(line);
    free(p);
    free(el_de_caut_1);
}

// Sparge id-ul unui nod in partea care provine de la parinte si partea proprie
void SplitID(char *id, char *first, char *last)
{
    char *aux = strdup(id), *ant = calloc(15, sizeof(char));

    char *aux_1 = strtok(aux, ".");

    while(aux_1 != NULL)
    {
        if(strcmp(ant, "") != 0)
        {
            strcat(first, ant);
            strcat(first, ".");
        }
        strcpy(last, aux_1);
        strcpy(ant, aux_1);
        aux_1 = strtok(NULL, ".");
    }
    free(ant);
    free(aux);
}