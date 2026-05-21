#include <stdio.h>

/**
 * Rozmiar kostki
 */
#ifndef N
#define N 5
#endif

/**
 * Współrzędne komórki kwadratowej tablicy (ściany)
 */
typedef struct
{
    int x; //wiersz
    int y; //kolumna
} Cell;

/* INICJALIZACJA KOSTKI RUBIKA */

/** pomocnicza
 * Inicjalizacja wiersza Arr jakiejś tablicy;
 * wiersz ma długość N, inicjalizujemy na wartość x
 */
void initializeRow(int Arr[], int x)
{
    for(int i = 0; i < N; ++i)
        Arr[i] = x;
}

/** pomocnicza
 * Inicjalizacja tablicy dwuwymiarowej Arr
 * o liczbie wierszy N na wartość x
 */
void initialize(int Arr[][N], int x)
{
    for(int i = 0; i < N; ++i)
        initializeRow(Arr[i], x);
}

/**
 * Inicjalizacja tablicy m kwadratowych tablic, które jako zbiory są singletonami. Wartości są od 0 do m
 * W moim programie to będą 6 tablic reprezentujących ściany, i mających wartości
 * odpowiednio od 0 do 5
 */
void initializeCube(int Arr[][N][N], int wallCount)
{
    for(int i = 0; i < wallCount; ++i)
        initialize(Arr[i], i);
}

/* OBRÓT ŚCIAN */
/* obrót ściany */
/** pomocnicza
 * Komórce o współrzędnych cell tablicy Arr przypisuje wartość value.
 * Funkcja przekazuje wartość, która była przypisana komórce przed przypisaniem
 */
int insertValue(int Arr[][N], Cell cell, int value)
{
    int original = Arr[cell.x][cell.y];
    Arr[cell.x][cell.y] = value;
    return original;
}

/** pomocnicza
 * Obraca współrzędne komórki w tablicy N x N o "angle" stopni
 */
void rotateCellCoordinates(Cell *cell, int angle)
{
    int temp = cell->x;
    if(angle == 90)
    {
        cell->x = cell->y;
        cell->y = N - 1 - temp;
    }
    if(angle == - 90)
    {
        cell->x = N - 1 - cell->y;
        cell->y = temp;
    }
    if(angle == 180)
    {
        cell->x = N - 1 - cell->x;
        cell->y = N - 1 - cell->y;
    }
}

/** pomocnicza
 * Obraca komórkę o współrzędnych cell w tablicy N x N o kąt "angle" stopni (z przesunięciem innych komórek)
 */
void rotateCell(int Arr[][N], Cell cell, int angle)
{
    int value = Arr[cell.x][cell.y];
    if(angle != 180)
        for(int j = 0; j < 4; ++j)
        {
            rotateCellCoordinates(&cell, angle);
            value = insertValue(Arr, cell, value);
        }
    else // musimy obrócić najpierw jedną parę komórek, przejść o 90 stopni i obrócić drugą parę komórek
        for(int i = 0; i < 2; ++i)
        {
            for(int j = 0; j < 2; ++j)
            {
                rotateCellCoordinates(&cell, angle);
                value = insertValue(Arr, cell, value);
            }
            rotateCellCoordinates(&cell, 90);
            value = Arr[cell.x][cell.y];
        }
}

/** pomocnicza
 * obraca ścianę
 */
void rotateWall(int Arr[][N], int angle)
{
    int n = N - 1;
    for(Cell cell = {0, 0}; cell.x < N / 2; cell.x++, n -= 2)
    {
        cell.y = cell.x;
        for( ; cell.y < cell.x + n; cell.y++)
        {
            rotateCell(Arr, cell, angle);
        }
    }
}

/* Obrót otoczenia obracanej ściany (obrót sąsiednich ścian) */

