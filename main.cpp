#include <bits/stdc++.h>
#include <conio.h>
using namespace std;

vector<vector<char>> v(12, vector<char>(18, ' '));
vector<char> b;
void init();
void make_snake(char &ch, char &direction);
void boardDrawing();

int amount = 0, complexity, record = -1;
double record_area_size = -1;
char choose_size, choose_complexity;

int i_begin_temp, j_begin_temp;
int   i_end_temp,   j_end_temp;

int i_begin, i_end, j_begin, j_end;
bool need_to_add;

int main()
{
    srand(time(0));
    setlocale(0, "");

    int rand_i, rand_j;
    char ch, direction;
    int food_appear = 1;

    ifstream f("record.txt");
    if(f.is_open()) {
        f >> record >> record_area_size;
    }
    f.close();

    again:
    system("cls");
    cout << "\n\n\tВыберите размер поляны:\n\n";
    cout << "\t1 - small\n";
    cout << "\t2 - average\n";
    cout << "\t3 - big\n";
    cout << "\n\t0 - для выхода (+ рекорды сохраняются только так)\n\n\t";
    cin >> choose_size;
    if(choose_size == '1') {
        v.resize(8);
        for(int i = 0; i < v.size(); ++i)
            v[i].resize(12);
    }
    else if(choose_size == '2') {
        v.resize(12);
        for(int i = 0; i < v.size(); ++i)
            v[i].resize(18);
    }
    else if(choose_size == '3') {
        v.resize(18);
        for(int i = 0; i < v.size(); ++i)
            v[i].resize(25);
    }
    else if(choose_size == '0') {
        ofstream f("record.txt");
        if(f.is_open()) {
            f << record << "\n" << record_area_size;
        }
        else cout << "Файл не открылся\n";
        f.close();
        return 0;
    }
    else goto again;

    i_begin_temp = v.size()-3; j_begin_temp = v[0].size()/2;
      i_end_temp = v.size()-2;   j_end_temp = v[0].size()/2;

    again1:

    init();
    make_snake(ch, direction);

    system("cls");
    cout << "\n\n\tВыберите уровень сложности: \n\n";
    cout << "\t1 - изи\n";
    cout << "\t2 - медиум\n";
    cout << "\t3 - хард\n";
    cout << "\n\t5 - шаг назад\n";
    cout << "\t0 - для выхода (+ рекорды сохраняются только так)\n\n\t";
    cin >> choose_complexity;
    if(choose_complexity == '1') complexity = 200;
    else if(choose_complexity == '2') complexity = 100;
    else if(choose_complexity == '3') complexity = 50;
    else if(choose_complexity == '5') goto again;
    else if(choose_complexity == '0') {
        ofstream f("record.txt");
        if(f.is_open()) {
            f << record << "\n" << record_area_size;
        }
        else cout << "Файл не открылся\n";
        f.close();
        return 0;
    }
    else goto again1;

    boardDrawing();
    this_thread::sleep_for(chrono::milliseconds(complexity));

    do {
        while(!kbhit()) {
            if(++food_appear % 20 == 0) {
                do {
                    rand_i = rand();
                    rand_i = rand() % (v.size()-2) + 1;
                    rand_j = rand();
                    rand_j = rand() % (v[0].size()-2) + 1;
                }while(v[rand_i][rand_j] == 'O' || v[rand_i][rand_j] == '*');
                v[rand_i][rand_j] = '*';
            }
            if(ch == 'j') {
                ch = direction;
                cin.ignore();
                cin.get();
            }
            else if(ch != 's' && ch != 'a' && ch != 'd' && ch != 'w') ch = direction;
            go_back:
            if(ch == 's') {
                if(direction == 'w') {
                    ch = 'w';
                    goto go_back;
                }

                if(v[i_begin+1][j_begin] == '*') {
                    ++amount;
                    need_to_add = 1;
                    goto loop_s;
                }
                else if(v[i_begin+1][j_begin] == '#') {
                    if(v[1][j_begin] == '*') {
                        ++amount;
                        need_to_add = 1;
                        goto loop_s;
                    }
                }

                if(b.back() == 's') {
                    if(v[i_end+1][j_end] == '#') {
                        v[i_end][j_end] = ' ';
                        i_end = 1;
                    }
                    else v[i_end++][j_end] = ' ';
                }
                else if(b.back() == 'w') {
                    if(v[i_end-1][j_end] == '#') {
                        v[i_end][j_end] = ' ';
                        i_end = v.size()-2;
                    }
                    else v[i_end--][j_end] = ' ';
                }
                else if(b.back() == 'd') {
                    if(v[i_end][j_end+1] == '#') {
                        v[i_end][j_end] = ' ';
                        j_end = 1;
                    }
                    else v[i_end][j_end++] = ' ';
                }
                else if(b.back() == 'a') {
                    if(v[i_end][j_end-1] == '#') {
                        v[i_end][j_end] = ' ';
                        j_end = v[0].size()-2;
                    }
                    else v[i_end][j_end--] = ' ';
                }

                loop_s:
                if(v[i_begin+1][j_begin] == 'O') {
                    endgame_s:
                    cout << "\n\tврезался лох\n\n\t";
                    if(amount > record) {
                        cout << "\tЗато новый рекорд!\n\n\t";
                        record = amount;
                    }
                    double d = (double) (amount / v.size() / v[0].size() * 100);
                    if(d > record_area_size) {
                        cout << "\tЗато новый рекорд!\n\n\t";
                        record_area_size = d;
                    }
                    cin.ignore();
                    cin.get();
                    goto again1;
                }
                else if(v[i_begin+1][j_begin] == '#') {
                    v[i_begin][j_begin] = 'O';
                    i_begin = 1;
                    if(v[i_begin][j_begin] == 'O') goto endgame_s;
                    v[i_begin][j_begin] = '@';
                    if(need_to_add) {
                        need_to_add = 0;
                        b.push_back(ch);
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                    }
                    else {
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                        b[0] = ch;
                    }
                }
                else {
                    if(need_to_add) {
                        need_to_add = 0;
                        b.push_back(ch);               // добавляем элемент в начало вектора
                        rotate(b.rbegin(), b.rbegin()+1, b.rend()); // с помощью такой штуки
                    }
                    else {
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                        b[0] = ch;
                    }
                    v[i_begin++][j_begin] = 'O';
                    v[i_begin][j_begin] = '@';
                }

            }
            else if(ch == 'a') {
                if(direction == 'd') {
                    ch = 'd';
                    goto go_back;
                }

                if(v[i_begin][j_begin-1] == '*') {
                    ++amount;
                    need_to_add = 1;
                    goto loop_a;
                }
                else if(v[i_begin][j_begin-1] == '#') {
                    if(v[i_begin][v[0].size()-2] == '*') {
                        ++amount;
                        need_to_add = 1;
                        goto loop_a;
                    }
                }

                if(b.back() == 's') {
                    if(v[i_end+1][j_end] == '#') {
                        v[i_end][j_end] = ' ';
                        i_end = 1;
                    }
                    else v[i_end++][j_end] = ' ';
                }
                else if(b.back() == 'w') {
                    if(v[i_end-1][j_end] == '#') {
                        v[i_end][j_end] = ' ';
                        i_end = v.size()-2;
                    }
                    else v[i_end--][j_end] = ' ';
                }
                else if(b.back() == 'd') {
                    if(v[i_end][j_end+1] == '#') {
                        v[i_end][j_end] = ' ';
                        j_end = 1;
                    }
                    else v[i_end][j_end++] = ' ';
                }
                else if(b.back() == 'a') {
                    if(v[i_end][j_end-1] == '#') {
                        v[i_end][j_end] = ' ';
                        j_end = v[0].size()-2;
                    }
                    else v[i_end][j_end--] = ' ';
                }

                loop_a:
                if(v[i_begin][j_begin-1] == 'O') {
                    endgame_a:
                    cout << "\n\tврезался лох\n\n\t";
                    if(amount > record) {
                        cout << "\tЗато новый рекорд!\n\n\t";
                        record = amount;
                    }
                    double d = (double) (amount / v.size() / v[0].size() * 100);
                    if(d > record_area_size) {
                        cout << "\tЗато новый рекорд!\n\n\t";
                        record_area_size = d;
                    }
                    cin.ignore();
                    cin.get();
                    goto again1;
                }
                else if(v[i_begin][j_begin-1] == '#') {
                    v[i_begin][j_begin] = 'O';
                    j_begin = v[0].size()-2;
                    if(v[i_begin][j_begin] == 'O') goto endgame_a;
                    v[i_begin][j_begin] = '@';
                    if(need_to_add) {
                        need_to_add = 0;
                        b.push_back(ch);
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                    }
                    else {
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                        b[0] = ch;
                    }
                }
                else {
                    if(need_to_add) {
                        need_to_add = 0;
                        b.push_back(ch);               // добавляем элемент в начало вектора
                        rotate(b.rbegin(), b.rbegin()+1, b.rend()); // с помощью такой штуки
                    }
                    else {
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                        b[0] = ch;
                    }
                    v[i_begin][j_begin--] = 'O';
                    v[i_begin][j_begin] = '@';
                }

            }
            else if(ch == 'd') {
                if(direction == 'a') {
                    ch = 'a';
                    goto go_back;
                }

                if(v[i_begin][j_begin+1] == '*') {
                    ++amount;
                    need_to_add = 1;
                    goto loop_d;
                }
                else if(v[i_begin][j_begin+1] == '#') {
                    if(v[i_begin][1] == '*') {
                        ++amount;
                        need_to_add = 1;
                        goto loop_d;
                    }
                }

                if(b.back() == 's') {
                    if(v[i_end+1][j_end] == '#') {
                        v[i_end][j_end] = ' ';
                        i_end = 1;
                    }
                    else v[i_end++][j_end] = ' ';
                }
                else if(b.back() == 'w') {
                    if(v[i_end-1][j_end] == '#') {
                        v[i_end][j_end] = ' ';
                        i_end = v.size()-2;
                    }
                    else v[i_end--][j_end] = ' ';
                }
                else if(b.back() == 'd') {
                    if(v[i_end][j_end+1] == '#') {
                        v[i_end][j_end] = ' ';
                        j_end = 1;
                    }
                    else v[i_end][j_end++] = ' ';
                }
                else if(b.back() == 'a') {
                    if(v[i_end][j_end-1] == '#') {
                        v[i_end][j_end] = ' ';
                        j_end = v[0].size()-2;
                    }
                    else v[i_end][j_end--] = ' ';
                }

                loop_d:
                if(v[i_begin][j_begin+1] == 'O') {
                    endgame_d:
                    cout << "\n\tврезался лох\n\n\t";
                    if(amount > record) {
                        cout << "\tЗато новый рекорд!\n\n\t";
                        record = amount;
                    }
                    double d = (double) (amount / v.size() / v[0].size() * 100);
                    if(d > record_area_size) {
                        cout << "\tЗато новый рекорд!\n\n\t";
                        record_area_size = d;
                    }
                    cin.ignore();
                    cin.get();
                    goto again1;
                }
                else if(v[i_begin][j_begin+1] == '#') {
                    v[i_begin][j_begin] = 'O';
                    j_begin = 1;
                    if(v[i_begin][j_begin] == 'O') goto endgame_d;
                    v[i_begin][j_begin] = '@';
                    if(need_to_add) {
                        need_to_add = 0;
                        b.push_back(ch);
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                    }
                    else {
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                        b[0] = ch;
                    }
                }
                else {
                    if(need_to_add) {
                        need_to_add = 0;
                        b.push_back(ch);               // добавляем элемент в начало вектора
                        rotate(b.rbegin(), b.rbegin()+1, b.rend()); // с помощью такой штуки
                    }
                    else {
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                        b[0] = ch;
                    }
                    v[i_begin][j_begin++] = 'O';
                    v[i_begin][j_begin] = '@';
                }

            }
            else if(ch == 'w') {
                if(direction == 's') {
                    ch = 's';
                    goto go_back;
                }

                if(v[i_begin-1][j_begin] == '*') {
                    ++amount;
                    need_to_add = 1;
                    goto loop_w;
                }
                else if(v[i_begin-1][j_begin] == '#') {
                    if(v[v.size()-2][j_begin] == '*') {
                        ++amount;
                        need_to_add = 1;
                        goto loop_w;
                    }
                }

                if(b.back() == 's') {
                    if(v[i_end+1][j_end] == '#') {
                        v[i_end][j_end] = ' ';
                        i_end = 1;
                    }
                    else v[i_end++][j_end] = ' ';
                }
                else if(b.back() == 'w') {
                    if(v[i_end-1][j_end] == '#') {
                        v[i_end][j_end] = ' ';
                        i_end = v.size()-2;
                    }
                    else v[i_end--][j_end] = ' ';
                }
                else if(b.back() == 'd') {
                    if(v[i_end][j_end+1] == '#') {
                        v[i_end][j_end] = ' ';
                        j_end = 1;
                    }
                    else v[i_end][j_end++] = ' ';
                }
                else if(b.back() == 'a') {
                    if(v[i_end][j_end-1] == '#') {
                        v[i_end][j_end] = ' ';
                        j_end = v[0].size()-2;
                    }
                    else v[i_end][j_end--] = ' ';
                }

                loop_w:
                if(v[i_begin-1][j_begin] == 'O') {
                    endgame_w:
                    cout << "\n\tврезался лох\n\n\t";
                    if(amount > record) {
                        cout << "\tЗато новый рекорд!\n\n\t";
                        record = amount;
                    }
                    double d = (double) (amount / v.size() / v[0].size() * 100);
                    if(d > record_area_size) {
                        cout << "\tЗато новый рекорд!\n\n\t";
                        record_area_size = d;
                    }
                    cin.ignore();
                    cin.get();
                    goto again1;
                }
                else if(v[i_begin-1][j_begin] == '#') {
                    v[i_begin][j_begin] = 'O';
                    i_begin = v.size()-2;
                    if(v[i_begin][j_begin] == 'O') goto endgame_w;
                    v[i_begin][j_begin] = '@';
                    if(need_to_add) {
                        need_to_add = 0;
                        b.push_back(ch);
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                    }
                    else {
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                        b[0] = ch;
                    }
                }
                else {
                    if(need_to_add) {
                        need_to_add = 0;
                        b.push_back(ch);               // добавляем элемент в начало вектора
                        rotate(b.rbegin(), b.rbegin()+1, b.rend()); // с помощью такой штуки
                    }
                    else {
                        rotate(b.rbegin(), b.rbegin()+1, b.rend());
                        b[0] = ch;
                    }
                    v[i_begin--][j_begin] = 'O';
                    v[i_begin][j_begin] = '@';
                }
            }

            direction = ch;
            boardDrawing();
            this_thread::sleep_for(chrono::milliseconds(complexity));

        }
        cout << "\t";
        ch = _getche();
        ch = tolower(ch);

    }while(ch != 'k');

    return 0;
}

