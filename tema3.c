/* DRAGHICI Vlad Matei - 312CB */
#include <stdio.h>
#include "utils.h"

int main(int argc, char *argv[])
{
    int i;
    (void)argc;
    FILE *html_file = fopen(argv[1], "rt");
    FILE *in_file = fopen(argv[2], "rt");
    FILE *out_file = fopen(argv[3], "wt");

    TArb arb = CitireHTML(html_file);
    fclose(html_file);
    int n;

    char *cmd = calloc(N, sizeof(char));
    if(!cmd)
        return 1;
    
    fscanf(in_file, "%d\n", &n);
    for(i = 0; i < n; i++)
    {
        fscanf(in_file, "%s\n", cmd);
        if(strcmp(cmd, "add") == 0)
        {
            Add(arb, in_file, out_file);
        }
        if(strcmp(cmd, "format") == 0)
        {
            PrintHTML(arb, 0, out_file);
        }
        if(strcmp(cmd, "deleteRecursively") == 0)
        {
            DeleteRecursively(arb, in_file, out_file);
        }
        if(strcmp(cmd, "overrideStyle") == 0)
        {
            OverrideStyle(arb, in_file, out_file);
        }
        if(strcmp(cmd, "appendStyle") == 0)
        {
            AppendStyle(arb, in_file, out_file);
        }
    }
    DistNod(&arb);
    
    free(cmd);
    fclose(in_file);
    fclose(out_file);

    return 0;
}