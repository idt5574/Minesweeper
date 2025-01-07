#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <ctime>
#include <limits>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;
using std::string;
using std::make_shared;

const string alph = "abcdefghijklmnopqrstuvwxyz";

void clearInput() {
    std::cin.clear(); // Сбрасываем флаги ошибок
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Убираем оставшиеся символы
}

enum cellType {
    dflt = 0,
    mineFeatured = 1,
    decorative = 2,
    opened = 3,
};

struct Cell {
private:
    char outputSymbol;
    bool flag;
    cellType type;
    vector<shared_ptr<Cell>> nearbyCells;

public:

    // Constructors and desctuctors area

    Cell(char outputSymbol = ' ', int countOfNearbyCells = 8, cellType type = dflt) {
        flag = false;
        this->outputSymbol = outputSymbol;
        this->type = type;
        nearbyCells.resize(countOfNearbyCells, nullptr);
    }

    // Setters area

    void setOutputSymbol(char outputSymbol) {
        this->outputSymbol = outputSymbol;
    }

    void setDefaultOutputSymbol() {

        switch (type)
        {
        case dflt:
            outputSymbol = '#';
            break;

        case mineFeatured:
            outputSymbol = 'M';
            break;
        
        case opened:
            outputSymbol = ' ';
            break;
        
        default:
            break;
        }
    }

    void setCellType(cellType type) {
        this->type = type;
    }

    void setNearbyCell(int index, shared_ptr<Cell> pCell) {
        nearbyCells[index] = pCell;
    }   

    void setFlag() {
        flag = true;
    }

    void unflag() {
        flag = false;
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

    shared_ptr<Cell> getNearbyCell(int x) {
        return nearbyCells[x];
    }

    bool getFlagStatus() {
        return flag;
    }

    // Another functions

    int countNearbyMines() {
        int nearbyMinesCounter = 0;

        for(int i = 0; i < nearbyCells.size(); i++) {
            if(nearbyCells[i].get()->getCellType() == mineFeatured)
                nearbyMinesCounter++;
        }

        return nearbyMinesCounter;
    }

    void openCell(int& closedCells) {

        type = opened;
        closedCells--;

        if(countNearbyMines() != 0) {
            outputSymbol = 48 + countNearbyMines();
            return;
        }

        outputSymbol = ' ';

        for(int i = 0; i < nearbyCells.size(); i++) {
            if(nearbyCells[i].get()->getCellType() == dflt && !nearbyCells[i].get()->getFlagStatus())
                nearbyCells[i].get()->openCell(closedCells);
        }
    }
};

struct GameField {
private:
    short height, width;
    int countOfMines;
    int closedCells;
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

        closedCells = (h * w) - countOfMines;
    }

    void initializeField() {
        field.resize(height, vector<shared_ptr<Cell>>(width));

        // Initializing main cell parameters
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 || i == height - 1) {
                    field[i][j] = make_shared<Cell>((j == 0 || j == width - 1) ? '+', 3, decorative : '-', 5, decorative);
                }  
                else if (j == 0 || j == width - 1) field[i][j] = make_shared<Cell>('|', 5, decorative); // '|';
                else field[i][j] = make_shared<Cell>('#');
            }
        }

        // Initializing nearby cells
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {

                int count = 0;

                for(int a = -1; a <= 1; a++) {
                    for(int b = -1; b <= 1; b++) {
                        if(a == 0 && b == 0) continue;
                        int n = i + a,
                            k = j + b;
                        
                        if(n > -1 && n < height && k > -1 && k < width)
                        {
                            field[i][j].get()->setNearbyCell(count, field[n][k]);
                            count++;
                        }
                    }
                } 
            } 
        }

        generateMines();
    }