void init()
{
    for(int i = 1; i < v.size()-1; ++i) {
        for(int j = 1; j < v[0].size()-1; ++j) {
            v[i][j] = ' ';
        }
    }
    for(int i = 0; i < v[0].size(); ++i) {
        v[0][i] = '#';
        v.back()[i] = '#';
    }
    for(int i = 0; i < v.size(); ++i) {
        v[i][0] = '#';
        v[i].back() = '#';
    }
    i_begin = i_begin_temp;
    j_begin = j_begin_temp;
    i_end = i_end_temp;
    j_end = j_end_temp;

    need_to_add = 0;
}

void make_snake(char &ch, char &direction)
{
    if(i_begin != i_end) amount = abs(i_begin - i_end) + 1;
    else amount = abs(j_begin - j_end) + 1;

    if(i_begin == i_end) {
        if(j_end <= j_begin) {
            for(int i = j_end; i < j_begin; ++i) {
                v[i_begin][i] = 'O';
            }
            ch = 'd';
            v[i_begin][j_begin] = '@';
        }
        else {
            ch = 'a';
            v[i_begin][j_begin] = '@';
            for(int i = j_begin+1; i <= j_end; ++i) {
                v[i_begin][i] = 'O';
            }
        }
    }
    else if(j_begin == j_end) {
        if(i_end <= i_begin) {
            for(int i = i_end; i < i_begin; ++i) {
                v[i][j_begin] = 'O';
            }
            ch = 's';
            v[i_begin][j_begin] = '@';
        }
        else {
            ch = 'w';
            v[i_begin][j_begin] = '@';
            for(int i = i_begin+1; i <= i_end; ++i) {
                v[i][j_begin] = 'O';
            }
        }
    }
    direction = ch;
    b.resize(amount-1);
    for(int i = 0; i < b.size(); ++i) {
        b[i] = ch;
    }
}

void boardDrawing()
{
    system("cls");

    if(record == -1) cout << "\n\tФайл не открылся\n";
    cout << "\n";
    for(int i = 0; i < v.size(); ++i) {
        cout << "\n\t" << v[i][0];
        for(int j = 1; j < v[0].size(); ++j) {
            cout << ' ' << v[i][j];
        }
    }

    printf("\n\n\tДлина змейки:%4d", amount);
    printf("     Рекорд:%6d\n", record);
}
