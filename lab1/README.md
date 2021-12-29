# Artificial Intelligence Lab1
This is a short instruction on how to build and run my code.

For lab detail, please see: https://cs.nyu.edu/courses/fall21/CSCI-GA.2560-001/lab1.html

## How to build
Use `Makefile` to build the code:
```
$ make
```

then you can see the executable `main` under the same directory.


## How to run
Follow the lab requirement, the program has five options and one non-option argument.

A typical way to run the program is,
```
$ ./main [-v] -s <start_node> -g <goal_node> -a <algo> [-d <depth>] <input_file>
```

### options
All options follow [POSIX recommended convention](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html), each option has a short and a long version. Short option start with `-`, long option start with `--`.

- `-v` or `--verbose`, no argument, optional, enable verbose mode
- `-s` or `--start`, string argument, mandatory, specify start node
- `-g` or `--goal`, string argument, mandatory, specify goal node
- `-a` or `--alg`, argument are either `BFS`, `ID` or `ASTAR`, mandatory, specify algorithm
- `-d` or `--depth`, positive integer argument, mandatory when algorithm is `ID`, specify the initial depth

### non-option argument
The program needs one non-option argument, the input file.

## Author
Kevin Chang: tc3149@nyu.edu
