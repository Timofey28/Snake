#include <bits/stdc++.h>
#include <Windows.h>
#include <conio.h>
#include "snake.cpp"
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
using namespace std;
using namespace SNAKE;

int FIELD_SIZE = 24,
    MSPF       = 80; // время одного кадра в ms
bool can_go_through_walls = 1;
int headX, headY, tailX, tailY;
vector<vector<char>> field;
coord head, tail;

/*
    FIELD_SIZE, MSPF, length
    vector<tuple<coord, coord, coord>> game
*/

mt19937 mersenne(static_cast<unsigned int>(time(0)));
void setPosition(int, int);
void setPosition(const coord &);
void setSnakeBody(int color = 255);
void unsetSnakeBody();
void init(int fsz = FIELD_SIZE, bool b = 0);
char getChar(int, int);
void play();
void save();
void reproduce();
void remov();
void change();

bool passClear(int, int, int);
int passClear_cycle_counter = 0;

int main()
{
    setlocale(0, "");

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO structCursorInfo;
    char choice;
    do {
        GetConsoleCursorInfo(hConsole, &structCursorInfo);
        structCursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &structCursorInfo);
        system("cls");
        cout << "\n\t1 - Играть";
        cout << "\n\t2 - Сохранить";
        cout << "\n\t3 - Воспроизвести";
        if(can_go_through_walls) cout << "\n\t4 - П";
        else cout << "\n\t4 - Не п";
        cout << "роходит сквозь стены";
        cout << "\n\t5 - Удалить сохранение";
        cout << "\n\t7 - Изменить параметры (текущий размер - " << FIELD_SIZE << " x " << FIELD_SIZE;
        cout << ", время на кадр - " << MSPF << " ms)";
        cout << "\n\n\t0 - Выход\n\n\t";
        cin.sync();
        choice = _getche();
        switch(choice)
        {
            case '0': break;
            case '1': play();
                      break;
            case '2': save();
                      break;
            case '3': reproduce();
                      break;
            case '4': can_go_through_walls ^= 1;
                      break;
            case '5': remov();
                      break;
            case '7': change();
                      break;
        }
    }while(choice != '0');
    GetConsoleCursorInfo(hConsole, &structCursorInfo);
    structCursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &structCursorInfo);
    system("cls");

    return 0;
}

void change()
{
    int h;
    HANDLE hWndConsole;
    if(hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
            h = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
        else exit(2);
    }
    else exit(2);
    again:
    system("cls");
    cout << "\n\tВведите новые параметры (размер поля + скорость):\n";
    cout << "\n\tТекущая вместимость, исходя из высоты окна: " << h - 6;
    cout << "\n\tТекущий размер поляны:                      " << FIELD_SIZE;
    cout << "\n\tТекущая скорость (время в ms на кадр):      " << MSPF;
    cout << "\n\n\tПредельные значения для размера поляны: от " << min_lawn << " до " << max_lawn;
    cout << "\n\tПредельные значения для скорости (время в ms на кадр): от " << min_mspf << " до " << max_mspf;
    cout << "\n\n\tЧтобы изменить только размер поля, нужно ввести одно число:\n\t";
    cout << "Например, при вводе \"15\" размер поля станет 15 х 15, а скорость останется прежней";
    cout << "\n\n\tЧтобы изменить только скорость, нужно ввести \"0\" и затем скорость без пробела:\n\t";
    cout << "Например, при вводе \"090\" время на кадр станет 90 ms, в то время как размер поляны останется прежним";
    setPosition(58, 1);
    string s;
    getline(cin, s);
    bool valid = 1;
    while(s[0] == ' ') s.erase(0, 1);
    while(s.back() == ' ') s.pop_back();
    int l = 1;
    while(l < s.size()) {
        if(s[l] == ' ' && s[l] == s[l-1] || s[l] == ' ' && s[l-1] == '+' || s[l] == ' ' && s[l+1] == '+') {
            s.erase(l--, 1);
        }
        l++;
    }
    if(s == "") return;
    for(int i = 0; i < s.size(); ++i)
        if(!isdigit(s[i]) && !isspace(s[i]) && s[i] != '+') {
            valid = 0;
            break;
        }
    if(!valid) {
        setPosition(0, 16);
        cout << "\tНеверный формат, повторите попытку => (enter) => ";
        cin.sync();
        cin.get();
        goto again;
    }
    int k, m = MSPF;
    if(s[0] == '0') {
        s.erase(0, 1);
        m = atoi(s.c_str());
        if(m >= min_mspf && m <= max_mspf) MSPF = m;
        return;
    }
    if(s.find(" ") != string::npos) {

        k = atoi(s.substr(0, s.find(' ')).c_str());
        m = atoi(s.substr(s.find(' ') + 1).c_str());
    }
    else if(s.find("+") != string::npos) {
        k = atoi(s.substr(0, s.find('+')).c_str());
        m = atoi(s.substr(s.find('+') + 1).c_str());
    }
    else k = atoi(s.c_str());
    if(m < min_mspf || m > max_mspf) m = MSPF;
    if(k < min_lawn) {
        setPosition(0, 16);
        cout << "\tСлишком мало, повторите попытку => (enter) => ";
        cin.sync();
        cin.get();
        goto again;
    }
    else if(k > max_lawn) {
        setPosition(0, 16);
        cout << "\tМаксимальный размер поля 60 х 60, повторите попытку => (enter) => ";
        cin.sync();
        cin.get();
        goto again;
    }
    FIELD_SIZE = k;
    MSPF = m;
}

