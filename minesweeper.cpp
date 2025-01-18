#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <ctime>
#include <limits>
#include <fstream>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;
using std::string;
using std::make_shared;

const string alph = "abcdefghijklmnopqrstuvwxyz";
bool saveLastGame = false;
bool savePlayerGame = false;

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
    continueGame,
    retToMenu,
    successSave,
    successLoad,
    saveErrFile,
    unknownCommand,
    lastSaveDoesNotExist,
    playerSaveDoesNotExist,
    lastSaveDeleted,
    playerSaveDeleted
};

struct Cell {
private:
    char outputSymbol;
    bool flag;
    short countNearbyMines;
    short countOfNearbyCells;
    cellType type;

public:

    // Constructors and desctuctors area

    Cell(char outputSymbol = ' ', int countOfNearbyCells = 8, cellType type = dflt) {
        flag = false;
        this->outputSymbol = outputSymbol;
        this->type = type;
        this->countOfNearbyCells = countOfNearbyCells;
        countNearbyMines = 0;
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
        return countOfNearbyCells;
    }

    short getCountOfNearbyMines() {
        return countNearbyMines;
    }

    bool getFlagStatus() {
        return flag;
    }

    // Pointer getters area
};

struct GameField {
private:
    short height, width;
    int countOfMines;
    int closedCells;
    vector<vector<Cell>> field;

    void generateMines() {
        int count = 0;
        srand(time(NULL));

        while (count < countOfMines)
        {
            int x = rand() % ((width - 2) - 1 + 1) + 1,
                y = rand() % ((height - 2) - 1 + 1) + 1;
            
            if(field[y][x].getCellType() != mineFeatured)
            {
                field[y][x].setCellType(mineFeatured);
                field[y][x].setOutputSymbol('M');
                count++;

                for(int a = -1; a <= 1; a++) {
                    for(int b = -1; b <= 1; b++) {
                        if(a == 0 && b == 0) continue;

                        field[y + a][x + b].increaseNearbyMines();
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

    // Pointer getters area

    short* pgetHeight() {
        return &height;
    }

    short* pgetWidth() {
        return &width;
    }

    int* pgetCountOfMines() {
        return &countOfMines;
    }

    int* pgetClosedCells() {
        return &closedCells;
    }

    Cell* pgetCell(int i, int j) {
        return &field[i][j];
    }

    // Other functions

    void CreateField(short h = 9, short w = 9, int countOfMines = 10) {
        validateSize(h, w, countOfMines);
        this->countOfMines = countOfMines;
        height = h + 2;
        width = w + 2;
        initializeField();
        cout << "New field generated. Size of field: " << h << 'x' << w << endl;
        cout << "Amount of mines - " << countOfMines << endl;
    }

    void show_field(bool showMines=false) {
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
                if(field[i][j].getCellType() != mineFeatured)
                    cout << field[i][j].getOutputSymbol();
                else if(showMines || field[i][j].getFlagStatus()) cout << field[i][j].getOutputSymbol();
                else cout << '#';
            }
            cout << endl;
        }
    }

    returnCodes openCell(int x, int y, bool rec=false) {        
        if(!rec) {
            if(x < 1 || x > width - 2 || y < 1 || y > height - 2)
                return errCellDoentExits;
            
            if(field[y][x].getFlagStatus())    
                return warnCellFlagged;

            if(field[y][x].getCellType() == mineFeatured) 
                return endLose;
            
            if(field[y][x].getCellType() == opened)
                return warnCellAlrdOpened;
        }

        field[y][x].setCellType(opened);
        closedCells--;

        field[y][x].setDefaultOutputSymbol();

        if(field[y][x].getCountOfNearbyMines() != 0)
            return continueGame;

        for(int a = -1; a <= 1; a++) {
            for(int b = -1; b <= 1; b++) {
                if(a == 0 && b == 0) continue;
                int n = y + a,
                    k = x + b;
                
                if(n < 1 || n > height - 2 || k < 1 || k > width - 2) continue;

                if(field[n][k].getCellType() == dflt && !field[n][k].getFlagStatus())
                    openCell(k, n, true);
            }
        }

        return continueGame;
    }

    returnCodes flagCell(int x, int y) {
        system("cls");

        if(field[y][x].getCellType() == opened)
            return warnCellAlrdOpened;

        if(field[y][x].getFlagStatus())
            return warnCellFlagged;

        field[y][x].setFlag();
        field[y][x].setOutputSymbol('F');
        return continueGame;
    }

    returnCodes unflagCell(int x, int y) {
        system("cls");

        if(!field[y][x].getFlagStatus())
            return warnCellFlagged;
        
        field[y][x].unflag();
        field[y][x].setDefaultOutputSymbol();
        return continueGame;
    }

    void initializeField(bool isLoad=false) {
        field.resize(height, vector<Cell>(width));

        if(isLoad) return;

        // Initializing main cell parameters
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 || i == height - 1) {
                    field[i][j] = (Cell){((j == 0 || j == width - 1) ? '+', 3, decorative : '-', 5, decorative)};
                }  
                else if (j == 0 || j == width - 1) field[i][j] = (Cell){'|', 5, decorative};
                else field[i][j] = (Cell){'#'};
            }
        }

        generateMines();
    }
};

struct Game {
private:
    GameField gameField;

public:

    void start() {
        system("cls");

        short height, width, amountOfMines;
        cout << "Input height and width values: ";
        cin >> height >> width;
        cout << "Input amount of mines: ";
        cin >> amountOfMines;

        gameField.CreateField(height, width, amountOfMines);
        gameField.show_field();
    }