public:
    // GameField(short h = 1, short w = 1, int countOfMines = 1) {
    //     validateSize(h, w);
    //     this->countOfMines = countOfMines;
    //     height = h + 2;
    //     width = w + 2;
    //     initializeField();
    //     cout << "New field generated. Size of field: " << h << 'x' << w << endl;
    // }

    // Getters area

    short getHeight() {
        return height;
    }

    short getWidth() {
        return width;
    }

    int getCountOfMines() {
        return countOfMines;
    }

    int getClosedCells() {
        return closedCells;
    }

    void CreateField(short h = 1, short w = 1, int countOfMines = 1) {
        this->countOfMines = countOfMines;
        validateSize(h, w);
        height = h + 2;
        width = w + 2;
        initializeField();
        cout << "New field generated. Size of field: " << h << 'x' << w << endl;
    }

    void show_field(bool showMines=false) const {
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
                else cout << '#';
            }
            cout << endl;
        }
    }

    bool openCell(int x, int y) {
        system("cls");
        if(x < 1 || x > width - 2 || y < 1 || y > height - 2)
        {
            cout << "ERROR! Cell " << alph[x - 1] << y << " is doesnt exist!" << endl;
            return false;
        }

        if(field[y][x].get()->getCellType() == mineFeatured) {
            cout << "You lose!" << endl;
            return true;
        }

        if(field[y][x].get()->getCellType() == opened)
        {
            cout << "Cell " << alph[x - 1] << y << " is already opened!" << endl;
            return false;
        }

        if(field[y][x].get()->getFlagStatus()) {
            cout << "Cell " << alph[x - 1] << y << " is flagged!" << endl;
        }

        field[y][x].get()->setCellType(opened);
        closedCells--;

        int curNearbyMinesCount = field[y][x].get()->countNearbyMines();

        if(curNearbyMinesCount != 0)
        {
            field[y][x].get()->setOutputSymbol(curNearbyMinesCount + 48);
            return false;
        }
        
        field[y][x].get()->setOutputSymbol(' ');

        for(int i = 0; i < field[y][x].get()->getCountOfNearbyCells(); i++) {
            shared_ptr<Cell> tempCellPTR {field[y][x].get()->getNearbyCell(i)};
            
            if(tempCellPTR.get()->getCellType() == dflt && !tempCellPTR.get()->getFlagStatus()) {
                tempCellPTR.get()->openCell(closedCells);
            }

            tempCellPTR.reset();
        }

        return false;
    }

    bool flagCell(int x, int y) {
        system("cls");
        if(field[y][x].get()->getFlagStatus())
            return false;
        

        field[y][x].get()->setFlag();
        field[y][x].get()->setOutputSymbol('F');
        return true;
    }

    bool unflagCell(int x, int y) {
        system("cls");
        if(!field[y][x].get()->getFlagStatus())
            return false;
        
        field[y][x].get()->unflag();
        field[y][x].get()->setDefaultOutputSymbol();
        return true;
    }
};

struct game {
private:
    GameField gameField;

public:

    void start() {
        short height, width, amountOfMines;
        cout << "Input height and width values: ";
        cin >> height >> width;
        cout << "Input amount of mines: ";
        cin >> amountOfMines;

        gameField.CreateField(height, width, amountOfMines);
        gameField.show_field();
    }

    bool readCommand() {
        cout << "> ";

        string command;
        cin >> command;

        if(command == "open") {
            bool rValue;
            char symbol;
            int number;

            cin >> symbol >> number;

            rValue = gameField.openCell(symbol - 96, number);
            gameField.show_field();

            if(gameField.getClosedCells() == 0) {
                cout << "You won!" << endl;
                rValue = true;
            }

            return rValue;
        }

        if(command == "flag") {
            char symbol;
            int number;
            bool rValue;

            cin >> symbol >> number;
            rValue = gameField.flagCell(symbol - 96, number);
            gameField.show_field();

            if(rValue)
                cout << "Cell " << symbol << number << " is flagged!" << endl;
            else 
                cout << "Cell " << symbol << number << " is already flagged!" << endl;

            return false;
        }

        if(command == "unflag") {
            char symbol;
            int number;
            bool rValue;

            cin >> symbol >> number;
            rValue = gameField.unflagCell(symbol - 96, number);
            gameField.show_field();

            if(rValue)
                cout << "Cell " << symbol << number << " is unflagged!" << endl;
            else 
                cout << "Cell " << symbol << number << " is already unflagged!" << endl;

            return false;
        }

        if(command == "help") {
            system("cls");
            gameField.show_field();

            printf("COMMANDS: \n help - output this menu\n open xy - open cell with xy coordinated\n           (x - horizontal, y - vertical)\n flag xy - flag cell\n unflag xy - unflag cell\n close - close game\n");
            return false;
        }

        if(command == "close") {
            return true;
        }

        clearInput();
        cout << "Unknown command!" << endl;
        return false;
    }
};

int main() {
    game g;
    g.start();

    while (1)
    {
        bool x = g.readCommand();
        if(x) break;
    }

    system("pause");

    return 0;
}
