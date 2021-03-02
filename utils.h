/* DRAGHICI Vlad Matei - 312CB */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#ifndef STRUCT
#define STRUCT

#define N 250

typedef struct TNodAttr
{
    char *name;
    char *value;
    struct TNodAttr *next;
} TNodAttr, *TAttr;

typedef struct TNodInfo
{
    char *type;
    char *id;
    TAttr style;
    TAttr otherAttributes;
    int isSelfClosing;
    char *contents;
} TNodInfo, *TInfo;

typedef struct TNodArb
{
    TInfo info;
    struct TNodArb *nextSibling;
    struct TNodArb *firstChild;
} TNodArb, *TArb;

typedef struct celst
{ 	
	struct celst *urm;
	void* info;
} TCel, *TList;

typedef struct stiva
{ 	
	size_t dime;
	TList vf;
} TStiva, *ASt;

typedef struct coada
{ 
	size_t dime;
	TList ic, sc;
} TCoada, *AQ;

typedef struct
{
    TArb nodCautat;
    TArb familie;
    int caz;
} infoNod, *TinfoNod;

typedef struct
{
    char *el_1;
    char *el_2;
} pereche_char;


typedef int (*CMP)(TinfoNod, void *, TArb);

typedef enum
{
    PARSE_ERROR             = 0,
    PARSE_CONTENTS          = 1,
    PARSE_OPENING_BRACKET   = 2,
    PARSE_TAG_TYPE          = 3,
    PARSE_CLOSING_TAG       = 4,
    PARSE_REST_OF_TAG       = 5,
    PARSE_ATTRIBUTE_NAME    = 6,
    PARSE_ATTRIBUTE_EQ      = 7,
    PARSE_ATTRIBUTE_VALUE   = 8,
    PARSE_SELF_CLOSING      = 9,
} TParseState;

//Functii stiva si coada
void* InitS(size_t d);
int Push(void* a, void* ae);
int Pop(void* s, void* ae);
void DistrS(void** a);
void* InitQ(size_t d);
int IntrQ(void* a, void* ae);
int ExtrQ(void* a, void* ae);
void DistrQ(void **a);

//Functii arbore
TParseState Interpret(TParseState currentState, char c, TArb *a, int *deschis,
 void *stiva_arb, TArb *root, int *next, TAttr *attr);
TArb CreateTagNode(char c);
TArb CreateNode();
TArb CitireHTML(FILE *html_file);
void PrintHTML(TArb arb, int nivel, FILE *out_file);
void StyleValueFormat(TAttr *attr);
void DistNod(TArb *arb);
void AddNod(TArb arb, char *id, char *tag, FILE *in_file);
TArb CautaNodDupaID(TArb arb, char *id, TArb *fam, int *caz);
void Add(TArb arb, FILE *in_file, FILE *out_file);
void DeleteRecursively(TArb arb, FILE *in_file, FILE *out_file);
void DeleteRecursivelyTag(TArb arb, char *tag, FILE *out_file);
int DeleteRecursivelySelector(TArb arb, void *selector, CMP cmp);
int cmpClass(TinfoNod pr, void *clasa, TArb arb);
int cmpTag(TinfoNod pr, void *tag, TArb arb);
int cmpTag_Class(TinfoNod pr, void *per, TArb arb);
int cmpTag_Parent(TinfoNod pr, void *per, TArb arb);
int cmpTag_Stramos(TinfoNod pr, void *per, TArb arb);
TinfoNod CautaNoduriDupaSelector(TArb arb, TArb root, void *selector, int *nrNoduriGasite, CMP cmp);
void RemakeID(TArb arb);
void OverrideStyle(TArb arb, FILE *in_file, FILE *out_file);
int OverrideStyleSelector(TArb arb, void *selector, CMP cmp, char *style);
void OverrideStyleTag(TArb arb, char *tag, FILE *out_file, char *style);
int AppendStyleSelector(TArb arb, void *selector, CMP cmp, char *style);
void AppendStyle(TArb arb, FILE *in_file, FILE *out_file);
void Append(char *init_value, char *style);
void AppendStyleTag(TArb arb, char *tag, FILE *out_file, char *style);
void SplitID(char *id, char *first, char *last);
#endif