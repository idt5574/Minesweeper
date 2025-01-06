#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <ctime>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;
using std::make_shared;

enum cellType {
    dflt = 0,
    mineFeatured = 1,
    decorative = 2
};

struct Cell {
private:
    char outputSymbol;
    cellType type;
    vector<shared_ptr<Cell>> nearbyCells;

public:

    // Constructors and desctuctors area

    Cell(char outputSymbol = ' ', int countOfNearbyCells = 8, cellType type = dflt) {
        this->outputSymbol = outputSymbol;
        this->type = type;
        nearbyCells.resize(countOfNearbyCells, nullptr);
    }

    // Setters area

    void setOutputSymbol(char outputSymbol) {
        this->outputSymbol = outputSymbol;
    }

    void setCellType(cellType type) {
        this->type = type;
    }

    void setNearbyCell(int index, shared_ptr<Cell> pCell) {
        nearbyCells[index] = pCell;
    }   

    // Getters area

    char getOutputSymbol() {
        return outputSymbol;
    }

    cellType getCellType() {
        return type;
    }

    int getCountOfNearbyCells() {
        return nearbyCells.size();
    }

    // Another functions

    int countNearbyMines() {
        int nearbyMinesCounter = 0;

        for(int i = 0; i < nearbyCells.size(); i++) {
            if(nearbyCells[i].get()->type = mineFeatured)
                nearbyMinesCounter++;
        }

        return nearbyMinesCounter;
    }

};

struct GameField {
private:
    short height, width;
    int countOfMines;
    vector<vector<shared_ptr<Cell>>> field;

    void generateMines() {
        int count = 0;
        srand(time(NULL));

        while (count < countOfMines)
        {
            int x = rand() % ((width - 2) - 1 + 1) + 1,
                y = rand() % ((height - 2) - 1 + 1) + 1;
            
            if(field[y][x].get()->getCellType() != mineFeatured)
            {
                field[y][x].get()->setCellType(mineFeatured);
                field[y][x].get()->setOutputSymbol('M');
                count++;
            }
        }
    }

    void validateSize(short& h, short& w) {
        if (h < 1 || w < 1) {
            cout << "WARNING! Minimal game field size - 1x1" << endl;
            h = 1;
            w = 1;
        } else if(h > 26 || w > 26)
        {
            cout << "WARNING! Maximal game field size - 26x26" << endl;
            h = 26;
            w = 26;
        }
    }

    void initializeField() {
        field.resize(height, vector<shared_ptr<Cell>>(width));

        // Initializing main cell parameters
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 || i == height - 1) {
                    field[i][j] = make_shared<Cell>((j == 0 || j == width - 1) ? '+', 3, decorative : '-', 4, decorative);
                }  
                else if (j == 0 || j == width - 1) field[i][j] = make_shared<Cell>('|', 5, decorative); // '|';
                else field[i][j] = make_shared<Cell>();
            }
        }

        // Initializing nearby cells
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {

                int count = 0;

                for(int a = -1; a <= 1; a++) {
                    for(int b = -1; b <= 1; b++) {
                        int n = i + a,
                            k = j + b;
                        
                        if(0 <= n > height && 0 <= k > width)
                        {
                            field[i][j].get()->setNearbyCell(count, field[n][k]);
                        }
                    }
                } 
            } 
        }

        generateMines();
    }

public:
    GameField(short h = 1, short w = 1, int countOfMines = 1) {
        validateSize(h, w);
        this->countOfMines = countOfMines;
        height = h + 2;
        width = w + 2;
        initializeField();
        cout << "New field generated. Size of field: " << h << 'x' << w << endl;
    }

    void show_field(bool showMines=false) const {
        const std::string alph = "abcdefghijklmnopqrstuvwxyz";
        cout << "   ";
        for (int j = 1; j < width - 1; j++) {
            cout << (j - 1 < alph.size() ? alph[j - 1] : ' ');
        }
        cout << endl;

        for (int i = 0; i < height; i++) {
            if (i > 0 && i < height - 1) {
                cout << (i < 10 ? " " : "") << i;
            } else {
                cout << "  ";
            }

            for (int j = 0; j < width; j++) {
                if(field[i][j].get()->getCellType() != mineFeatured)
                    cout << field[i][j].get()->getOutputSymbol();
                else if(showMines) cout << field[i][j].get()->getOutputSymbol();
                else cout << ' ';
            }
            cout << endl;
        }
    }
};

int main() {
    short height, width, amountOfMines;
    cout << "Input height and width values: ";
    cin >> height >> width;
    cout << "Input amount of mines: ";
    cin >> amountOfMines;

    system("cls");

    GameField field(height, width, amountOfMines);
    field.show_field(true);

    system("pause");

    return 0;
}
