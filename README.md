# Assignment 4 - The Game of Life

This program is a simulation of the game of life. The game consists of a grid which is filled up with a certain number of live cells, following generations until the game of life is complete. It follows three basic rules, if a cell has 2 or 3 neighbors it survives, if a cell is dead but has three neighbors it comes back to life, and any other cells die. Once the game of life is complete, the screen is printed as well as the graphics via ncurses, which allows for visualization of the game in 'o' and '.' characters, and gets outputted to a file. 

## Building

```
$ make
```

## Running

```
$ ./life [options]
```
**OPTIONS**

**-t** makes universe toroidal <br>
**-s** silences ncurses <br>
**-n** specify the number of generations <br>
**-i** specify the input file; default STDIN <br>
**-o** specify the output file; default STDOUT <br>

## FILES

```
$ life.c : contains the main method allowing for the game of life to take place
```
```
$ universe.c : contains the definitions of all the declared functions necessary for playing the game of life
```
```
$ universe.h : contains the declarations of all functions needed for the game of life
```
```
$ Makefile: compiler for program
```
