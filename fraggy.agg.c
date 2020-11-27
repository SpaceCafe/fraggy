/**
 * fraggy.agg.c
 *
 * @author Lars Thoms <lars@thoms.io>
 */

/* ====[ Requirements ]========================================================================== */

#include "fraggy.agg.h"


/* ====[ FIEMAP ]================================================================================ */

static int filefrag_fiemap(long fd, unsigned int *num_extents)
{
    __u64 buf[2048];

    struct fiemap*        fiemap = (struct fiemap*) buf;
    struct fiemap_extent* fm_ext = &fiemap->fm_extents[0];
    struct fiemap_extent  fm_last;

    int count = (sizeof(buf) - sizeof(*fiemap)) / sizeof(struct fiemap_extent);

    unsigned long long expected = 0, expected_dense = 0;
    unsigned long      flags    = 0;
    unsigned int       tot_extents = 0, i;
    int                n = 0, last = 0, rc;

    memset(fiemap, 0, sizeof(struct fiemap));
    memset(&fm_last, 0, sizeof(fm_last));

    do
    {
        fiemap->fm_length       = ~0ULL;
        fiemap->fm_flags        = flags;
        fiemap->fm_extent_count = count;

        rc = ioctl(fd, FS_IOC_FIEMAP, (unsigned long) fiemap);

        if(rc < 0)
        {
            static int fiemap_incompat_printed;

            rc = -errno;
            if(rc == -EBADR && !fiemap_incompat_printed)
            {
                fprintf(stderr, "FIEMAP failed with unknown flags %x\n", fiemap->fm_flags);
                fiemap_incompat_printed = 1;
            }
            return rc;
        }

        /* If 0 extents are returned, then more ioctls are not needed */
        if(fiemap->fm_mapped_extents == 0)
        {
            break;
        }

        for (i = 0; i < fiemap->fm_mapped_extents; i++)
        {
            expected_dense = fm_last.fe_physical + fm_last.fe_length;
            expected       = fm_last.fe_physical + fm_ext[i].fe_logical - fm_last.fe_logical;

            if((fm_ext[i].fe_logical != 0) &&
               (fm_ext[i].fe_physical != expected) &&
               (fm_ext[i].fe_physical != expected_dense))
            {
                tot_extents++;
            }
            else
            {
                expected = 0;
                if(!tot_extents)
                {
                    tot_extents = 1;
                }
            }

            if(fm_ext[i].fe_flags & FIEMAP_EXTENT_LAST)
            {
                last = 1;
            }

            fm_last = fm_ext[i];
            n++;
        }

        fiemap->fm_start = (fm_ext[i - 1].fe_logical + fm_ext[i - 1].fe_length);
    }
    while(last == 0);

    *num_extents = tot_extents;

    return 0;
}


/* ====[ Node Handler ]========================================================================== */

static int node_fragmentation(const char *node_path, const struct stat *node_info,
                              const int node_typeflag, struct FTW *node_pathinfo)
{
    unsigned int num_extents = 1;
    int rc = 0;

    // Given path is a not readable directory
    if(node_typeflag == FTW_DNR)
    {
        return -(errno = EACCES);
    }

    if((node_typeflag == FTW_F) || (node_typeflag == FTW_D) || (node_typeflag == FTW_DP))
    {

        // Initialize file descriptor
        long file = open(node_path, O_RDONLY);

        // Check for possible failures
        if(file < 0)
        {
            perror(node_path);
            return -errno;
        }


        rc = filefrag_fiemap(file, &num_extents);
        close(file);

        // Empty files are not interesting
        if((rc == 0) && (num_extents > 0))
        {
            increase_distribution(num_extents);
        }
    }

    return -(errno = rc);
}



/* ====[ Usage ]================================================================================= */

static void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s [ FILE | DIRECTORY ]\n"
                    "Get a list of all amount of extents per node in filesystem", program_name);
}


/* ====[ Program ]=============================================================================== */

int main(int argc, char *argv[])
{
    int rc;

    // Check if parameter is set
    if (argc > 1)
    {
        // Check for invalid directory path
        if(argv[1] == NULL || *argv[1] == '\0')
        {
            fprintf(stderr, "%s.\n", strerror(errno = EINVAL));
            return EXIT_FAILURE;
        }

        rc = nftw(argv[1], node_fragmentation, USE_FDS, FTW_PHYS);
        if(rc)
        {
            fprintf(stderr, "%s.\n", strerror(errno = rc));
            return EXIT_FAILURE;
        }
    }

    // Print usage
    else
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    print_distribution();
    return EXIT_SUCCESS;
}
