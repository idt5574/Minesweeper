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

enum returnCodes {
    errCellDoentExits,
    warnCellAlrdOpened,
    warnCellFlagged,
    endLose,
    goodWin,
    endGame,
    continueGame
};

struct Cell {
private:
    char outputSymbol;
    bool flag;
    short countNearbyMines;
    cellType type;
    vector<shared_ptr<Cell>> nearbyCells;

public:

    // Constructors and desctuctors area

    Cell(char outputSymbol = ' ', int countOfNearbyCells = 8, cellType type = dflt) {
        flag = false;
        this->outputSymbol = outputSymbol;
        this->type = type;
        countNearbyMines = 0;
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

            if(countNearbyMines == 0)
                outputSymbol = ' ';
            else outputSymbol = 48 + countNearbyMines;
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

    void increaseNearbyMines(int x=1) {
        countNearbyMines += x;
    }

    void decreaseNearbyMines(int x=1) {
        countNearbyMines -= x;
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

    short getCountOfNearbyMines() {
        return countNearbyMines;
    }

    shared_ptr<Cell> getNearbyCell(int x) {
        return nearbyCells[x];
    }

    bool getFlagStatus() {
        return flag;
    }

    // Another functions

    void openCell(int& closedCells) {

        type = opened;
        closedCells--;

        setDefaultOutputSymbol();

        if(countNearbyMines != 0) 
            return;

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

                for(int a = -1; a <= 1; a++) {
                    for(int b = -1; b <= 1; b++) {
                        if(a == 0 && b == 0) continue;

                        field[y + a][x + b].get()->increaseNearbyMines();
                    }
                }
            }
        }
    }

    void validateSize(short& h, short& w, int& c) {
        if (h < 9 || w < 9) {
            cout << "WARNING! Minimal game field size - 9x9" << endl;
            h = 9;
            w = 9;
        } else if(h > 26 || w > 26)
        {
            cout << "WARNING! Maximal game field size - 26x26" << endl;
            h = 26;
            w = 26;
        }

        if(c < 10) {
            cout << "WARNING! Minimal amount of mines - 10" << endl;
            c = 10;
        } else if(((double)c / ((double)h * (double)w)) * 100.0 > 30.0) {
            cout << "WARNING! Mines can occupy no more than 30%% of the map.\n";
            c = (int)((h * w) * 0.3);
        }

        closedCells = (h * w) - c;
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

    void CreateField(short h = 9, short w = 9, int countOfMines = 10) {
        validateSize(h, w, countOfMines);
        this->countOfMines = countOfMines;
        height = h + 2;
        width = w + 2;
        initializeField();
        cout << "New field generated. Size of field: " << h << 'x' << w << endl;
        cout << "Amount of mines - " << countOfMines << endl;
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
                else if(showMines || field[i][j].get()->getFlagStatus()) cout << field[i][j].get()->getOutputSymbol();
                else cout << '#';
            }
            cout << endl;
        }
    }

    returnCodes openCell(int x, int y) {
        system("cls");
        if(x < 1 || x > width - 2 || y < 1 || y > height - 2)
            return errCellDoentExits;
        
        if(field[y][x].get()->getFlagStatus())    
            return warnCellFlagged;

        if(field[y][x].get()->getCellType() == mineFeatured) 
            return endLose;
        
        if(field[y][x].get()->getCellType() == opened)
            return warnCellAlrdOpened;

        field[y][x].get()->setCellType(opened);
        closedCells--;

        int curNearbyMinesCount = field[y][x].get()->getCountOfNearbyMines();
        field[y][x].get()->setDefaultOutputSymbol();

        if(curNearbyMinesCount != 0)
            return continueGame;


        for(int i = 0; i < field[y][x].get()->getCountOfNearbyCells(); i++) {
            shared_ptr<Cell> tempCellPTR {field[y][x].get()->getNearbyCell(i)};
            
            if(tempCellPTR.get()->getCellType() == dflt && !tempCellPTR.get()->getFlagStatus()) {
                tempCellPTR.get()->openCell(closedCells);
            }

            tempCellPTR.reset();
        }

        return continueGame;
    }

    returnCodes flagCell(int x, int y) {
        system("cls");

        if(field[y][x].get()->getCellType() == opened)
            return warnCellAlrdOpened;

        if(field[y][x].get()->getFlagStatus())
            return warnCellFlagged;

        field[y][x].get()->setFlag();
        field[y][x].get()->setOutputSymbol('F');
        return continueGame;
    }

    returnCodes unflagCell(int x, int y) {
        system("cls");

        if(!field[y][x].get()->getFlagStatus())
            return warnCellFlagged;
        
        field[y][x].get()->unflag();
        field[y][x].get()->setDefaultOutputSymbol();
        return continueGame;
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

    returnCodes readCommand() {
        cout << "> ";

        string command;
        cin >> command;

        if(command == "open") {
            returnCodes rValue = continueGame;
            returnCodes gValue;
            char symbol;
            int number;

            cin >> symbol >> number;

            gValue = gameField.openCell(symbol - 96, number);
            gameField.show_field();

            switch (gValue)
            {
            case errCellDoentExits:
                cout << "ERROR! Cell " << symbol << number << " is doesnt exist!" << endl;
                break;
            
            case endLose:
                cout << "You lose!" << endl;
                rValue = endGame;
                break;

            case warnCellAlrdOpened:
                cout << "Cell " << symbol << number << " is already opened!" << endl;
                break; 

            case warnCellFlagged:
                cout << "Cell " << symbol << number << " is flagged!" << endl;
                break;
            
            default:
                break;
            }

            if(gameField.getClosedCells() == 0) {
                cout << "You won!" << endl;
                rValue = endGame;
            }

            return rValue;
        }

        if(command == "flag") {
            char symbol;
            int number;
            returnCodes rValue = continueGame;
            returnCodes gValue;

            cin >> symbol >> number;
            gValue = gameField.flagCell(symbol - 96, number);
            gameField.show_field();

            switch (gValue)
            {
            case warnCellFlagged:
                cout << "Cell " << symbol << number << " is already flagged!" << endl;
                break;
            
            case continueGame:
                cout << "Cell " << symbol << number << " is flagged!" << endl;
                break;

            case warnCellAlrdOpened:
                cout << "Cell " << symbol << number << " is already opened!" << endl;

            default:
                break;
            }

            return rValue;
        }

        if(command == "unflag") {
            char symbol;
            int number;
            returnCodes rValue = continueGame;
            returnCodes gValue;

            cin >> symbol >> number;
            gValue = gameField.unflagCell(symbol - 96, number);
            gameField.show_field();

            switch (gValue)
            {
            case warnCellFlagged:
                cout << "Cell " << symbol << number << " is already unflagged!" << endl;
                break;
            
            case continueGame:
                cout << "Cell " << symbol << number << " is unflagged!" << endl;
                break;

            default:
                break;
            }   

            return rValue;
        }

        if(command == "help") {
            system("cls");
            gameField.show_field();

            printf("COMMANDS: \n help - output this menu\n open xy - open cell with xy coordinated\n           (x - horizontal, y - vertical)\n flag xy - flag cell\n unflag xy - unflag cell\n close - close game\n");
            return continueGame;
        }

        if(command == "close") {
            return endGame;
        }

        clearInput();
        cout << "Unknown command!" << endl;
        return continueGame;
    }
};

int main() {
    game g;
    g.start();

    while (1)
    {
        returnCodes x = g.readCommand();
        if(x == endGame) break;
    }

    system("pause");

    return 0;
}
