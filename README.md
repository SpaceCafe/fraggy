# Fraggy

Get fragmentation distribution of a filesystem, which implements `FIEMAP` as an IO system call.


## Requirements

-   GCC
-   CMake
-   R *(optional)*

    ```
    $ R --no-save <<< "install.packages(c('argparser', 'tidyverse', 'svglite'))"
    ```


## Build

```
$ mkdir build/
$ cd build/
$ cmake build ../
$ make
```


## Usage

The aggregator `fraggy.agg` only needs a path as a sole parameter. It prints
the CSV directly to standard output. Output redirection on the shell is
required to write into a file.

```
$ ./fraggy.agg /mnt/filesystem > frag_distribution.csv
```

To run the analyser and visualizer a minimum of one CSV from `fraggy.agg` is
required. The number of distributions to be compared is not limited. To set
ordinary labels to each file the optional parameter `-l | --label` can be used.
They must match the number of given input files.

More information can be found on the program's help page `-h | --help`.

```
$ ./fraggy.anlz                                                     \
    --prefix "disk1_ext4_"                                          \
    --output "./results"                                            \
    --input  "./frag_distribution1.csv" "./frag_distribution2.csv"  \
    --label  "fragmented" "defragmented"
```