// Funkcje następnika ściany
/* Kiedy obracamy ścianę kostki, to obracamy tę ścianę, a także kilka warstw ścian sąsiednich i czasami obracamy ściane przeciwległą
 * Komórki obracanych sąsiednich ścian są podporządkowane pewnym regułom:
 * obracamy współrzędne komórki ściany o pewien kąt i "nakładamy" tę komórkę na nowe współrzędne ale na innej ścianie.
 * Więc przy obrocie górnej czy dolnej ściany (Up or Down, skrót UorD) obracamy tę ścianę, a także komórki na sąsiadujacych ścianach.
 * Jeśli chcemy obrócić komórkę na sąsiedniej ścianie o numerze wall o kąt angle, to musimy wiedzieć na jakiej sąsiedniej ścianie ta komórka wyląduje.
 * Temu służą funkcje następnika z prefiksem next. W danym przypadku to będzie funkcja nextUorD */

/** pomocnicza
 * funkcja następnika dla sąsiednich ścian przy obrocie górnej czy dolnej ścinay
 */
int nextUorD(int wall, int UorD, int angle)
{
    int result = 0;
    if(UorD == 5 && angle != 180) // w przypadku kąta =/= 180 ma znaczenie o obrót jakiej ściany chodzi.
        angle *= - 1;
    if(angle == 90)
    {
        switch(wall)
        {
            case 1:
                result = 4;
                break;
            case 2:
                result = 1;
                break;
            case 3:
                result = 2;
                break;
            case 4:
                result = 3;
                break;
        }
    }
    if(angle == - 90)
    {
        switch(wall)
        {
            case 1:
                result = 2;
                break;
            case 2:
                result = 3;
                break;
            case 3:
                result = 4;
                break;
            case 4:
                result = 1;
                break;
        }
    }
    if(angle == 180)
    {
        switch(wall)
        {
            case 1:
                result = 3;
                break;
            case 2:
                result = 4;
                break;
            case 3:
                result = 1;
                break;
            case 4:
                result = 2;
                break;
        }
    }
    return result;
}

/** pomocnicza
 * funkcja następnika dla sąsiednich ścian przy obrocie lewej czy prawej ścinay
 */
int nextLorR(int wall, int LorR, int angle)
{
    int result = 0;
    if(LorR == 3 && angle != 180) // w przypadku kąta =/= 180 ma znaczenie o obrót jakiej ściany chodzi.
        angle *= - 1;
    if(angle == 90)
    {
        switch(wall)
        {
            case 0:
                result = 2;
                break;
            case 2:
                result = 5;
                break;
            case 5:
                result = 4;
                break;
            case 4:
                result = 0;
                break;
        }
    }
    if(angle == - 90)
    {
        switch(wall)
        {
            case 0:
                result = 4;
                break;
            case 4:
                result = 5;
                break;
            case 5:
                result = 2;
                break;
            case 2:
                result = 0;
                break;
        }
    }
    if(angle == 180)
    {
        switch(wall)
        {
            case 0:
                result = 5;
                break;
            case 4:
                result = 2;
                break;
            case 5:
                result = 0;
                break;
            case 2:
                result = 4;
                break;
        }
    }
    return result;
}

/** pomocnicza
 * funkcja następnika dla sąsiednich ścian przy obrocie przedniej czy tylnej ścinay
 */
int nextForB(int wall, int angle)
{
    int result = 0; /* funkcja nextForB jest wykorzystywana tylko w funkcji rotateForBcell.
    Obrót ForB nieco się różni od innych tym, że inaczej obracamy komórkę w zależności od tego czy to obrót f czy b.
    Dla wygody nextForB jest taki sam dla f i b, natomiast ta funkcja jest wywoływana z innym kątem obrotu dla f i b (jesli angle != 180 dla obrotu b to zmieniamy znak kąta) */
    if(angle == 90)
    {
        switch(wall)
        {
            case 0:
                result = 3;
                break;
            case 3:
                result = 5;
                break;
            case 5:
                result = 1;
                break;
            case 1:
                result = 0;
                break;
        }
    }
    if(angle == - 90)
    {
        switch(wall)
        {
            case 0:
                result = 1;
                break;
            case 1:
                result = 5;
                break;
            case 5:
                result = 3;
                break;
            case 3:
                result = 0;
                break;
        }
    }
    if(angle == 180)
    {
        switch(wall)
        {
            case 0:
                result = 5;
                break;
            case 3:
                result = 1;
                break;
            case 5:
                result = 0;
                break;
            case 1:
                result = 3;
                break;
        }
    }
    return result;
}

