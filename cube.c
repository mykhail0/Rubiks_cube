#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * cube's size.
 */
#ifndef N
#define N 5
#endif

#define NaN (-1)

/**
 * Square's coordinates on a face.
 */
typedef struct {
  int x;  // row
  int y;  // column
} Cell;

typedef enum { U, L, F, R, B, D } Face;
typedef enum { RIGHT, ANTIRIGHT, STRAIGHT } Angle;

// OPPOSITE_WALL[i] is a face opposite to face i.
static const Face OPPOSITE_FACE[] = {D, R, B, L, F, U};

static const char ALLOWED_CHARS[] = {'u',  'l',  'f',  'r', 'b', 'd',
                                     '\'', '\"', '\n', '.', '\0'};

/* Rubik's cube's initialization. */

/**
 * Initialize each element of an array of length N to some value x.
 */
void initializeRow(int arr[], int x) {
  for (int i = 0; i < N; ++i) arr[i] = x;
}

/**
 * Initialize every element of a 2D array to some value x.
 */
void initialize(int arr[][N], int x) {
  for (int i = 0; i < N; ++i) initializeRow(arr[i], x);
}

void initializeCube(int arr[][N][N], int faceCount) {
  for (int i = 0; i < faceCount; ++i) initialize(arr[i], i);
}

/* Face rotations. */
/**
 * Set square with coords of `cell` to `value` in the face `arr`.
 * Returns the value that was set there beforehand.
 */
int insertValue(int arr[][N], Cell cell, int value) {
  int original = arr[cell.x][cell.y];
  arr[cell.x][cell.y] = value;
  return original;
}

/**
 * Rotates coordinates of a cell of a N x N array by `angle` degrees.
 */
void rotateCellCoordinates(Cell* cell, Angle angle) {
  int temp = cell->x;
  if (angle == RIGHT) {
    cell->x = cell->y;
    cell->y = N - 1 - temp;
  }
  if (angle == ANTIRIGHT) {
    cell->x = N - 1 - cell->y;
    cell->y = temp;
  }
  if (angle == STRAIGHT) {
    cell->x = N - 1 - cell->x;
    cell->y = N - 1 - cell->y;
  }
}

/**
 * Rotates a cell with given coordinates by a given angle, shifting over other
 * cells cyclically.
 */
void rotateCell(int arr[][N], Cell cell, Angle angle) {
  int value = arr[cell.x][cell.y];
  if (angle == STRAIGHT) {
    // Do 2 rotations of pairs of cells.
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        rotateCellCoordinates(&cell, angle);
        value = insertValue(arr, cell, value);
      }
      rotateCellCoordinates(&cell, RIGHT);
      value = arr[cell.x][cell.y];
    }
  } else {
    for (int j = 0; j < 4; ++j) {
      rotateCellCoordinates(&cell, angle);
      value = insertValue(arr, cell, value);
    }
  }
}

/**
 * Rotates a face.
 */
void rotateFace(int arr[][N], Angle angle) {
  int n = N - 1;
  for (Cell cell = {0, 0}; cell.x < N / 2; cell.x++, n -= 2) {
    cell.y = cell.x;
    for (; cell.y < cell.x + n; cell.y++) {
      rotateCell(arr, cell, angle);
    }
  }
}

/* Rotation of neighbouring faces. */

// Successor functions.
/* When we rotate a face, we also have to consider rotating neighbouring faces
 * and even the opposite face. So for a given rotation (defined by a face and
 * angle) we need to find a face on which a given square (defined by a face and
 * coordinates) will land. */

/**
 * Array functions as a successor function for a neighbouring face in case of
 * rotating an upper or down face.
 */
static const Face nextUorDarr[3][6] = {
    {NaN, B, L, F, R, NaN}, {NaN, F, R, B, L, NaN}, {NaN, 3, 4, 1, 2, NaN}};
Face nextUorD(Face face, Face UorD, Angle angle) {
  assert(face != U && face != D);
  if (UorD == D && angle != STRAIGHT) angle = 1 - angle;
  return nextUorDarr[angle][face];
}

/**
 * Array functions as a successor function for a neighbouring face in case of
 * rotating a left or right face.
 */
static const Face nextLorRarr[3][6] = {
    {F, NaN, D, NaN, U, B}, {B, NaN, U, NaN, D, F}, {D, NaN, B, NaN, F, U}};
Face nextLorR(Face face, Face LorR, Angle angle) {
  assert(face != L && face != R);
  if (LorR == R && angle != STRAIGHT) angle = 1 - angle;
  return nextLorRarr[angle][face];
}

