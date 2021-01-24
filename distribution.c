/*******************************************************************************
 * distribution handler of fraggy.agg
 *
 * @author Lars Thoms
 * @date 2020-01-24
 ******************************************************************************/

#include "distribution.h"


struct distribution_tpl* distribution = NULL;


void increase_distribution(int key)
{
    struct distribution_tpl *s;

    HASH_FIND_INT(distribution, &key, s);
    if(s == NULL)
    {
        s        = (struct distribution_tpl*) malloc(sizeof * s);
        s->key   = key;
        s->value = 0;

        HASH_ADD_INT(distribution, key, s);
    }

    s->value++;
}


int sort_distribution(struct distribution_tpl *a, struct distribution_tpl *b)
{
    return (a->key - b->key);
}


void print_distribution()
{
    struct distribution_tpl *s;

    // Sort by key
    HASH_SORT(distribution, sort_distribution);

    // Create CSV
    fprintf(stdout, "fragments,amount\n");
    for(s = distribution; s != NULL; s = s->hh.next)
    {
        fprintf(stdout, "%d,%d\n", s->key, s->value);
    }
}
