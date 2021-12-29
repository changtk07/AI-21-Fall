# Artificial Intelligence Lab4
This is a short instruction on how to build and run my code.

For lab detail, please see: https://cs.nyu.edu/courses/fall21/CSCI-GA.2560-001/lab4.html

## How to build
Use `Makefile` to build the code:
```
$ make
```

then you can see two executable `knn` and `kmeans` under the `build` directory.


## How to run
Follow the lab requirement, the `knn` program has five flags; `kmeans` program has two flags.

A typical way to run `knn` program is:
```
$ build/knn [--k <num_of_cluster>] [--dist <dist_func>] [--unitw] --train <train_file> --test <test_file>
```
and `kmeans`:
```
$ build/kmeans [--dist <dist_func>] --data <data_file> <init_centroids_list>
```

## Options
All options follow [POSIX recommended convention](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html), each option has a short and a long version. Short options start with `-`, long options start with `--`.


### kNN
- `-k` or `--k`, positive integer argument, optional, default to 3, specify number of clusters
- `-d` or `--dist`, either `e2` or `manh`, optional, default to `e2`, specify distance function
- `-u` or `--unitw`, no argument, optional, enable unit weight voting
- `-t` or `--train`, file path argument, mandatory, specify train file
- `e` or `--test`, file path argument, mandatory, specify test file


### kMeans
- `-d` or `--dist`, either `e2` or `manh`, optional, default to `e2`, specify distance function
- `-f` or `--data`, file path argument, mandatory, specify data file

The kMeans program also requires a list of initial centroids, each centroid looks like `1,2,3`(integers seperated by commas) and are seperated by one space. The list shouldn't be empty.


## Contact
Kevin Chang: tc3149@nyu.edu