void remov()
{
    ask_again:
    system("cls");
    cout << "\n\tВведите название файла: ";
    string folder, name, s;
    if(can_go_through_walls) folder = "Savings_CanGoThroughWalls";
    else folder = "Savings_Cannot";
    vector<string> names;
    char c[100];
    cin.getline(c, 100);
    OemToCharA(c, c);
    name = c;
    while(name.back() == ' ') name.pop_back();
    while(name[0] == ' ') name.erase(0, 1);
    int l = 1;
    while(l < name.size()) {
        if(name[l] == ' ' && name[l] == name[l-1]) {
            name.erase(l--, 1);
        }
        l++;
    }
    DIR *dir;
    dirent *entry;
    dir = opendir(folder.c_str());
    while((entry = readdir(dir)) != nullptr) {
        s = entry->d_name;
        if(s == "." || s == "..") continue;
        s.erase(s.size() - 4);
        for(int i = 0; i < s.size(); ++i) {
            if(s[i] == '_') s[i] = ' ';
            else if(s[i] == '-') s[i] = '(';
            else if(s[i] == '=') s[i] = ')';
        }
        names.push_back(s);
    }
    closedir(dir);
    if(name == "") return;
    if(name == "dir" || name == "DIR" || name == "вшк" || name == "ВШК") {
        for(int i = 0; i < names.size(); ++i) cout << "\n\t" << i + 1 << ") " << names[i];
        cout << "\n\n\t";
        while(1) {
            foo:
            cin.sync();
            getline(cin, s);
            while(s.back() == ' ') s.pop_back();
            while(s[0] == ' ') s.erase(0, 1);
            if(s == "") return;
            if(s == "back" || s == "BACK" || s == "Ёдб«") goto ask_again;
            for(int i = 0; i < s.size(); ++i) {
                if(!isdigit(s[i])) {
                    cout << "\tНеверный формат, повторите попытку => ";
                    goto foo;
                }
            }
            int k = atoi(s.c_str());
            if(k < 1 || k > names.size()) {
                cout << "\tТакого номера нет, повторите попытку => ";
                goto foo;
            }
            else {
                name = names[k-1];
                break;
            }
        }
    }
    vector<string> last_part(names.size());
    for(int i = 0; i < names.size(); ++i) {
        if(names[i].find("(") != string::npos) {
            last_part[i] = names[i].substr(names[i].find("(") - 1);
            names[i].erase(names[i].find("(") - 1);
        }
    }
    if(name.find("(") != string::npos) name.erase(name.find("(") - 1);

    if(name[0] == '\\') name.erase(0, 1);
    if(find(names.begin(), names.end(), name) != names.end()) {
        int k = 0;
        while(names[k] != name) k++;
        name = folder + '/' + name + last_part[k] + ".txt";
        for(int i = 0; i < name.size(); ++i) {
            if(name[i] == ' ') name[i] = '_';
            else if(name[i] == '(') name[i] = '-';
            else if(name[i] == ')') name[i] = '=';
        }
        remove(name.c_str());
    }
}