/** pomocnicza
 * funkcja obracająca komórkę sąsiedniej od górnej czy dolnej ściany przy obrocie górnej czy dolnej ściany.
 * cell - współrzędne obracanej komórki, wall - ściana, na której znajduje się obracana komórka
 * kolejne dwie pomocnicze działają analogicznie
 */
void rotateUorDcell(int Cube[][N][N], int UorD, int wall, Cell cell, int angle)
{
    int value = Cube[wall][cell.x][cell.y];
    if(angle != 180)
        for(int i = 0; i < 4; ++i)
        {
            wall = nextUorD(wall, UorD, angle);
            value = insertValue(Cube[wall], cell, value);
        }
    else // w przypadku obrotu 180 obracamy komórki w jednej parze ścian, a potem w drugiej parze
        for(int j = 0; j < 2; ++j)
        {
            for(int i = 0; i < 2; ++i)
            {
                wall = nextUorD(wall, UorD, angle);
                value = insertValue(Cube[wall], cell, value);
            }
            wall = nextUorD(wall, UorD, 90); // przechodzimy na inna parę ścian
            value = Cube[wall][cell.x][cell.y];
        }
}

/** pomocnicza
 * jak wyżej
 */
void rotateLorRcell(int Cube[][N][N], int LorR, int wall, Cell cell, int angle)
{
    int value = Cube[wall][cell.x][cell.y];
    if(angle != 180)
        for(int i = 0; i < 4; ++i)
        {
            if(wall == 4 || nextLorR(wall, LorR, angle) == 4) /* kiedy zachodzi ten warunek (ściana jest tylna albo następnik jest ścianą tylną)
            to komórka przechodzi na obróconą o 180 komórkę ściany następnika */
            {
                rotateCellCoordinates(&cell, 180);
                wall = nextLorR(wall, LorR, angle);
                value = insertValue(Cube[wall], cell, value);
            }
            else // w innym przypadku komórka przeskakuje na to samo miejsce co zajmowałą, jedynie że w ścianie następnika
            {
                wall = nextLorR(wall, LorR, angle);
                value = insertValue(Cube[wall], cell, value);
            }
        }
    else // w przypadku obrotu 180 obracamy komórki w jednej parze ścian, a potem w drugiej parze
        for(int j = 0; j < 2; ++j)
        {
            for(int i = 0; i < 2; ++i) //obracamy jedną parę
            {
                if(wall == 4 || nextLorR(wall, LorR, angle) == 4)
                {
                    rotateCellCoordinates(&cell, 180);
                    wall = nextLorR(wall, LorR, angle);
                    value = insertValue(Cube[wall], cell, value);
                }
                else
                {
                    wall = nextLorR(wall, LorR, angle);
                    value = insertValue(Cube[wall], cell, value);
                }
            }
            if(wall == 4 || nextLorR(wall, LorR, 90) == 4) // trzeba przejść na inną parę ścian
                rotateCellCoordinates(&cell, 180);
            wall = nextLorR(wall, LorR, 90);
            value = Cube[wall][cell.x][cell.y];
        }
}

/** pomocnicza
 * jak wyżej
 */
