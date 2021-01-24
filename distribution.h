/*******************************************************************************
 * distribution handler of fraggy.agg
 *
 * @author Lars Thoms
 * @date 2020-01-24
 ******************************************************************************/

#ifndef FRAGGY_DISTRIBUTION_H
#define FRAGGY_DISTRIBUTION_H

#include <stdio.h>
#include "uthash.h"


/*******************************************************************************
 * declarations
 ******************************************************************************/

/**
 * hash map to store fragmentation counts and their frequency
 */
struct distribution_tpl
{
    int key, value;
    UT_hash_handle hh;
};

/**
 * add amount of fragments to distribution table
 *
 * @param int fragmentation count
 */
void increase_distribution(int);

/**
 * sort by key ascending
 *
 * @param struct distribution_tpl *
 * @param struct distribution_tpl *
 * @return int
 */
int sort_distribution(struct distribution_tpl *, struct distribution_tpl *);

/**
 * generate and print CSV from distribution map
 */
void print_distribution(void);

#endif //FRAGGY_DISTRIBUTION_H