void reproduce()
{
    string name;
    vector<string> names;
    char c[50], ch;
    DIR *dir;
    dirent *entry;
    string folder;
    if(can_go_through_walls) folder = "Savings_CanGoThroughWalls";
    else folder = "Savings_Cannot";
    dir = opendir(folder.c_str());
    if(!dir) {
        perror("opendir");
        exit(1);
    }
    while((entry = readdir(dir)) != nullptr) {
        name = entry->d_name;
        if(name == "." || name == "..") continue;
        name.erase(name.size() - 4);
        for(int i = 0; i < name.size(); ++i) {
            if(name[i] == '-') name[i] = '(';
            else if(name[i] == '=') name[i] = ')';
            else if(name[i] == '_') name[i] = ' ';
        }
        names.push_back(name);
    }
    closedir(dir);
    vector<string> original_names(names);
    if(!names.size()) {
        system("cls");
        cout << "\n\tНет сохраненных игр\n\n\t";
        cin.sync();
        cin.get();
        return;
    }
    ask_name:
    system("cls");
    cout << "\n\tВведите название игры: ";
    cout << "\n\n\tdir    - просмотр всех игр";
    cout << "\n\tchange - изменить название сохраненной игры";
    setPosition(31, 1);
    got:
    names = original_names;
    cin.sync();
    cin.getline(c, 50);
    OemToCharA(c, c);
    name = c;
    while(name.back() == ' ') name.pop_back();
    while(name[0] == ' ') name.erase(0, 1);
    int l = 1;
    while(l < name.size()) {
        if(name[l] == ' ' && name[l] == name[l-1]) {
            name.erase(l--, 1);
        }
        l++;
    }
    if(name == "") return;
    if(name == "dir" || name == "DIR" || name == "вшк" || name == "ВШК") {
        system("cls");
        for(int i = 0; i < names.size(); ++i) cout << "\n\t" << i + 1 << ") " << names[i];
        cout << "\n\n\t";
        while(1) {
            foo:
            cin.sync();
            cin.getline(c, 50);
            OemToCharA(c, c);
            name = c;
            while(name.back() == ' ') name.pop_back();
            while(name[0] == ' ') name.erase(0, 1);
            if(name == "") return;
            if(name == "back" || name == "BACK" || name == "ифсл" || name == "ИФСЛ") goto ask_name;
            else if(name == "change" || name == "CHANGE" || name == "срфтпу" || name == "СРФТПУ") {
                for(int i = 0; i < names.size(); ++i) {
                    for(int j = 0; j < names[i].size(); ++j) {
                        if(names[i][j] == ' ') names[i][j] = '_';
                        else if(names[i][j] == '(') names[i][j] = '-';
                        else if(names[i][j] == ')') names[i][j] = '=';
                    }
                }
                goto to_change;
            }
            for(int i = 0; i < name.size(); ++i) {
                if(!isdigit(name[i])) {
                    cout << "\tНеверный формат, повторите попытку => ";
                    goto foo;
                }
            }
            int k = atoi(name.c_str());
            if(k < 1 || k > names.size()) {
                cout << "\tТакого номера нет, повторите попытку => ";
                goto foo;
            }
            else {
                name = names[k-1];
                break;
            }
        }
    }
    else if(name == "change" || name == "CHANGE" || name == "срфтпу" || name == "СРФТПУ") {
        for(int i = 0; i < names.size(); ++i) {
            for(int j = 0; j < names[i].size(); ++j) {
                if(names[i][j] == ' ') names[i][j] = '_';
                else if(names[i][j] == '(') names[i][j] = '-';
                else if(names[i][j] == ')') names[i][j] = '=';
            }
        }
        to_change:
        int k;
        string s;
        do {
            system("cls");
            cout << "\n\tFree to change\n";
            for(int i = 0; i < names.size(); ++i) {
                s = names[i];
                for(int j = 0; j < s.size(); ++j) {
                    if(s[j] == '_') s[j] = ' ';
                    else if(s[j] == '-') s[j] = '(';
                    else if(s[j] == '=') s[j] = ')';
                }
                cout << "\n\t" << i + 1 << ") " << s;
            }
            cout << "\n\n\t";
            ch = _getche();
            k = ch - 48;
            if(ch == 13) return;
        }while(k < 1 || k > names.size());
        name = names[k-1];
        string last;
        if(name.find("-") != string::npos) last = name.substr(name.find("-") - 1);
        if(last.find("another") != string::npos) last.erase(last.find("another") - 1);
        if(can_go_through_walls) s = "Savings_CanGoThroughWalls/" + name + ".txt";
        else s = "Savings_Cannot/" + name + ".txt";
        ifstream in(s);
        for(int i = 0; i < name.size(); ++i)
            if(name[i] == '_') name[i] = ' ';
        if(name.find("-") != string::npos) name.erase(name.find("-") - 1);
        cout << "\n\tВведите новое имя файла \"" << name << "\": ";
        cin.getline(c, 50);
        OemToCharA(c, c);
        name = c;
        while(name.back() == ' ') name.pop_back();
        while(name[0] == ' ') name.erase(0, 1);
        l = 1;
        while(l < name.size()) {
            if(name[l] == ' ' && name[l] == name[l-1]) {
                name.erase(l--, 1);
            }
            l++;
        }
        if(name == "") return;
        for(int i = 0; i < name.size(); ++i)
            if(name[i] == ' ') name[i] = '_';
        name += last;
        string copy_name = names[k - 1];
        if(copy_name.find("another") != string::npos) copy_name.erase(copy_name.find("another") - 1);
        if(name == copy_name) goto to_change;
        copy_name = name;
        int j = 1;
        while(find(names.begin(), names.end(), name) != names.end()) {
            name = copy_name + "_another_" + to_string(j++);
        }
        names[k-1] = name;
        if(can_go_through_walls) name = "Savings_CanGoThroughWalls/" + name + ".txt";
        else name = "Savings_Cannot/" + name + ".txt";
        ofstream out(name);
        while(in.peek() != '\n') {
            ch = in.get();
            out << ch;
        }
        while(in.peek() != ' ') {
            ch = in.get();
            out << ch;
        }
        for(int i = 0; i < 3; ++i) {
            ch = in.get();
            out << ch;
        }
        in.close();
        out.close();
        remove(s.c_str());
        goto to_change;
    }
    vector<string> last_part(names.size());
    ifstream fin;
    if(name.find("(") == string::npos) {
        for(int i = 0; i < names.size(); ++i) {
            if(names[i].find("(") != string::npos) {
                last_part[i] = names[i].substr(names[i].find("(") - 1);
                names[i].erase(names[i].find("(") - 1);
            }
        }
        if(find(names.begin(), names.end(), name) == names.end()) {
            system("cls");
            cout << "\n\tТакой игры (" << name << ") нет, повторите попытку => ";
            goto got;
        }
        else if(count(names.begin(), names.end(), name) > 1) {
            system("cls");
            cout << "\n\tИгр с таким названием несколько, выберите нужный вариант:\n";
            int j = 1;
            vector<string> same_names;
            for(int i = 0; i < names.size(); ++i) {
                string s = names[i];
                if(s == name) {
                    s += last_part[i];
                    cout << "\n\t" << j++ << ") " << s;
                    same_names.push_back(s);
                }
            }
            cout << "\n\n\t";
            do {
                ch = _getche();
                if(ch == 13) goto ask_name;
            }while((ch - 48) < 1 || (ch - 48) > same_names.size());
            name = same_names[ch - 49];
        }
        else {
            int k = 0;
            while(names[k] != name) k++;
            name += last_part[k];
        }
    }
    for(int i = 0; i < name.size(); ++i) {
        if(name[i] == ' ') name[i] = '_';
        else if(name[i] == '(') name[i] = '-';
        else if(name[i] == ')') name[i] = '=';
    }
    fin.open(folder + '/' + name + ".txt");
    if(name.find("-") == string::npos) {
        system("cls");
        cout << "\n\tИгра не запустится, так как в названии файла нет дефиса";
        cout << " (а это типа открывающаяся скобка и она должна быть везде). Выберите другой вариант, нажав enter => ";
        cin.sync();
        cin.get();
        goto ask_name;
    }
    cout << "\n";
    int len, field_size, mspf, lenFromStart;
    fin >> field_size >> mspf >> len;
    int starting_mspf = mspf, spaces_amount, starting_len = len;
    if(len > 1000) spaces_amount = 3;
    else if(len > 100) spaces_amount = 2;
    else if(len > 10) spaces_amount = 1;
    else spaces_amount = 0;
    game.clear();
    coord h, t, f, last_head, last_tail;
    while(fin.peek() != ' ') {
        fin >> h >> t >> f;
        game.push_back(make_tuple(h, t, f));
    }
    char temp1, temp2;
    fin >> temp1 >> temp2;
    direction_when_crashed.first = system62to10(temp1);
    direction_when_crashed.second = system62to10(temp2);

    head = get<0>(game[0]);
    tail = get<1>(game[0]);
    f    = get<2>(game[0]);
    if(head.fieldX != tail.fieldX) lenFromStart = abs(head.fieldX - tail.fieldX) + 1;
    else lenFromStart = abs(head.fieldY - tail.fieldY) + 1;
    init(field_size, 1);
    if(tooSmallWindow) {
        tooSmallWindow = 0;
        system("cls");
        cout << "\n\tМало места!\n\tНе хватает " << FIELD_FROM_LEFT << " строк";
        if(FIELD_FROM_LEFT % 10 == 1) cout << "и\n\t";
        else cout << "\n\t";
        cin.sync();
        cin.get();
        return;
    }
    setSnakeBody(14);
    cout << "Длина змеи: " << lenFromStart << " из " << len << "        Время на кадр: " << mspf << " ms";
    unsetSnakeBody();
    bool first_apple = 1;
    stack<int> backwards;
    backwards.push(mspf);
    cin.sync();
    int x, y, longitude = 4, foodX, foodY;
    bool return_to_pause = 0, reveal_apple = 0;
    int i = 1;
    bool reversed = 0;
    while(i < game.size()) {
        if(i == 0 && reversed) {
            longitude = 4;
            lenFromStart = 2;
            goto beginning;
        }
        last_head = head;
        head = get<0>(game[i]);
        f    = get<2>(game[i]);

        longitude++;
        if(longitude == 2) {
            setPosition(foodX, foodY);
            setSnakeBody(0);
            cout << "  ";
            unsetSnakeBody();
            setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
            foodX = coord::getX(last_head.fieldX);
            foodY = coord::getY(last_head.fieldY);
        }
        else if(longitude == 3) {
            setPosition(foodX, foodY);
            setSnakeBody(4 * 17);
            cout << "ff";
            setSnakeBody(14);
            setPosition(FIELD_FROM_LEFT + 12, field_size + FIELD_FROM_UP + 3);
            if(!first_apple) {
                cout << --lenFromStart;
                if(lenFromStart == 9) cout << " из " << len << ' ';
                else if(lenFromStart == 99) cout << " из " << len << ' ';
            }
            unsetSnakeBody();
            setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
        }
        if(f.fieldX) {
            if(reversed) {
                setPosition(foodX, foodY);
                setSnakeBody(0);
                cout << "  ";
                longitude = 0;
                unsetSnakeBody();
                setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
            }
            else {
                foodX = coord::getX(f.fieldX);
                foodY = coord::getY(f.fieldY);
                setPosition(foodX, foodY);
                setSnakeBody(4 * 17);
                cout << "ff";
                unsetSnakeBody();
                setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
            }
        }
        setSnakeBody();
        x = coord::getX(head.fieldX);
        y = coord::getY(head.fieldY);
        if(!reversed) {
            if(head.fieldX < last_head.fieldX && !(head.fieldX == 1 && last_head.fieldX == field_size)) {
                setPosition(x + 1, y);
                cout << "z";
                setPosition(x, y);
                cout << "z";
            }
            else {
                setPosition(x, y);
                cout << "zz";
            }
        }
        else {
            setPosition(coord::getX(last_head.fieldX), coord::getY(last_head.fieldY));
            setSnakeBody(0);
            cout << "  ";
        }
        unsetSnakeBody();
        setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
        if(!reversed) {
            if(!f.fieldX || first_apple) {
                if(f.fieldX) first_apple = 0;
                setPosition(coord::getX(tail.fieldX), coord::getY(tail.fieldY));
                cout << "  ";
                setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
            }
            else {
                setSnakeBody(14);
                setPosition(FIELD_FROM_LEFT + 12, field_size + FIELD_FROM_UP + 3);
                cout << ++lenFromStart;
                if(lenFromStart == 10) cout << " из " << len;
                else if(lenFromStart == 100) cout << " из " << len;
                setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
                unsetSnakeBody();
            }
        }
        last_tail = tail;
        tail = get<1>(game[i]);
        if(reversed) {
            setSnakeBody();
            if(coord::getX(last_tail.fieldX) > coord::getX(tail.fieldX)) {
                setPosition(coord::getX(tail.fieldX) + 1, coord::getY(tail.fieldY));
                cout << "z";
                setPosition(coord::getX(tail.fieldX), coord::getY(tail.fieldY));
                cout << "z";
            }
            else {
                setPosition(coord::getX(tail.fieldX), coord::getY(tail.fieldY));
                cout << "zz";
            }
            unsetSnakeBody();
            setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
        }
        if(kbhit()) {
            beginning:
            setSnakeBody(0);
            ch = _getche();
            if(ch == 32) {
                setSnakeBody(159);
                setPosition(x, y);
                if(head.fieldY > last_head.fieldY && !reversed ||
                   head.fieldY < last_head.fieldY && reversed) cout << "..";
                else if(head.fieldY < last_head.fieldY && !reversed ||
                        head.fieldY > last_head.fieldY && reversed) {
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << (wchar_t) 0x02D9 << (wchar_t) 0x02D9;
                    _setmode(_fileno(stdout), _O_TEXT);
                }
                else if(head.fieldX > last_head.fieldX && !reversed ||
                        head.fieldX < last_head.fieldX && reversed) cout << " :";
                else if(head.fieldX < last_head.fieldX && !reversed ||
                        head.fieldX > last_head.fieldX && reversed) cout << ": ";
                setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
                setSnakeBody(0);
                return_to_pause = 0;
                while(1) {
                    backToPause:
                    ch = _getche();
                    if(ch == 32 || ch == 13) {
                        setPosition(x, y);
                        setSnakeBody(255);
                        cout << "zz";
                        unsetSnakeBody();
                        break;
                    }
                    if(ch == 8 || ch == 27) break;
                    if(ch == -32) {
                        return_to_pause = 1;
                        break;
                    }
                }
                if(ch == 8 || ch == 27) {
                    unsetSnakeBody();
                    return;
                }
            }
            if(ch == -32) {
                ch = _getche();
                if(ch == 75 && i < game.size() - 20) {
                    if(backwards.top() > starting_mspf) {
                        backwards.push(mspf);
                        double d = mspf * 1.5;
                        mspf = round(d);
                    }
                    else if(backwards.top() < starting_mspf) {
                        mspf = backwards.top();
                        backwards.pop();
                    }
                    else {
                        if(mspf >= starting_mspf) {
                            backwards.push(mspf);
                            double d = mspf * 1.5;
                            mspf = round(d);
                        }
                        else if(mspf < starting_mspf) {
                            mspf = starting_mspf;
                            backwards.pop();
                        }
                    }
                    setPosition(FIELD_FROM_LEFT + 41 + spaces_amount, field_size + FIELD_FROM_UP + 3);
                    setSnakeBody(14);
                    cout << mspf << " ms     ";
                    setSnakeBody(0);
                    setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
                }
                else if(ch == 77 && i < game.size() - 20) {
                    if(mspf == 1) {
                        if(return_to_pause) {
                            return_to_pause = 0;
                            goto backToPause;
                        }
                        else continue;
                    }
                    if(backwards.top() < starting_mspf) {
                        backwards.push(mspf);
                        double d = mspf / 1.5;
                        mspf = round(d);
                    }
                    else if(backwards.top() > starting_mspf) {
                        mspf = backwards.top();
                        backwards.pop();
                    }
                    else {
                        if(mspf <= starting_mspf) {
                            backwards.push(mspf);
                            double d = mspf / 1.5;
                            mspf = round(d);
                        }
                        else if(mspf > starting_mspf) {
                            mspf = starting_mspf;
                            backwards.pop();
                        }
                    }
                    setPosition(FIELD_FROM_LEFT + 41 + spaces_amount, field_size + FIELD_FROM_UP + 3);
                    setSnakeBody(14);
                    cout << mspf << " ms     ";
                    setSnakeBody(0);
                    setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
                }
                else if(ch == 72 && reversed) {
                    reversed = 0;
                    if(can_go_through_walls) setSnakeBody(170);
                    else setSnakeBody(6 * 17);
                    setPosition(FIELD_FROM_LEFT, FIELD_FROM_UP);
                    for(int i = 0; i < (field_size + 2) * 2; ++i) cout << '#';
                    for(int i = 0; i <= field_size; ++i) {
                        setPosition(FIELD_FROM_LEFT, FIELD_FROM_UP + i + 1);
                        cout << "##";
                        setPosition(FIELD_FROM_LEFT + field_size * 2 + 2, FIELD_FROM_UP + i + 1);
                        cout << "##";
                    }
                    setPosition(FIELD_FROM_LEFT, FIELD_FROM_UP + field_size + 1);
                    for(int i = 0; i < (field_size + 2) * 2; ++i) cout << '#';
                    setSnakeBody(0);
                    setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
                }
                else if(ch == 80 && !reversed) {
                    reversed = 1;
                    setSnakeBody(1);
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wchar_t wch = 0x2593;
                    setPosition(FIELD_FROM_LEFT, FIELD_FROM_UP);
                    for(int i = 0; i < (field_size + 2) * 2; ++i) wcout << wch;
                    for(int i = 0; i <= field_size; ++i) {
                        setPosition(FIELD_FROM_LEFT, FIELD_FROM_UP + i + 1);
                        wcout << wch << wch;
                        setPosition(FIELD_FROM_LEFT + field_size * 2 + 2, FIELD_FROM_UP + i + 1);
                        wcout << wch << wch;
                    }
                    setPosition(FIELD_FROM_LEFT, FIELD_FROM_UP + field_size + 1);
                    for(int i = 0; i < (field_size + 2) * 2; ++i) wcout << wch;
                    _setmode(_fileno(stdout), _O_TEXT);
                    setSnakeBody(0);
                    setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
                }
                if(return_to_pause) {
                    return_to_pause = 0;
                    goto backToPause;
                }
            }
            else if(ch == 8 || ch == 27) {
                unsetSnakeBody();
                return;
            }
            unsetSnakeBody();
            if(i == 0 && reversed) goto beginning;
        }
        if(i >= game.size() - 20 && mspf != starting_mspf) {
            mspf = starting_mspf;
            setPosition(FIELD_FROM_LEFT + 43 + spaces_amount, field_size + FIELD_FROM_UP + 3);
            setSnakeBody(14);
            cout << mspf << " ms)     ";
            unsetSnakeBody();
            setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
        }
        if(reversed) i--;
        else i++;
        this_thread::sleep_for(chrono::milliseconds(mspf));
    }
    setSnakeBody(159);
    setPosition(coord::getX(head.fieldX), coord::getY(head.fieldY));
    int crX = direction_when_crashed.first, crY = direction_when_crashed.second;
    if(crY > head.fieldY) cout << "..";
    else if(crY < head.fieldY) {
        _setmode(_fileno(stdout), _O_U16TEXT);
        wcout << (wchar_t) 0x02D9 << (wchar_t) 0x02D9;
        _setmode(_fileno(stdout), _O_TEXT);
    }
    else if(crX > head.fieldX) cout << " :";
    else if(crX < head.fieldX) cout << ": ";
    setPosition(FIELD_FROM_LEFT, field_size + FIELD_FROM_UP + 2);
    setSnakeBody(0);
    cin.sync();
    cin.get();
    unsetSnakeBody();
    game.clear();
}

