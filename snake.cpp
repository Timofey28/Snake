#include <bits/stdc++.h>
using namespace std;

namespace SNAKE
{
    int system62to10(char);
    char system10to62(int);

    int FIELD_FROM_LEFT,
        FIELD_FROM_UP;

    int width, height;

    struct coord
    {
        int x, y;
        int fieldX, fieldY;
        coord() : x(0), y(0), fieldX(0), fieldY(0) {}
        coord(int _x, int _y) : x (_x * 2 + FIELD_FROM_LEFT), y(_y + FIELD_FROM_UP), fieldX(_x), fieldY(_y) {}
        coord(const coord &c) : x(c.x), y(c.y), fieldX(c.fieldX), fieldY(c.fieldY) {}
        static int getX(int field_x) {return FIELD_FROM_LEFT + field_x * 2;}
        static int getY(int field_y) {return FIELD_FROM_UP + field_y;}
        friend ostream &operator<<(ostream &stream, const coord &c);
        friend istream &operator>>(istream &stream, coord &c);
    };

    char direction;
    int nn_direction; // הכÿ םוינמסועט
    int length;
    vector<char> dirs;
    vector<int> nn_dirs;

    vector<tuple<coord, coord, coord>> game;
    ostream &operator<<(ostream &stream, const coord &c)
    {
        if(c.fieldX && c.fieldY) stream << system10to62(c.fieldX) << system10to62(c.fieldY);
        else stream << '0';
        return stream;
    }
    istream &operator>>(istream &stream, coord &c)
    {
        char in;
        stream >> in;
        c.fieldX = system62to10(in);
        if(c.fieldX) {
            stream >> in;
            c.fieldY = system62to10(in);
        }
        else c.fieldY = 0;
        return stream;
    }

    char system10to62(int num)
    {
        char ans;
        if(num < 10) return num + 48;
        if(num >= 10 && num < 36) return (num - 10) + 'a';
        if(num >= 36 && num < 62) return (num - 36) + 'A';
        return '$';
    }
    int system62to10(char ch)
    {
        if(ch >= '0' && ch <= '9') return ch - 48;
        if(ch >= 'a' && ch <= 'z') return 10 + ch - 'a';
        if(ch >= 'A' && ch <= 'Z') return 36 + ch - 'A';
        return 62;
    }

    pair<int, int> direction_when_crashed;
    bool tooSmallWindow = 0;
    int min_lawn = 5, max_lawn = 60, min_mspf = 30, max_mspf = 150;

    double sigmoid(double x) {return (double) (1/(1 + pow(2.7182818284, -x)));}
}