/**
 * Array functions as a successor function for a neighbouring face in case of
 * rotating a frontal or back face.
 * Differs from 2 above in that cell is rotated differently depending on if it's
 * an F or B rotation. For comfort implementation is the same, just the function
 * is called with different `angle` in rotateForBcell (if `angle` != STRAIGHT,
 * then the angle's sign is changed).
 */
static const Face nextForBarr[3][6] = {
    {R, U, NaN, D, NaN, L}, {L, D, NaN, U, NaN, R}, {D, R, NaN, L, NaN, U}};
Face nextForB(Face face, Angle angle) {
  assert(face != F && face != B);
  return nextForBarr[angle][face];
}

/**
 * Rotates a cell of a neighbouring face of a U or D face, when a U or D face
 * is rotated.
 */
void rotateUorDcell(int cube[][N][N], Face UorD, Face face, Cell cell,
                    Angle angle) {
  int value = cube[face][cell.x][cell.y];
  if (angle == STRAIGHT) {
    // Rotate cells in 2 pairs of faces.
    for (int j = 0; j < 2; ++j) {
      for (int i = 0; i < 2; ++i) {
        face = nextUorD(face, UorD, angle);
        value = insertValue(cube[face], cell, value);
      }
      face = nextUorD(face, UorD, RIGHT);
      value = cube[face][cell.x][cell.y];
    }
  } else {
    for (int i = 0; i < 4; ++i) {
      face = nextUorD(face, UorD, angle);
      value = insertValue(cube[face], cell, value);
    }
  }
}

/**
 * Same as rotateUorDcell.
 */
void rotateLorRcell(int cube[][N][N], Face LorR, Face face, Cell cell,
                    Angle angle) {
  int value = cube[face][cell.x][cell.y];
  if (angle == STRAIGHT) {
    // Rotate 2 pairs of faces.
    for (int j = 0; j < 2; ++j) {
      // Rotate one pair.
      for (int i = 0; i < 2; ++i) {
        if (face == B || nextLorR(face, LorR, angle) == B) {
          rotateCellCoordinates(&cell, STRAIGHT);
          face = nextLorR(face, LorR, angle);
          value = insertValue(cube[face], cell, value);
        } else {
          face = nextLorR(face, LorR, angle);
          value = insertValue(cube[face], cell, value);
        }
      }
      if (face == B || nextLorR(face, LorR, RIGHT) == B)
        rotateCellCoordinates(&cell, STRAIGHT);
      face = nextLorR(face, LorR, RIGHT);
      value = cube[face][cell.x][cell.y];
    }
  } else {
    for (int i = 0; i < 4; ++i) {
      if (face == B || nextLorR(face, LorR, angle) == B) {
        rotateCellCoordinates(&cell, STRAIGHT);
        face = nextLorR(face, LorR, angle);
        value = insertValue(cube[face], cell, value);
      } else {
        face = nextLorR(face, LorR, angle);
        value = insertValue(cube[face], cell, value);
      }
    }
  }
}

/**
 * Same as rotateUorDcell.
 */
void rotateForBcell(int cube[][N][N], Face ForB, Face face, Cell cell,
                    Angle angle) {
  int value = cube[face][cell.x][cell.y];
  if (angle == STRAIGHT) {
    // Rotate 2 pairs of faces.
    for (int j = 0; j < 2; ++j) {
      for (int i = 0; i < 2; ++i) {
        rotateCellCoordinates(&cell, angle);
        face = nextForB(face, angle);
        value = insertValue(cube[face], cell, value);
      }
      face = nextForB(face, RIGHT);
      rotateCellCoordinates(&cell, RIGHT);
      value = cube[face][cell.x][cell.y];
    }
  } else {
    if (ForB == B) angle = 1 - angle;
    for (int i = 0; i < 4; ++i) {
      rotateCellCoordinates(&cell, angle);
      face = nextForB(face, angle);
      value = insertValue(cube[face], cell, value);
    }
  }
}

/**
 * cell - coordinates of a rotated cell
 * face - face on which the rotated cell resides
 */
void rotateNeighbourFaceCell(int cube[][N][N], Face faceRotated, Face face,
                             Cell cell, Angle angle) {
  if (faceRotated == U || faceRotated == D)
    rotateUorDcell(cube, faceRotated, face, cell, angle);
  if (faceRotated == L || faceRotated == R)
    rotateLorRcell(cube, faceRotated, face, cell, angle);
  if (faceRotated == F || faceRotated == B)
    rotateForBcell(cube, faceRotated, face, cell, angle);
}

/**
 * Rotates layers of neighbouring faces.
 */
