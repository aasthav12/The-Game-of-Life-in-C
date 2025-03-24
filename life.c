#include "universe.h"

#include <inttypes.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define OPTIONS "tsn:i:o:"
#define DELAY   50000

int main(int argc, char **argv) {
    int opt = 0;
    bool toroidal = false;
    bool ncurse = true;
    uint32_t gens = 100;
    FILE *infile = stdin;
    FILE *outfile = stdout;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't': toroidal = true; break;
        case 's': ncurse = false; break;
        case 'n': gens = strtoul(optarg, NULL, 10); break;
        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                fprintf(stderr, "Error in opening file\n");
                return -1;
            }
            break;
        case 'o':
            outfile = fopen(optarg, "w");
            if (outfile == NULL) {
                fprintf(stderr, "Error in opening file\n");
                return -1;
            }
            break;
        }
    }

    // read in the row and column from the input file, or from stdin
    uint32_t row_read = 0;
    uint32_t col_read = 0;

    // row is first item in first row of file
    // col is second item in first row of file
    fscanf(infile,
        "%d"
        "%d\n",
        &row_read, &col_read);

    // create two universes using the rows and columns specified
    // flag them as toroidal based on the command line input
    Universe *A = uv_create(row_read, col_read, toroidal);
    Universe *B = uv_create(row_read, col_read, toroidal);

    uv_populate(A, infile);

    // check if uv_popular went through, that the cells are all within the range of the rows and cols
    if (uv_populate(A, infile) == false) {
        printf("Invalid lines within file\n");
        return -1;
    }

    // set up the ncurses screen
    // based off of the ncurses example from the asgn pdf
    initscr();
    curs_set(FALSE);

    for (uint32_t g = 0; g < gens; g++) {
        if (ncurse) {
            clear();
            // display Universe A
            // printed out to resemble the testing file provided
            // o for cell and blank for no cell
            for (uint32_t r = 0; r < row_read; r++) {
                for (uint32_t c = 0; c < col_read; c++) {
                    if (uv_get_cell(A, r, c)) {
                        mvprintw(r, c, "o");
                    } else {
                        mvprintw(r, c, "");
                    }
                }
            }
            refresh();
            usleep(DELAY);
        }

        uint32_t count = 0;
        bool cell_state = false;
        // iterate through the game and perform the rules
        for (uint32_t r = 0; r < row_read; r += 1) {
            for (uint32_t c = 0; c < col_read; c += 1) {
                count = uv_census(A, r, c);
                cell_state = uv_get_cell(A, r, c);
                // if cell has 2 or 3 neighbors and the cell itself is alive then mark cell as alive
                if (cell_state == true && (count == 3 || count == 2)) {
                    uv_live_cell(B, r, c);
                    // if cell is dead but has 3 neighbors bring back to life
                } else if (cell_state == false && count == 3) {
                    uv_live_cell(B, r, c);
                    // base case mark as dead
                } else {
                    uv_dead_cell(B, r, c);
                }
            }
        }

        // now swap the universes using pointer rule once game is finished
        Universe *swap = A;
        A = B;
        B = swap;
    }

    // after game has been complete close the ncurses display
    endwin();

    // print the contents of the complete game to the outfile specified
    uv_print(A, outfile);

    // free up memory by deleting the universes
    uv_delete(A);
    uv_delete(B);

    // close the outfile once contents have gotten printed
    fclose(outfile);
    return 0;
}
