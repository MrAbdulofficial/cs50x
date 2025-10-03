#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct person
{
    struct person *parents[2];
    char alleles[2];
}
person;

const int GENERATIONS = 3;
const int INDENT_LENGTH = 4;

person *create_family(int generations);
void print_family(person *p, int generation);
void free_family(person *p);
char random_allele(void);
char *blood_group(char a1, char a2);

int main(void)
{
    srand(time(0));

    person *p = create_family(GENERATIONS);

    print_family(p, 0);

    free_family(p);
}

person *create_family(int generations)
{
    person *p = malloc(sizeof(person));
    if (p == NULL)
    {
        return NULL;
    }

    if (generations > 1)
    {
        p->parents[0] = create_family(generations - 1);
        p->parents[1] = create_family(generations - 1);

        p->alleles[0] = p->parents[0]->alleles[rand() % 2];
        p->alleles[1] = p->parents[1]->alleles[rand() % 2];
    }
    else
    {
        p->parents[0] = NULL;
        p->parents[1] = NULL;

        p->alleles[0] = random_allele();
        p->alleles[1] = random_allele();
    }
    return p;
}

void print_family(person *p, int generation)
{
    if (p == NULL)
    {
        return;
    }

    for (int i = 0; i < generation * INDENT_LENGTH; i++)
    {
        printf(" ");
    }

    if (generation == 0)
    {
        printf("Child (Gen %i): Blood Group %s (%c%c)\n",
               generation, blood_group(p->alleles[0], p->alleles[1]),
               p->alleles[0], p->alleles[1]);
    }
    else if (generation == 1)
    {
        printf("Parent (Gen %i): Blood Group %s (%c%c)\n",
               generation, blood_group(p->alleles[0], p->alleles[1]),
               p->alleles[0], p->alleles[1]);
    }
    else if (generation == 2)
    {
        printf("Grandparent (Gen %i): Blood Group %s (%c%c)\n",
               generation, blood_group(p->alleles[0], p->alleles[1]),
               p->alleles[0], p->alleles[1]);
    }
    else
    {
        printf("Gen %i: Blood Group %s (%c%c)\n",
               generation, blood_group(p->alleles[0], p->alleles[1]),
               p->alleles[0], p->alleles[1]);
    }

    print_family(p->parents[0], generation + 1);
    print_family(p->parents[1], generation + 1);
}

void free_family(person *p)
{
    if (p == NULL)
    {
        return;
    }
    free_family(p->parents[0]);
    free_family(p->parents[1]);
    free(p);
}

char random_allele(void)
{
    int r = rand() % 3;
    if (r == 0)
    {
        return 'A';
    }
    else if (r == 1)
    {
        return 'B';
    }
    else
    {
        return 'O';
    }
}

char *blood_group(char a1, char a2)
{
    if ((a1 == 'A' && a2 == 'A') || (a1 == 'A' && a2 == 'O') || (a1 == 'O' && a2 == 'A'))
        return "A";
    if ((a1 == 'B' && a2 == 'B') || (a1 == 'B' && a2 == 'O') || (a1 == 'O' && a2 == 'B'))
        return "B";
    if ((a1 == 'A' && a2 == 'B') || (a1 == 'B' && a2 == 'A'))
        return "AB";
    return "O";
}