void rotateForBcell(int Cube[][N][N], int ForB, int wall, Cell cell, int angle)
{
    int value = Cube[wall][cell.x][cell.y];
    if(angle != 180) // komórka przechodzi na obróconą o 90 albo - 90 komórkę ściany następnika
    {
        if(ForB == 4)
            angle *= - 1;
        for(int i = 0; i < 4; ++i)
        {
            rotateCellCoordinates(&cell, angle);
            wall = nextForB(wall, angle);
            value = insertValue(Cube[wall], cell, value);
        }
    }
    else // w przypadku obrotu 180 obracamy komórki w jednej parze ścian, a potem w drugiej parze
        for(int j = 0; j < 2; ++j)
        {
            for(int i = 0; i < 2; ++i) // komórka przechodzi na obróconą o 180 komórkę ściany następnika
            {
                rotateCellCoordinates(&cell, angle);
                wall = nextForB(wall, angle);
                value = insertValue(Cube[wall], cell, value);
            }
            wall = nextForB(wall, 90); // teraz musimy to powtórzyć dla dwóch pozostałych sąsiednich ścian
            rotateCellCoordinates(&cell, 90);
            value = Cube[wall][cell.x][cell.y];
        }
}

/** pomocznicza
 * jest połączeniem trzech powyższych funkcji
 * wallRotated - numer ściany, której sąsiadami się zajmujemy
 * cell - współrzędne obracanej komórki
 * wall - ściana, na której znajduje się obracana komórka
 */
void rotateNeighbourWallCell(int Cube[][N][N], int wallRotated, int wall, Cell cell, int angle)
{
    if(wallRotated == 0 || wallRotated == 5)
        rotateUorDcell(Cube, wallRotated, wall, cell, angle);
    if(wallRotated == 1 || wallRotated == 3)
        rotateLorRcell(Cube, wallRotated, wall, cell, angle);
    if(wallRotated == 2 || wallRotated == 4)
        rotateForBcell(Cube, wallRotated, wall, cell, angle);
}

/** pomocnicza
 * obraca warstwy sąsiednich ścian
 */
void rotateNeighbourWall(int Cube[][N][N], int wallRotated, int angle, int layers)
{
    int wall;
    if(wallRotated == 0 || wallRotated == 5)
    {
        wall = 1;
        if(wallRotated == 0) // niestety trzeba rozważyć oddzielnie trzy przypadki: dla ściany dolnej, górnej i ścian bocznych
        {
            for(Cell cell = {0, 0}; cell.x < layers; cell.x++)
            {
                for(cell.y = 0; cell.y < N; cell.y++)
                {
                    rotateNeighbourWallCell(Cube, wallRotated, wall, cell, angle);
                }
            }
        }
        else
        {
            for(Cell cell = {N - 1, 0}; N - cell.x <= layers; cell.x--)
            {
                for(cell.y = 0; cell.y < N; cell.y++)
                {
                    rotateNeighbourWallCell(Cube, wallRotated, wall, cell, angle);
                }
            }
        }
    }
    else // dla ścian bocznych można zaprojektować jedną pętlę
    {
        wall = wallRotated + 1;
        if(wall == 5)
            wall = 1;
        for(Cell cell = {0, 0}; cell.y < layers; cell.y++)
        {
            for(cell.x = 0; cell.x < N; cell.x++)
            {
                rotateNeighbourWallCell(Cube, wallRotated, wall, cell, angle);
            }
        }
    }
}

/** pomocnicza
 * przekazuje numer ściany która nie jest sąsiadem ściany o numerze n
 */
int oppositeWall(int n)
{
    int result = 0;
    switch(n)
    {
        case 0:
            result = 5;
            break;
        case 1:
            result = 3;
            break;
        case 2:
            result = 4;
            break;
        case 3:
            result = 1;
            break;
        case 4:
            result = 2;
            break;
        case 5:
            result = 0;
            break;
    }
    return result;
}

/**
 * obraca ścianę o kąt angle uwzględniając otaczające ją warstwy i ścianę z naprzeciwka
 */
void rotate(int Cube[][N][N], int wallRotated, int angle, int layers)
{
    rotateWall(Cube[wallRotated], angle);
    rotateNeighbourWall(Cube, wallRotated, angle, layers);
    if(layers == N)
    {
        if(angle != 180) //kiedy obracamy wszystkie warstwy to przeciwległa ściana jest obracana o odwrotny kąt
            angle *= - 1;
        rotateWall(Cube[oppositeWall(wallRotated)], angle);
    }
}

