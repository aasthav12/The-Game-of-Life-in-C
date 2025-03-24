#include "universe.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// opaque struct for Universe
struct Universe {
    uint32_t rows;
    uint32_t cols;
    bool **grid;
    bool toroidal;
};

// creating the matrix universe
Universe *uv_create(uint32_t rows, uint32_t cols, bool toroidal) {
    Universe *u = (Universe *) calloc(1, sizeof(Universe));
    u->rows = rows;
    u->cols = cols;
    u->toroidal = toroidal;

    u->grid = (bool **) calloc(rows, sizeof(bool *));
    for (uint32_t r = 0; r < rows; r += 1) {
        u->grid[r] = (bool *) calloc(cols, sizeof(bool));
    }

    return u;
}

// free cols then rows
void uv_delete(Universe *u) {
    for (uint32_t c = 0; c < u->cols; c += 1) {
        free(u->grid[c]);
    }
    free(u->grid);
    free(u);
    u = NULL;
}

// return the rows in the universe
uint32_t uv_rows(Universe *u) {
    return u->rows;
}

// return the cols in the universe
uint32_t uv_cols(Universe *u) {
    return u->cols;
}

// set the cell at grid[r][c] to true
void uv_live_cell(Universe *u, uint32_t r, uint32_t c) {
    u->grid[r][c] = true;
}

// set the cell at grid[r][c] to false
void uv_dead_cell(Universe *u, uint32_t r, uint32_t c) {
    u->grid[r][c] = false;
}

// check if the cell is in range, if it isn't return false
bool uv_get_cell(Universe *u, uint32_t r, uint32_t c) {
    if (r > u->rows || c > u->cols) {
        return false;
    }
    return u->grid[r][c];
}

bool uv_populate(Universe *u, FILE *infile) {
    // read in from the infile
    // the first row from infile goes rows cols
    // every other line are the row column pairs that populate the universe
    // if a pair is outside the bounds of the universe return false
    // if all the pairs have populated the universe return true
    uint32_t value = 0;
    uint32_t lines = 0;

    while (fscanf(infile,
               "%d"
               "%d\n",
               &lines, &value)
           > 0) {
        if (lines >= u->rows || value >= u->cols || lines < 0 || value < 0) {
            return false;
        }

        uv_live_cell(u, lines, value);
    }

    return true;
}

uint32_t uv_census(Universe *u, uint32_t r, uint32_t c) {
    // return the number of neighbors a given cell has
    // this account for side by side cells and diagonal cells
    // check if the universe is a torus, if it is make sure to wrap around
    //      using modular arithmetic

    uint32_t count = 0;
    bool check = uv_get_cell(u, r, c);

    // IF UNIVERSE IS NOT TOROIDAL
    if (u->toroidal == false) {
        // check the cells in all 8 directions, W, NW, N, NE, E, SE, S, SW
        for (int i = -1; i <= 1; i++) { // acting as the rows
            for (int j = -1; j <= 1; j++) { // acting as the cols
                // continue statements that serve as border check
                // if the cell being observed has neighbors or itself is on
                //     the border, then it is to be iterated over
                if ((r == 0 && i < 0)) {
                    continue;
                }
                if (i + r >= u->rows) {
                    continue;
                }
                if ((c == 0 && j < 0)) {
                    continue;
                }
                if (j + c >= u->cols) {
                    continue;
                }
                // after all the if conditions relating to border check have been complete, if the neighbor is indeed a cell increment the census count
                if (uv_get_cell(u, i + r, j + c)) {
                    count++;
                }
            }
        }
        // check if the cell being observed is alive and if so decrement it
        //      so that it is not included in the count of its neighbors
        if (check) {
            count--;
        }

        // IF UNIVERSE IS TOROIDAL
    } else {
        // loops to observe cells in all 8 directions
        for (int i = -1; i <= 1; i += 1) {
            for (int j = -1; j <= 1; j += 1) {
                // utilize modular arithmetic to get the neighbor on other side of universe
                if (uv_get_cell(u, r % u->rows, c % u->cols)) {
                    // if neighbor exists increment census count
                    count++;
                }
            }
        }
        // check if the cell being observed is alive and if so decrement it
        //      so that it is not included in the count of its neighbors
        if (check) {
            count--;
        }
    }

    return count;
}

void uv_print(Universe *u, FILE *outfile) {
    // print the universe to outfile
    // any live cells will be marked with an 'o'
    // any dead cells will be marked with a '.'
    // print out a flattened universe regardless of whether or not the universe is
    //    toroidal
    for (uint32_t r = 0; r < u->rows; r++) {
        for (uint32_t c = 0; c < u->cols; c++) {
            if (u->grid[r][c]) {
                fputc('o', outfile);
            } else {
                fputc('.', outfile);
            }
        }
        fputc('\n', outfile);
    }
}

/*
int main(void) {
  uint32_t rows1 = 5;
  uint32_t cols1 = 5;
  bool toroidal1 = false;
  
  Universe *uv = uv_create(rows1,cols1,toroidal1);
  
  
  FILE *infile = fopen("infile.txt", "r"); 
  uv_populate(uv, infile);
  
  
  FILE *outfile = fopen("outfile.txt", "w"); 
  uv_print(uv, outfile);
  
  uv_delete(uv);
  
  fclose(infile);
  fclose(outfile);
  
  uv_delete(uv);
  
  return 0;
}
*/