void save()
{
    step_back:
    system("cls");
    if(!game.size()) {
        cout << "\n\tПока нечего сохранять\n\n\t";
        cin.sync();
        cin.get();
        return;
    }
    string folder;
    if(can_go_through_walls) folder = "Savings_CanGoThroughWalls";
    else folder = "Savings_Cannot";
    struct _stat buf;
    if(_stat(folder.c_str(), &buf)) {
        cout << "\n\tСоздается папка...";
        this_thread::sleep_for(1500ms);
        mkdir(folder.c_str());
    }
    string name, s;
    cout << "\n\tВведите название игры: ";
    cin.sync();
    char c[100];
    cin.getline(c, 100);
    OemToCharA(c, c);
    name = c;
    while(name.back() == ' ') name.pop_back();
    while(name[0] == ' ') name.erase(0, 1);
    int l = 1;
    while(l < name.size()) {
        if(name[l] == ' ' && name[l] == name[l-1]) {
            name.erase(l--, 1);
        }
        l++;
    }
    if(name == "back" || name == "BACK" || name == "ифсл" || name == "ИФСЛ") return;
    for(int i = 0; i < name.size(); ++i)
        if(name[i] == ' ') name[i] = '_';
    folder += '/';
    if(name == "") {
        DIR *dir;
        dirent *entry;
        dir = opendir(folder.c_str());
        if(!dir) {
            perror("opendir");
            exit(1);
        }
        vector<string> names;
        while((entry = readdir(dir)) != nullptr) {
            s = entry->d_name;
            if(s == "." || s == "..") continue;
            s.erase(s.size() - 4);
            names.push_back(s);
        }
        closedir(dir);
        name = to_string(length) + "_-" + to_string(FIELD_SIZE) + "+=";
        if(find(names.begin(), names.end(), name) != names.end()) {
            int k = 1;
            s = name;
            do name = s + "_another_" + to_string(k++);
            while(find(names.begin(), names.end(), name) != names.end());
        }
    }
    else name += "_-" + to_string(FIELD_SIZE) + "+=";
    if(name == "back" || name == "BACK" || name == "ифсл" || name == "ИФСЛ") return;
    if(name[0] == '\\') name.erase(0, 1);
    ifstream fin(folder + '/' + name + ".txt");
    if(fin.is_open()) {
        cout << "\n\tДанное имя уже существует. Заменить содержимое файла? ";
        cin.sync();
        getline(cin, s);
        fin.close();
        if(s != "d" && s != "ў" && s != "D" && s != "‚") goto step_back;
    }
    ofstream fout(folder + '/' + name + ".txt");
    if(!fout.is_open()) {
        cout << "\nfile did not open\n";
        exit(1);
    }
    fout << FIELD_SIZE << ' ' << MSPF << ' ' << length << '\n';
    for(auto [x, y, f] : game) {
        fout << x << y << f;
    }
    fout << ' ' << system10to62(direction_when_crashed.first) << system10to62(direction_when_crashed.second);
    fout.close();
}

