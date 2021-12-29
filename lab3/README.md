# Artificial Intelligence Lab3
This is a short instruction on how to build and run my code.

For lab detail, please see: https://cs.nyu.edu/courses/fall21/CSCI-GA.2560-001/lab3.html

## How to build
Use `Makefile` to build the code:
```
$ make
```

then you can see the executable `lab3` under the same directory.


## How to run
Follow the lab requirement, the program has four options and one non-option argument.

A typical way to run the program is,
```
$ ./lab3 [--df <discount_factor>] [--tol <tolerance>] [--iter <iterations>] [--min] <input_file>
```

### options
All options follow [POSIX recommended convention](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html), each option has a short and a long version. Short options start with `-`, long options start with `--`.

- `-d` or `--df`, float number argument in range [0, 1], optional, specify discount factor
- `-t` or `--tol`, float number argument, optional, specify tolerance
- `-i` or `--iter`, integer argument, optional, specify iterations
- `-m` or `--min`, no argument, optional, enable minimization mode

### non-option argument
The program needs one non-option argument, the input file.


## Contact
Kevin Chang: tc3149@nyu.edu
