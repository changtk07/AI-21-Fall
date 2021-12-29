# Artificial Intelligence Lab2
This is a short instruction on how to build and run my code.

For lab detail, please see: https://cs.nyu.edu/courses/fall21/CSCI-GA.2560-001/lab2.html

## How to build
Use `Makefile` to build the code:
```
$ make
```

then you can see the executable `lab2` under the same directory.


## How to run
Follow the lab requirement, the program has two options and one non-option argument.

A typical way to run the program is,
```
$ ./lab2 [-v] -m <mode> <input_file>
```

### options
All options follow [POSIX recommended convention](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html), each option has a short and a long version. Short options start with `-`, long options start with `--`.

- `-v` or `--verbose`, no argument, optional, enable verbose mode
- `-m` or `--mode`, argument is either `cnf`, `dpll` or `solver`, mandatory, specify program mode

### non-option argument
The program needs one non-option argument, the input file.

## Extra Credit
My program supports `_` and parenthesis. I've tested the program against the following BNF sentences
```
(((!!!!!(!(!!!(!!!(!!(!!A)))))))) | !!((!B))
((!!!(!!(!!(!!!!(!(!(!A))) <=> !!!!(!!B))) <=> ((!!(!!!(!C)))))))
```
```
!!(!!(((((!!!!P)) & !(!!(!!(!!Q))))) => !!!!W <=> !!(!!A | ((B & C)))))
(((((!!A) => !(!!(!B))))))
!!(!!(!!(!!(!!C)) | !!!!(!!B & !!A)))
```

and the `Australia` file, it works as intened.


## Contact
Kevin Chang: tc3149@nyu.edu
