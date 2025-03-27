#include <iostream>
#include <vector>
#include <random>
#include <time.h>

/*
You are given a locked container represented as a two-dimensional grid of boolean values (true = locked, false = unlocked).
Your task is to write an algorithm that fully unlocks the box, i.e.,
transforms the entire matrix into all false.

Implement the function:
bool openBox(uint32_t y, uint32_t x);
This function should:
    - Use the SecureBox public API (toggle, isLocked, getState).
    - Strategically toggle cells to reach a state where all elements are false.
    - Return true if the box remains locked, false if successfully unlocked.
You are not allowed to path or modify the SecureBox class.

Evaluation Criteria:
    - Functional correctness
    - Computational efficiency
    - Code quality, structure, and comments
    - Algorithmic insight and clarity
*/

class SecureBox
{
private:
    std::vector<std::vector<bool>> box;

public:

    //================================================================================
    // Constructor: SecureBox
    // Description: Initializes the secure box with a given size and 
    //              shuffles its state using a pseudo-random number generator 
    //              seeded with current time.
    //================================================================================
    SecureBox(uint32_t y, uint32_t x) : ySize(y), xSize(x)
    {
        rng.seed(time(0));
        box.resize(y);
        for (auto& it : box)
            it.resize(x);
        shuffle();
    }

    //================================================================================
    // Method: toggle
    // Description: Toggles the state at position (x, y) and also all cells in the
    //              same row above and the same column to the left of it.
    //================================================================================
    void toggle(uint32_t y, uint32_t x)
    {
        box[y][x] = !box[y][x];
        for (uint32_t i = 0; i < xSize; i++)
            box[y][i] = !box[y][i];
        for (uint32_t i = 0; i < ySize; i++)
            box[i][x] = !box[i][x];
    }

    //================================================================================
    // Method: isLocked
    // Description: Returns true if any cell 
    //              in the box is true (locked); false otherwise.
    //================================================================================
    bool isLocked()
    {
        for (uint32_t x = 0; x < xSize; x++)
            for (uint32_t y = 0; y < ySize; y++)
                if (box[y][x])
                    return true;

        return false;
    }

    //================================================================================
    // Method: getState
    // Description: Returns a copy of the current state of the box.
    //================================================================================
    std::vector<std::vector<bool>> getState()
    {
        return box;
    }

private:
    std::mt19937_64 rng;
    uint32_t ySize, xSize;

    //================================================================================
    // Method: shuffle
    // Description: Randomly toggles cells in the box to 
    // create an initial locked state.
    //================================================================================
    void shuffle()
    {
        for (uint32_t t = rng() % 1000; t > 0; t--)
            toggle(rng() % ySize, rng() % xSize);
    }
};
void print(SecureBox& box)
{
    for (int i = 0; i < box.getState().size(); i++)
    {
        for (int j = 0; j < box.getState()[i].size(); j++)
        {
            std::cout << box.getState()[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

//================================================================================
// Function: openBox
// Description: Your task is to implement this function to unlock the SecureBox.
//              Use only the public methods of SecureBox (toggle, getState, isLocked).
//              You must determine the correct sequence of toggle operations to make
//              all values in the box 'false'. The function should return false if
//              the box is successfully unlocked, or true if any cell remains locked.
//================================================================================

bool openBox(uint32_t y, uint32_t x)
{
    SecureBox box(y, x);

    print(box);

    // Початкова матриця state від box
    const auto state = box.getState();
    const int boxSize = y * x;

    // Матриця допустимих конфігурацій для матриці box з розміром boxSize...boxSize + 1
    std::vector<std::vector<int>> matrix(boxSize, std::vector<int>(boxSize + 1, 0));

    /*
    * Формула: sum(a,b) = (i==a | j==b),
    * де допустимі конфігурації дорівнюють L(a,b) = XOR state(i,j);
    */

    // Побудова матриці matrix та запис початкової матриці 
    for (uint32_t i = 0; i < y; i++)
    {
        for (uint32_t j = 0; j < x; j++)
        {

            //Індекс для матриці matrix
            int p = i * x + j;

            //Введення в матрицю matrix початкову матрицю box
            matrix[p][boxSize] = state[i][j] ? 1 : 0;

            // Позначення ефектів toggle, як допустимі конфігурації для клітинок j
            for (uint32_t a = 0; a < y; a++)
            {
                int q = a * x + j; 
                matrix[p][q] = 1;
            }
            // Позначення ефектів toggle, як допустимі конфігурації для клітинок i
            for (uint32_t b = 0; b < x; b++)
            {
                int q = i * x + b;
                matrix[p][q] = 1;
            }
        }
    }

    //Вирішення через Gauss-Jordan елімінації по модулю два.

    int row = 0;
    std::vector<int> index(boxSize, -1);
    for (int col = 0; col < boxSize && row < boxSize; col++)
    {
        int pivot = -1;
        //Знаходження в стовпцях 1, як опорне значення.
        for (int r = row; r < boxSize; r++)
        {
            if (matrix[r][col] == 1)
            {
                pivot = r;
                break;
            }
        }

        if (pivot == -1)
        {
            continue;
        }

        if (pivot != row)
        {
            std::swap(matrix[row], matrix[pivot]);
        }

        index[col] = row;
        // Обнулення стовпець col для рядків
        for (int r = 0; r < boxSize; r++)
        {
            if (r != row && matrix[r][col] == 1)
            {
                for (int c = col; c <= boxSize; c++)
                {
                    //Використовуємо XOR, як віднімання 
                    matrix[r][c] ^= matrix[row][c];
                }
            }
        }
        row++;
    }
    // Перевірка на сумісність системи. Якщо по сусідству 1 в рядку не має одиниць. 
    for (int r = row; r < boxSize; r++)
    {
        if (matrix[r][boxSize] == 1)
        {
            std::cout << "No solution for SecureBox\n";
            print(box);
            return true;
        }
    }
    // Задання значень для вільних зміних 0
    std::vector<int> ans(boxSize, 0);
    for (int col = 0; col < boxSize; col++)
    {
        if (index[col] != -1)
        {
            ans[col] = matrix[index[col]][boxSize];
        }
        else 
        {
            ans[col] = 0;
        }
    }
    // Використання знайденої матриці, заповненою правильною послідовнстю для toggle операцій.
    for (int q = 0; q < boxSize; q++)
    {
        if (ans[q] == 1)
        {
            uint32_t a = q / x;
            uint32_t b = q % x;
            box.toggle(a, b);
        }
    }

    std::cout << "Solvet SecureBox: \n";
    print(box);

    return box.isLocked();
}


int main(int argc, char* argv[])
{
    /*uint32_t y = std::atol(argv[1]);
    uint32_t x = std::atol(argv[2]);*/
    uint32_t y = 10;
    uint32_t x = 10;
    bool state = openBox(y, x);

    if (state)
        std::cout << "BOX: LOCKED!" << std::endl;
    else
        std::cout << "BOX: OPENED!" << std::endl;

    return state;
}

