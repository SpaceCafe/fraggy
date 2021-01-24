/*******************************************************************************
 * get fragmentation distribution of a filesystem,
 * which implements `FIEMAP` as an io system call.
 *
 * @author Lars Thoms
 * @date 2020-01-24
 ******************************************************************************/

#ifndef FRAGGY_AGG_H
#define FRAGGY_AGG_H

/* enable POSIX.1-2008 + XSI features */
#define _XOPEN_SOURCE 700

/* enable 64-bit file sizes and offsets support of C library */
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

/* set number of used file descriptors
   POSIX.1 allows minimum of 20 descriptors minus 3 standard streams.
   Therefore, 15 is safe. */
#ifndef USE_FDS
#define USE_FDS 15
#endif

#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <linux/fiemap.h>
#include <linux/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "distribution.h"


/*******************************************************************************
 * declarations
 ******************************************************************************/

/**
 * Fetch FIEMAP from given file descriptor and count extents.
 * Code fragments are copied from filefrag.c of the ext4defrag toolchain.
 *
 * @param  long           file descriptor
 * @param  unsigned int * fragmentation count
 * @return int            errno
 */
static int filefrag_fiemap(long, unsigned int *);


/**
 * Determine fragmentation count of given node and populate distribution list
 * It is called by nftw()
 *
 * @param  const char *        node's path
 * @param  const struct stat * node's stat structure
 * @param  const int           node's type (file, directory, symbolic, ...)
 * @param  struct FTW *        FTW structure
 * @return int                 errno
 */
static int node_fragmentation(const char *, const struct stat *, const int, struct FTW *);


/**
 * Print usage information
 *
 * @param const char * program name
 */
static void print_usage(const char *);

#endif //FRAGGY_DISTRIBUTION_H
