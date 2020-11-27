/**
 * distribution.h
 *
 * @author Lars Thoms <lars@thoms.io>
 */

#ifndef FRAGGY_DISTRIBUTION_H
#define FRAGGY_DISTRIBUTION_H

/* ====[ Requirements ]========================================================================== */

// Includes
#include <stdio.h>
#include "uthash.h"


/* ====[ Declarations ]========================================================================== */

/**
 * Hash map to store fragmentation counts and their frequency
 */
struct distribution_tpl
{
    int key, value;
    UT_hash_handle hh;
};

/**
 * Add amount of fragments to distribution table
 *
 * @param int fragmentation count
 */
void increase_distribution(int);

/**
 * Sort by key ascending
 *
 * @param struct distribution_tpl *
 * @param struct distribution_tpl *
 * @return int
 */
int sort_distribution(struct distribution_tpl *, struct distribution_tpl *);

/**
 * Generate and print CSV from distribution map
 */
void print_distribution(void);

#endif //FRAGGY_DISTRIBUTION_H