    returnCodes save(bool lastSave=false) {
        system("cls");

        std::ofstream ofs((lastSave ? "saveL.dat" : "saveP.dat"), std::ios::out | std::ios::binary);

        if(!ofs.is_open()) { // Check the successful opening
            cout << "ERROR: File " << (lastSave ? "\"saveL.dat\"" : "\"saveP.dat\"") << " wasn's opened for writing." << endl << "The program will be aborted!" << endl;
            system("pause");
            return saveErrFile;
        }
        
        // Write game field parameters in the file
        ofs.write((char*)gameField.pgetHeight(), sizeof(short));
        ofs.write((char*)gameField.pgetWidth(), sizeof(short));
        ofs.write((char*)gameField.pgetCountOfMines(), sizeof(int));
        ofs.write((char*)gameField.pgetClosedCells(), sizeof(int));

        // Write parameters of every cell in the file
        for(int i = 0; i < gameField.getHeight(); i++) {
            for(int j = 0; j < gameField.getWidth(); j++) {
                ofs.write((char*)gameField.pgetCell(i, j), sizeof(Cell));
            }
        }   

        ofs.close();

        if(lastSave) {
            saveLastGame = true;
            return successSave;
        }

        gameField.show_field();
        savePlayerGame = true;
        cout << "Game was successfull saved!" << endl;

        return successSave;
    }

    returnCodes load(bool lastSave=false) {
        std::ifstream ifs((lastSave ? "saveL.dat" : "saveP.dat"), std::ios::in | std::ios::binary);

        if(!ifs.is_open()) { // Check the successful opening
            cout << "ERROR: File " << (lastSave ? "\"saveL.dat\"" : "\"saveP.dat\"") << " wasn's opened for reading." << endl;
            system("pause");
            return saveErrFile;
        }

        // Write game field parameters from the file
        ifs.read((char*)gameField.pgetHeight(), sizeof(short));
        ifs.read((char*)gameField.pgetWidth(), sizeof(short));
        ifs.read((char*)gameField.pgetCountOfMines(), sizeof(int));
        ifs.read((char*)gameField.pgetClosedCells(), sizeof(int));

        gameField.initializeField(true);

        for(int i = 0; i < gameField.getHeight(); i++) {
            for(int j = 0; j < gameField.getWidth(); j++) {
                ifs.read((char*)gameField.pgetCell(i, j), sizeof(Cell));
            }
        }

        ifs.close();

        gameField.show_field();
        return successLoad;
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

            save(true);

            system("cls");
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

            save(true);

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

            save(true);

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

        if(command == "save") {
            returnCodes gValue = save();

            switch (gValue)
            {
            case saveErrFile:
                cout << "Game will be closed..." << endl;
                return endGame;
            
            default:
                break;
            }

            return continueGame;
        }

        if(command == "help") {
            system("cls");
            gameField.show_field();

            printf("COMMANDS: \n help - output this menu\n open xy - open cell with xy coordinated\n           (x - horizontal, y - vertical)\n flag xy - flag cell\n unflag xy - unflag cell\n save - save game\n close - close game\n");
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

struct GameMenu {
private:
    Game game;

    void gameplay() {
        while (1)
        {
            returnCodes x = game.readCommand();
            if(x == endGame) break;
        }

        system("pause");
    }

public:
    void startMenu() {

        while (true)
        {
            printf("==|MENU|==\n- newgame\n");

            if(saveLastGame)
                cout << "- loadlast" << endl << "- dellast" << endl;
            
            if(savePlayerGame) 
                cout << "- loadsave" << endl << "- delsave" << endl;

            cout << "- close" << endl;

            returnCodes gValue = commandReaded();

            switch (gValue)
            {
            case unknownCommand:
                system("cls");
                cout << "Unknown command!" << endl;
                break;
            
            case endGame:
                return;

            default:
                system("cls");
                break;
            }

            if(gValue == endGame) break;
        }
        
    }

    returnCodes commandReaded() {
        string command;

        cout << "> ";
        cin >> command;

        if(command == "newgame") {
            game.start();
            gameplay();

            return retToMenu;
        }

        if(command == "loadsave") {
            if(!savePlayerGame)
                return playerSaveDoesNotExist;

            game.load();
            gameplay();
            
            return retToMenu;
        }

        if(command == "loadlast") {
            if(!saveLastGame)
                return lastSaveDoesNotExist;

            game.load(true);
            gameplay();

            return retToMenu;
        }

        if(command == "dellast") {
            saveLastGame = false;
            remove("saveL.dat");
            return lastSaveDeleted;
        }

        if(command == "delsave") {
            savePlayerGame = false;
            remove("saveP.dat");
            return playerSaveDeleted;
        }

        if(command == "close") {
            return endGame;
        }

        return unknownCommand;
    }

};

int main() {
    std::ifstream ifs("savestatus.dat", std::ios::in | std::ios::binary);

    if(ifs.is_open()) {
        ifs.read((char*)&saveLastGame, sizeof(bool));
        ifs.read((char*)&savePlayerGame, sizeof(bool));
    }

    ifs.close();

    GameMenu gm;
    gm.startMenu();

    std::ofstream ofs("savestatus.dat", std::ios::out | std::ios::binary);
    ofs.write((char*)&saveLastGame, sizeof(bool));
    ofs.write((char*)&savePlayerGame, sizeof(bool));
    ofs.close();

    return 0;
}