void rotateNeighbourFace(int cube[][N][N], Face faceRotated, Angle angle,
                         int layers) {
  int face;
  if (faceRotated == U || faceRotated == D) {
    face = 1;
    // 3 separated cases are needed, for 1) upper, 2) down, and 3) other faces.
    if (faceRotated == U) {
      for (Cell cell = {0, 0}; cell.x < layers; cell.x++) {
        for (cell.y = 0; cell.y < N; cell.y++) {
          rotateNeighbourFaceCell(cube, faceRotated, face, cell, angle);
        }
      }
    } else {
      // faceRotated == D
      for (Cell cell = {N - 1, 0}; N - cell.x <= layers; cell.x--) {
        for (cell.y = 0; cell.y < N; cell.y++) {
          rotateNeighbourFaceCell(cube, faceRotated, face, cell, angle);
        }
      }
    }
  } else {
    face = faceRotated + 1;
    if (face == 5) face = 1;
    for (Cell cell = {0, 0}; cell.y < layers; cell.y++) {
      for (cell.x = 0; cell.x < N; cell.x++) {
        rotateNeighbourFaceCell(cube, faceRotated, face, cell, angle);
      }
    }
  }
}

/**
 * Rotates a face by an angle with all changes to face's surroundings it
 * entails.
 */
void rotate(int cube[][N][N], Face face, Angle angle, int layers) {
  rotateFace(cube[face], angle);
  rotateNeighbourFace(cube, face, angle, layers);
  if (layers == N) {
    if (angle != STRAIGHT)
      // when every layer is rotated, the opposite wall is
      // rotated by an opposite angle
      angle = 1 - angle;
    rotateFace(cube[OPPOSITE_FACE[face]], angle);
  }
}

/* Output */

/**
 * Print n spaces.
 */
void nspaces(int n) {
  for (int i = 0; i < n; ++i) putchar(' ');
}

/**
 * Print a row of an array.
 */
void printRow(int arr[]) {
  for (int i = 0; i < N; ++i) printf("%d", arr[i]);
}

/**
 * Prints Up or Down face according to specification
 */
void printUorDFace(int arr[][N]) {
  int i;
  for (i = 0; i < N - 1; ++i) {
    nspaces(N + 1);
    printRow(arr[i]);
    putchar('\n');
  }
  nspaces(N + 1);
  printRow(arr[i]);
}

/**
 * Prints other faces according to specification.
 */
void printFaces(int cube[][N][N]) {
  for (int i = 0; i < N; ++i) {
    int j;
    for (j = 1; j < 4; ++j) {
      printRow(cube[j][i]);
      putchar('|');
    }
    printRow(cube[j][i]);
    putchar('\n');
  }
}

/**
 * Prints Rubik's cube according to specification.
 */
void output(int cube[][N][N]) {
  putchar('\n');
  printUorDFace(cube[0]);
  putchar('\n');
  printFaces(cube);
  printUorDFace(cube[5]);
  putchar('\n');
}

/* Input */
bool isValid(char ch) {
  return isdigit(ch) || strchr(ALLOWED_CHARS, ch) != NULL;
}

char getChar() {
  int ch = getchar();
  char res = (char)ch;

  if (ch == EOF) {
    if (feof(stdin)) {
      res = '.';
    } else if (ferror(stdin)) {
      perror("getchar()");
      exit(EXIT_FAILURE);
    }
  } else if (!isValid(res)) {
    perror(
        "Invalid character, valid input is only new line, .ulfrbd\'\" and "
        "digits.");
    exit(EXIT_FAILURE);
  }

  return res;
}

void input(int cube[][N][N]) {
  for (char ch = getChar(); ch != '.';) {
    if (ch == '\n') {
      output(cube);
      ch = getChar();
    } else {
      // ch is either a digit or in {\, ", u, l, f, r, b, d}
      if (ch == '\\' || ch == '\"') {
        perror("Expected a character for face, not an angle.");
        exit(EXIT_FAILURE);
      }

      Face face;
      switch (ch) {
        case 'u':
          face = U;
          break;
        case 'l':
          face = L;
          break;
        case 'f':
          face = F;
          break;
        case 'r':
          face = R;
          break;
        case 'b':
          face = B;
          break;
        case 'd':
          face = D;
          break;
      }

      ch = getChar();
      int layers = isdigit(ch) ? 0 : 1;
      while (isdigit(ch)) {
        int i = ch - '0';
        layers *= 10;
        layers += i;
        ch = getChar();
      }
      // ch is not a digit but valid

      if (ch == '.') continue;
      Angle angle = RIGHT;
      if (ch != '\n' && (ch == '\'' || ch == '\"')) {
        angle = ch == '\'' ? ANTIRIGHT : STRAIGHT;
        rotate(cube, face, angle, layers);
        ch = getChar();
      } else {
        rotate(cube, face, angle, layers);
      }
    }
  }
}

int main() {
  static int cube[6][N][N];
  initializeCube(cube, 6);
  input(cube);
  return 0;
}
