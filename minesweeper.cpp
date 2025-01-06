#include <iostream>
#include <vector>
#include <string>

using std::cin;
using std::cout;
using std::endl;

class GameField {
private:
    int height, width;
    std::vector<std::vector<char>> field;

    void validate_size(int& h, int& w) {
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

    void initialize_field() {
        field.resize(height, std::vector<char>(width, ' '));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 || i == height - 1) field[i][j] = (j == 0 || j == width - 1) ? '+' : '-';
                else if (j == 0 || j == width - 1) field[i][j] = '|';
            }
        }
    }

public:
    GameField(int h = 1, int w = 1) {
        validate_size(h, w);
        height = h + 2;
        width = w + 2;
        initialize_field();
        cout << "New field generated. Size of field: " << h << 'x' << w << endl;
    }

    void show_field() const {
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
                cout << field[i][j];
            }
            cout << endl;
        }
    }
};

int main() {
    int height, width;
    cout << "Input height and width values: ";
    cin >> height >> width;

    system("cls");

    GameField field(height, width);
    field.show_field();

    system("pause");

    return 0;
}
