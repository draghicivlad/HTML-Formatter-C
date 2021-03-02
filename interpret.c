/* DRAGHICI Vlad Matei - 312CB */
#include "utils.h"
#include <ctype.h>
#include <stdlib.h>

TParseState Interpret(TParseState currentState, char c, TArb *a, int *deschis,
 void *stiva_arb, TArb *root, int *next, TAttr *attr)
{
    TParseState nextState;
    switch (currentState)
    {
    case PARSE_CONTENTS:
        if (c == '<')
        {
            // Eliminarea spatiului dupa content
            if((*a) != NULL)
                if(strcmp((*a)->info->contents, "") != 0)
                    while((*a)->info->contents[strlen((*a)->info->contents) - 1] == ' ')
                        (*a)->info->contents[strlen((*a)->info->contents) - 1] = '\0';
            nextState = PARSE_OPENING_BRACKET;
        }
        else
        {   nextState = PARSE_CONTENTS;
            if(c != '\n' && c != 13)
            {
                // Eliminarea spatiului inainte de content
                if(strcmp((*a)->info->contents, "") == 0 && c == ' ')
                    break;
                strncat((*a)->info->contents, &c, 1);
            }
        }
        break;
    case PARSE_OPENING_BRACKET:
        if (isspace(c)){   nextState = PARSE_OPENING_BRACKET;  }
        else if (c == '>')      {   nextState = PARSE_ERROR;            }
        else if (c == '/')      {   nextState = PARSE_CLOSING_TAG;      }
        else
        {
            nextState = PARSE_TAG_TYPE; 
            // Nodul nou este radacina sau fratele nodului precedent
            if(*deschis == 0) 
            {
                if(*next == 0) // Radacina
                {
                    (*a) = CreateTagNode(c);
                    if(*root == NULL)
                        *root = *a;
                    *deschis = 1;
                }
                else // Fratele nodului precedent
                {
                    int id;
                    (*a)->nextSibling = CreateTagNode(c);
                    char *first = calloc(N, sizeof(char)), *last = calloc(N,
                     sizeof(char));

                    if(!first || !last)
                        return PARSE_ERROR;

                    // Crearea id-ului
                    SplitID((*a)->info->id, first, last);
                    id = atoi(last);
                    id++;
                    sprintf(last, "%d", id);
                    strcpy((*a)->nextSibling->info->id, first);
                    strcat((*a)->nextSibling->info->id, last);

                    free(first);
                    free(last);

                    (*a) = (*a)->nextSibling;
                    *deschis = 1;
                }
            }
            else // Nodul nou este primul copil al nodului precedent
            {
                // Salveaza parintele in stiva pentru intoarcerea la acesta
                Push(stiva_arb, a);
                (*a)->firstChild = CreateTagNode(c);

                // Crearea id-ului
                strcpy((*a)->firstChild->info->id, (*a)->info->id);
                if(strcmp((*a)->info->id, "") != 0)
                    strcat((*a)->firstChild->info->id, ".");
                strcat((*a)->firstChild->info->id, "1");

                *a = (*a)->firstChild;
                *next = 0;
            }
        }
        break;
    case PARSE_TAG_TYPE:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else
        {
            nextState = PARSE_TAG_TYPE;
            strncat((*a)->info->type, &c, 1);
        }
        break;
    case PARSE_CLOSING_TAG:
        if (c == '>')
        {
            nextState = PARSE_CONTENTS;

            if(*deschis == 1)
            { 
                *next = 1;
                *deschis = 0;
            }
            else
            {
                // Nodul actual devine parintele nodului actual
                Pop(stiva_arb, a);
                *next = 1;
            }
        }
        else                    {   nextState = PARSE_CLOSING_TAG;      }
        break;
    case PARSE_REST_OF_TAG:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else if (c == '/')      {   nextState = PARSE_SELF_CLOSING;     }
        else // Creare atribut temporar
        {   
            nextState = PARSE_ATTRIBUTE_NAME;
            *attr = calloc(1, sizeof(TNodAttr));
            if(!attr)
                return PARSE_ERROR;
            (*attr)->name = calloc(N, sizeof(char));
            if(!(*attr)->name)
                return PARSE_ERROR;
            (*attr)->value = calloc(N, sizeof(char));
            if(!(*attr)->value)
                return PARSE_ERROR;

            if(c != '\n') 
                strncat((*attr)->name, &c, 1);
        }
        break;
    case PARSE_ATTRIBUTE_NAME:
        if (c == '=')           {   nextState = PARSE_ATTRIBUTE_EQ;     }
        else
        {   
            nextState = PARSE_ATTRIBUTE_NAME;
            if(c != '\n') 
                strncat((*attr)->name, &c, 1);
        }
        break;
    case PARSE_ATTRIBUTE_EQ:
        if (c == '\"')          {   nextState = PARSE_ATTRIBUTE_VALUE;  }
        break;
    case PARSE_ATTRIBUTE_VALUE:
        if (c == '\"')
        {
            nextState = PARSE_REST_OF_TAG;
            if(strcmp((*attr)->name, "style") == 0)
            {
                StyleValueFormat(attr);
                (*a)->info->style = *attr;
            }
            else
            {
                if((*a)->info->otherAttributes == NULL)
                    (*a)->info->otherAttributes = *attr;
                else
                {
                    TAttr aux;
                    for(aux = (*a)->info->otherAttributes; aux->next != NULL;
                     aux = aux->next);
                    aux->next = *attr;
                }
            }
        }
        else
        {
            nextState = PARSE_ATTRIBUTE_VALUE;
            if(c != '\n') 
                strncat((*attr)->value, &c, 1);
        }
        break;
    case PARSE_SELF_CLOSING:
        if (c == '>')
        {   
            nextState = PARSE_CONTENTS; 
            (*a)->info->isSelfClosing = 1;
            *next = 1;
            *deschis = 0;
        }
        else                    {   nextState = PARSE_ERROR;            }
        break;
    default:
        break;
    }     
    return nextState;
}