void play()
{
    cin.sync();
    init();
    if(tooSmallWindow) {
        tooSmallWindow = 0;
        system("cls");
        cout << "\n\tМало места!\n\tНе хватает " << FIELD_FROM_LEFT << " строк";
        if(FIELD_FROM_LEFT % 10 == 1 && FIELD_FROM_LEFT != 11) cout << "и\n\t";
        else cout << "\n\t";
        cin.sync();
        cin.get();
        return;
    }
    int foodX, foodY;
    coord last_tail = tail;
    game.clear();
    bool need_food = 1, just_appeared = 0;
    char ch;
    while(1) {
        if(just_appeared) {
            game.push_back({head, tail, (coord) {foodX, foodY}});
            just_appeared = 0;
        }
        else game.push_back({head, tail, (coord) {0, 0}});

        if(kbhit()) {
            setSnakeBody(0);
            ch = _getche();
            if(ch == -32) {
                ch = _getche();
                if(ch == 72 && direction != 'd') direction = 'u';
                else if(ch == 75 && direction != 'r') direction = 'l';
                else if(ch == 77 && direction != 'l') direction = 'r';
                else if(ch == 80 && direction != 'u') direction = 'd';
            }
            else if((ch == 119 || ch == 87 || ch == -106 || ch == -26) && direction != 'd') direction = 'u';
            else if((ch == 97 || ch == 65 || ch == -108 || ch == -28) && direction != 'r') direction = 'l';
            else if((ch == 100 || ch == 68 || ch == -126 || ch == -94) && direction != 'l') direction = 'r';
            else if((ch == 115 || ch == 83 || ch == -101 || ch == -21) && direction != 'u') direction = 'd';
            else if(ch == 32) {
                setSnakeBody(14);
                setPosition(FIELD_FROM_LEFT, FIELD_SIZE + FIELD_FROM_UP + 3);
                cout << "Текущая длина змеи: " << length;
                setSnakeBody(159);
                setPosition(head);
                if(direction == 'd') cout << "..";
                else if(direction == 'u') {
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << (wchar_t) 0x02D9 << (wchar_t) 0x02D9;
                    _setmode(_fileno(stdout), _O_TEXT);
                }
                else if(direction == 'r') cout << " :";
                else if(direction == 'l') cout << ": ";
                setPosition(FIELD_FROM_LEFT, FIELD_SIZE + FIELD_FROM_UP + 3);
                setSnakeBody(0);
                while(1) {
                    ch = _getche();
                    if(ch == 32 || ch == 13) {
                        setPosition(head);
                        setSnakeBody(255);
                        cout << "zz";
                        setSnakeBody(0);
                        setPosition(FIELD_FROM_LEFT, FIELD_SIZE + FIELD_FROM_UP + 3);
                        cout << "                                 ";
                        unsetSnakeBody();
                        break;
                    }
                    if(ch == 8 || ch == 27) break;
                }
            }
            if(ch == 8 || ch == 27) {
                if(direction == 'u') {
                    setPosition(head);
                    setSnakeBody(159);
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << (wchar_t) 0x02D9 << (wchar_t) 0x02D9;
                    _setmode(_fileno(stdout), _O_TEXT);
                    direction_when_crashed = {head.fieldX, head.fieldY - 1};
                }
                else if(direction == 'd') {
                    setPosition(head);
                    setSnakeBody(159);
                    cout << "..";
                    direction_when_crashed = {head.fieldX, head.fieldY + 1};
                }
                else if(direction == 'l') {
                    setPosition(head);
                    setSnakeBody(159);
                    cout << ": ";
                    direction_when_crashed = {head.fieldX - 1, head.fieldY};
                }
                else if(direction == 'r') {
                    setPosition(head);
                    setSnakeBody(159);
                    cout << " :";
                    direction_when_crashed = {head.fieldX + 1, head.fieldY};
                }
                unsetSnakeBody();
                break;
            }
            unsetSnakeBody();
        }

        if(need_food) {
            just_appeared = 1;
            need_food = 0;
            do {
                foodX = mersenne() % FIELD_SIZE + 1;
                foodY = mersenne() % FIELD_SIZE + 1;
            }while(getChar(coord::getX(foodX), coord::getY(foodY)) == 'z' ||
                   last_tail.fieldX == foodX && last_tail.fieldY == foodY);
            setPosition(coord::getX(foodX), coord::getY(foodY));
            setSnakeBody(68);
            cout << "ff";
            unsetSnakeBody();
            setPosition(FIELD_FROM_LEFT, FIELD_SIZE + FIELD_FROM_UP + 3);
        }

        if(direction == 'l') {
            head.fieldX--;
            head.x -= 2;
        }
        else if(direction == 'r') {
            head.fieldX++;
            head.x += 2;
        }
        else if(direction == 'u') {
            head.fieldY--;
            head.y--;
        }
        else if(direction == 'd') {
            head.fieldY++;
            head.y++;
        }

        if(head.fieldX == 0 || head.fieldX == FIELD_SIZE + 1 ||
           head.fieldY == 0 || head.fieldY == FIELD_SIZE + 1) {
            if(can_go_through_walls) {
                if(head.fieldX == 0) {
                    head.fieldX = FIELD_SIZE;
                    head.x = coord::getX(FIELD_SIZE);
                }
                else if(head.fieldX == FIELD_SIZE + 1) {
                    head.fieldX = 1;
                    head.x = coord::getX(1);
                }
                else if(head.fieldY == 0) {
                    head.fieldY = FIELD_SIZE;
                    head.y = coord::getY(FIELD_SIZE);
                }
                else if(head.fieldY == FIELD_SIZE + 1) {
                    head.fieldY = 1;
                    head.y = coord::getY(1);
                }
            }
            else {
                direction_when_crashed = {head.fieldX, head.fieldY};
                if(direction == 'u') {
                    head.fieldY++;
                    head.y++;
                    setPosition(head);
                    setSnakeBody(159);
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << (wchar_t) 0x02D9 << (wchar_t) 0x02D9;
                    _setmode(_fileno(stdout), _O_TEXT);
                }
                else if(direction == 'd') {
                    head.fieldY--;
                    head.y--;
                    setPosition(head);
                    setSnakeBody(159);
                    cout << "..";
                }
                else if(direction == 'l') {
                    head.fieldX++;
                    head.x += 2;
                    setPosition(head);
                    setSnakeBody(159);
                    cout << ": ";
                }
                else if(direction == 'r') {
                    head.fieldX--;
                    head.x -= 2;
                    setPosition(head);
                    setSnakeBody(159);
                    cout << " :";
                }
                break;
            }
        }

        if(getChar(head.x, head.y) == 'z') {
            if(direction == 'u') {
                if(head.fieldY == FIELD_SIZE) {
                    head.fieldY = 1;
                    head.y = coord::getY(1);
                    direction_when_crashed = {head.fieldX, 0};
                }
                else {
                    direction_when_crashed = {head.fieldX, head.fieldY};
                    head.fieldY++;
                    head.y++;
                }
                setPosition(head);
                setSnakeBody(159);
                _setmode(_fileno(stdout), _O_U16TEXT);
                wcout << (wchar_t) 0x02D9 << (wchar_t) 0x02D9;
                _setmode(_fileno(stdout), _O_TEXT);
            }
            else if(direction == 'd') {
                if(head.fieldY == 1) {
                    head.fieldY = FIELD_SIZE;
                    head.y = coord::getY(FIELD_SIZE);
                    direction_when_crashed = {head.fieldX, FIELD_SIZE + 1};
                }
                else {
                    direction_when_crashed = {head.fieldX, head.fieldY};
                    head.fieldY--;
                    head.y--;
                }
                setPosition(head);
                setSnakeBody(159);
                cout << "..";
            }
            else if(direction == 'l') {
                if(head.fieldX == FIELD_SIZE) {
                    head.fieldX = 1;
                    head.x = coord::getX(1);
                    direction_when_crashed = {0, head.fieldY};
                }
                else {
                    direction_when_crashed = {head.fieldX, head.fieldY};
                    head.fieldX++;
                    head.x += 2;
                }
                setPosition(head);
                setSnakeBody(159);
                cout << ": ";
            }
            else if(direction == 'r') {
                if(head.fieldX == 1) {
                    head.fieldX = FIELD_SIZE;
                    head.x = coord::getX(FIELD_SIZE);
                    direction_when_crashed = {FIELD_SIZE + 1, head.fieldY};
                }
                else {
                    direction_when_crashed = {head.fieldX, head.fieldY};
                    head.fieldX--;
                    head.x -= 2;
                }
                setPosition(head);
                setSnakeBody(159);
                cout << " :";
            }
            break;
        }
        if(getChar(head.x, head.y) == 'f') {
            need_food = 1;
            length++;
            foodX = 0;
            foodY = 0;
        }

        setSnakeBody();
        if(direction == 'l') {
            setPosition(head.x + 1, head.y);
            cout << "z";
            setPosition(head);
            cout << "z";
        }
        else {
            setPosition(head);
            cout << "zz";
        }
        unsetSnakeBody();
        setPosition(tail);
        cout << "  ";
        setPosition(FIELD_FROM_LEFT, FIELD_SIZE + FIELD_FROM_UP + 3);

        if(!need_food) {
            last_tail = tail;
            if(dirs[0] == 'l') {
                if(tail.fieldX - 1 == 0) {
                    tail.fieldX = FIELD_SIZE;
                    tail.x = coord::getX(FIELD_SIZE);
                }
                else {
                    tail.x -= 2;
                    tail.fieldX--;
                }
            }
            else if(dirs[0] == 'r') {
                if(tail.fieldX + 1 == FIELD_SIZE + 1) {
                    tail.fieldX = 1;
                    tail.x = coord::getX(1);
                }
                else {
                    tail.x += 2;
                    tail.fieldX++;
                }
            }
            else if(dirs[0] == 'u') {
                if(tail.fieldY - 1 == 0) {
                    tail.fieldY = FIELD_SIZE;
                    tail.y = coord::getY(FIELD_SIZE);
                }
                else {
                    tail.y--;
                    tail.fieldY--;
                }
            }
            else if(dirs[0] == 'd') {
                if(tail.fieldY + 1 == FIELD_SIZE + 1) {
                    tail.fieldY = 1;
                    tail.y = coord::getY(1);
                }
                else {
                    tail.y++;
                    tail.fieldY++;
                }
            }
            dirs.erase(dirs.begin());
        }
        dirs.push_back(direction);
        this_thread::sleep_for(chrono::milliseconds(MSPF));
    }
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(hConsole, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &structCursorInfo);
    setSnakeBody(0);
    setPosition(FIELD_FROM_LEFT, FIELD_SIZE + FIELD_FROM_UP + 3);
    cout << "                                 ";
    setSnakeBody(14);
    setPosition(FIELD_FROM_LEFT, FIELD_SIZE + FIELD_FROM_UP + 3);
    cout << "Длина змеи: " << length;
    setSnakeBody(0);
    cin.sync();
    cin.get();
    unsetSnakeBody();
}

