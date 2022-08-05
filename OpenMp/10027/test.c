#include <omp.h>
#include <stdio.h>
 
#define set(v,j) (v |= (1 << j))
#define clear(v,j) (v ^= (1 << j))
#define test(v,j) ((v>>j) & 1)


int sudoku[9][9];
int Num[3][9] = {0}, blockNum[3][3] = {0};
int Blank[81][2] = {0};
int n;

int count = 0;
#pragma omp threadprivate(count)
/* rowColConflict */
int rowColConflict(int try, int row, int col, int sudoku[9][9], int Num[][9])
{
  return test(Num[0][row], try) || test(Num[1][col], try);
}
/* blockConflict */
int blockConflict(int try, int row, int col, int sudoku[9][9], int Num[][9])
{
  
  int blockRow = row / 3;
  int blockCol = col / 3;
 
  int conflict = 0;
  for (int i = 0; i < 3 && !conflict; i++)
    for (int j = 0; j < 3 && !conflict; j++)
      if (sudoku[3 * blockRow + i][3 * blockCol + j] 
      == try)
    conflict = 1;
  return conflict;
}
/* conflict */
int conflict(int try, int row, int col, int sudoku[9][9], int Num[][9])
{
  return (rowColConflict(try, row, col, sudoku, Num) ||
      blockConflict(try, row, col, sudoku, Num));
}
/* placeNumber */
int placeNumber(int n, int sudoku[9][9], int Num[][9])
{
  if (n == 81){
    count ++
    return;
  }
  int row = n / 9;
  int col = n % 9;
  if (sudoku[row][col] != 0)
    return(placeNumber(n + 1, sudoku, Num));
  /* numSolution */
  int numSolution = 0;
  for (int try = 1; try <= 9; try++) {
    if (!conflict(try, row, col, sudoku, Num)) {
      sudoku[row][col] = try;
      set(Num[0][row], try);
      set(Num[1][col], try);
      Num[2][row/3*3+col/3] |= (1 << try);
 
 
      numSolution += placeNumber(n + 1, sudoku, Num);
 
      clear(Num[0][row], try);
      clear(Num[1][col], try);
      Num[2][row/3*3+col/3] ^= (1 <<  try);
    }
  } /* for */
  sudoku[row][col] = 0;
  return numSolution;
}

int bfs(){
    int res = 0;
    #pragma omp parallel 
    {
        #pragma omp single
        {
            placeNumber(0, sudoku, Num)
        }
        #pragma omp critical
    }
}
/* main */
int main(void)
{
 
  int firstZero = -1;

  n = 0;
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      scanf("%d", &(sudoku[i][j]));
      if (sudoku[i][j] == 0 ){
          Blank[n][0] = i;
          Blank[n][1] = j;
          n ++;
      }
      if (sudoku[i][j] != 0){
          set(Num[0][i], sudoku[i][j]);
          set(Num[1][j], sudoku[i][j]);
          Num[2][i/3*3+j/3] |= (1 << sudoku[i][j]);
      }
    }
  /* omp */
  int numSolution = 0;
  int num = bfs();
  return 0;
}
/* end */