/* WYJŚCIE */

/** pomocnicza
 * Drukuje n spacji
 */
void nspaces(int n)
{
    for(int i = 0; i < n; ++i)
        putchar(' ');
}

/** pomocnicza
 * Drukuje wiersz tablicy
 */
void printRow(int Arr[])
{
    for(int i = 0; i < N; ++i)
        printf("%d", Arr[i]);
}

/** pomocnicza
 * Drukuje górną lub dolną tablicę zgodnie z zasadami
 */
void printUorLWall(int Arr[][N])
{
    int i;
    for(i = 0; i < N - 1; ++i)
    {
        nspaces(N + 1);
        printRow(Arr[i]);
        putchar('\n');
    }
    nspaces(N + 1);
    printRow(Arr[i]);
}

/** pomocnicza
 * Drukuje pozostałe tablicy zgodnie z zasadami
 */
void printWalls(int Cube[][N][N])
{
    for(int i = 0; i < N; ++i)
    {
        int j;
        for(j = 1; j < 4; ++j)
        {
            printRow(Cube[j][i]);
            putchar('|');
        }
        printRow(Cube[j][i]);
        putchar('\n');
    }
}

/**
 * Drukuje kostkę Rubika zgodnie z zasadami
 */
void output(int Cube[][N][N]) // jesli bedzie kolejne wywolanie to trzeba spacje pomiedzy wstawic
{
    putchar('\n');
    printUorLWall(Cube[0]);
    putchar('\n');
    printWalls(Cube);
    printUorLWall(Cube[5]);
    putchar('\n');
}

/* WEJŚCIE */

int convertWallandAngle(char ch)
{
    int result = - 1; /* wartość jeśli znak to nie numer ściany ani kąt obrotu,
    najczęściej będę tego używał do wykrycia cyfry (dla obliczenia liczby warstw) */
    switch(ch)
    {
        case 'u':
            result = 0;
            break;
        case 'l':
            result = 1;
            break;
        case 'f':
            result = 2;
            break;
        case 'r':
            result = 3;
            break;
        case 'b':
            result = 4;
            break;
        case 'd':
            result = 5;
            break;
        case '\'':
            result = - 90;
            break;
        case '\"':
            result = 180;
            break;
    }
    return result;
}

void input(int Cube[][N][N], char ch)
{
    if(ch != '\n')
    {
        int wall = convertWallandAngle(ch), layers = 1, angle = 90;
        ch = getchar();
        if(convertWallandAngle(ch) == - 1 && ch != '\n' && ch != '.')
            layers = 0;
        while(convertWallandAngle(ch) == - 1 && ch != '\n' && ch != '.')
        {
            int i = ch - '0';
            layers *= 10;
            layers += i;
            ch = getchar();
        } // napotkałem '\'', '\"', nową ścianę, '\n' albo '.'
        if(ch != '.')
        {
            if(ch == '\n')
            {
                rotate(Cube, wall, angle, layers);
                output(Cube);
                ch = getchar();
                if(ch != '.')
                    input(Cube, ch);
            }
            else
            {
                int newSymbol = convertWallandAngle(ch);
                if(newSymbol == - 90 || newSymbol == 180)
                {
                    angle = newSymbol;
                    rotate(Cube, wall, angle, layers);
                    ch = getchar();
                    if(ch != '.')
                        input(Cube, ch);
                }
                else
                {
                    rotate(Cube, wall, angle, layers);
                    input(Cube, ch);
                }
            }
        }
    }
    else
    {
        output(Cube);
        ch = getchar();
        if(ch != '.')
            input(Cube, ch);
    }
}

int main()
{
    static int Cube[6][N][N];
    initializeCube(Cube, 6);
    char ch = getchar();
    if(ch != '.')
        input(Cube, ch);
    return 0;
}