char getChar(int x, int y)
{
    char buff[3];
    DWORD cbRead = 0;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    ReadConsoleOutputCharacter(hCon, buff, 2, pos, &cbRead);
    return buff[0];
}

void init(int fsz, bool b)
{
    field.clear();
    field.resize(fsz + 2, vector<char>(fsz * 2 + 4, ' '));
    dirs.clear();
    nn_dirs.clear();
    for(int i = 0; i < fsz * 2 + 4; ++i) {
        field[0][i] = '#';
        field.back()[i] = '#';
    }
    for(int i = 0; i < fsz + 2; ++i) {
        field[i][0] = '#';
        field[i][1] = '#';
        field[i][field[0].size()-2] = '#';
        field[i].back() = '#';
    }
    HANDLE hWndConsole;
    if(hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
        {
            width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
            height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
        }
        else exit(2);
    }
    else exit(2);
    if(height - fsz - 6 < 0) {
        tooSmallWindow = 1;
        FIELD_FROM_LEFT = - height + fsz + 6;
        return;
    }
    FIELD_FROM_LEFT = width / 2 - (fsz + 2);
    FIELD_FROM_UP   = height / 2 - (fsz / 2 + 2);
    for(int i = 0; i < 3; ++i) {
        if(FIELD_FROM_UP == 1) break;
        else FIELD_FROM_UP--;
    }
    int fromUp = FIELD_FROM_UP;
    headX = FIELD_SIZE / 3;
    headY = FIELD_SIZE * 2 / 3;
    tailX = FIELD_SIZE / 3;
    tailY = FIELD_SIZE * 2 / 3 + 2;

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    system("cls");
    while(--fromUp) cout << "\n";
    string spaces(FIELD_FROM_LEFT, ' ');
    for(auto x : field) {
        SetConsoleTextAttribute(hStdOut, (WORD) 15);
        cout << "\n" << spaces;
        for(auto y : x) {
            if(y == '#') {
                if(can_go_through_walls) SetConsoleTextAttribute(hStdOut, (WORD) 170);
                else SetConsoleTextAttribute(hStdOut, (WORD) 17 * 6);
            }
            else SetConsoleTextAttribute(hStdOut, (WORD) 15);
            cout << y;
        }
    }
    if(b) {
        unsetSnakeBody();
        setPosition(FIELD_FROM_LEFT, fsz + FIELD_FROM_UP + 3);
        return;
    }
    head = {headX, headY};
    tail = {tailX, tailY};
    setSnakeBody();
    if(head.y < tail.y) {
        for(int i = head.y; i <= tail.y; ++i) {
            setPosition(head.x, i);
            cout << "zz";
        }
        length = tail.y - head.y + 1;
        direction = 'u';
        nn_direction = 0;
        dirs.resize(length - 1, direction);
        nn_dirs.resize(length - 1, nn_direction);
    }
    else if(head.y > tail.y) {
        for(int i = tail.y; i <= head.y; ++i) {
            setPosition(head.x, i);
            cout << "zz";
        }
        length = head.y - tail.y + 1;
        direction = 'd';
        nn_direction = 2;
        dirs.resize(length - 1, direction);
        nn_dirs.resize(length - 1, nn_direction);
    }
    else if(head.x > tail.x) {
        for(int i = tail.x; i <= head.x; ++i) {
            setPosition(i, head.y);
            cout << "zz";
        }
        length = (head.x - tail.x + 2) / 2;
        direction = 'r';
        nn_direction = 1;
        dirs.resize(length - 1, direction);
        nn_dirs.resize(length - 1, nn_direction);
    }
    else if(head.x < tail.x){
        for(int i = head.x; i <= tail.x; ++i) {
            setPosition(i, head.y);
            cout << "zz";
        }
        length = (tail.x - head.x + 2) / 2;
        direction = 'l';
        nn_direction = 3;
        dirs.resize(length - 1, direction);
        nn_dirs.resize(length - 1, nn_direction);
    }
    unsetSnakeBody();
    setPosition(FIELD_FROM_LEFT, fsz + FIELD_FROM_UP + 3);
    cout << "\n\t";
}

void setSnakeBody(int color)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD) color);
}

void unsetSnakeBody()
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD) 15);
}

void setPosition(int x, int y)
{
    static COORD coord;
    coord.X = x;
    coord.Y = y;
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!SetConsoleCursorPosition(hConsole, coord)) {
        cout << "\nError: " << GetLastError() << "\n";
        exit(1);
    }
}
void setPosition(const coord &point)
{
    static COORD coor;
    coor.X = point.x;
    coor.Y = point.y;
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!SetConsoleCursorPosition(hConsole, coor)) {
        cout << "\nError in second: " << GetLastError() << "\n";
        exit(1);
    }
}
