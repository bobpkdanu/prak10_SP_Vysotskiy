#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const int width = 30;
const int height = 15;

const int Up = 0;
const int Right = 1;
const int Down = 2;
const int Left = 3;

struct MapState
{
    int x;
    int y;
    MapState* next;
};

struct Record
{
    char name[50];
    int score;
};

MapState* head = nullptr;
MapState* tail = nullptr;

int length = 0;
int score = 0;
int direction = Right;
int foodX, foodY;

void clearSnake()
{
    while (head != nullptr)
    {
        MapState* temp = head;
        head = head->next;
        delete temp;
    }

    tail = nullptr;
    length = 0;
}

void addMapState(int x, int y)
{
    MapState* p = new MapState;

    p->x = x;
    p->y = y;
    p->next = nullptr;
    if (head == nullptr)
    {
        head = p;
        tail = p;
    }
    else
    {
        tail->next = p;
        tail = p;
    }
    length++;
}

bool checkPosition(int x, int y)
{
    MapState* p = head;

    while (p != nullptr)
    {
        if (p->x == x && p->y == y)
            return true;
        p = p->next;
    }
    return false;
}

void createFood()
{
    do
    {
        foodX = rand() % width;
        foodY = rand() % height;
    } while (checkPosition(foodX, foodY));
}

void newGame()
{
    clearSnake();
    score = 0;
    direction = Right;
    addMapState(12, 7);
    addMapState(11, 7);
    addMapState(10, 7);

    createFood();
}

void drawField()
{
    system("cls");

    for (int y = -1; y <= height; y++)
    {
        for (int x = -1; x <= width; x++)
        {
            if (x == -1 || x == width ||
                y == -1 || y == height)
            {
                cout << "#";
            }
            else if (x == foodX && y == foodY)
            {
                cout << "*";
            }
            else
            {
                MapState* p = head;
                bool found = false;

                while (p != nullptr)
                {
                    if (p->x == x && p->y == y)
                    {
                        if (p == head)
                            cout << "O";
                        else
                            cout << "o";

                        found = true;
                        break;
                    }
                    p = p->next;
                }
                if (!found) cout << " ";
            }
        }
        cout << endl;
    }

    cout << "Счет: " << score << endl;
    cout << "Двигаться - стрелочки" << endl;
    cout << "S - сохранение" << endl;
    cout << "Q - выход" << endl;
}

void saveGame()
{
    ofstream file("save.bin", ios::binary);
    if (!file)
    {
        cout << "Не сохранено" << endl;
        Sleep(1000);
        return;
    }
    file.write((char*)&score, sizeof(score));
    file.write((char*)&direction, sizeof(direction));
    file.write((char*)&foodX, sizeof(foodX));
    file.write((char*)&foodY, sizeof(foodY));
    file.write((char*)&length, sizeof(length));
    MapState* p = head;

    while (p != nullptr)
    {
        file.write((char*)&p->x, sizeof(p->x));
        file.write((char*)&p->y, sizeof(p->y));
        p = p->next;
    }
    file.close();
    cout << "Сохранено" << endl;
    Sleep(1000);
}

bool loadGame()
{
    ifstream file("save.bin", ios::binary);
    if (!file) return false;

    int newScore, newDirection;
    int newFoodX, newFoodY, newLength;
    file.read((char*)&newScore, sizeof(newScore));
    file.read((char*)&newDirection, sizeof(newDirection));
    file.read((char*)&newFoodX, sizeof(newFoodX));
    file.read((char*)&newFoodY, sizeof(newFoodY));
    file.read((char*)&newLength, sizeof(newLength));

    if (!file || newLength < 1 || newLength > width * height || newDirection < Up || newDirection > Left || 
        newFoodX < 0 || newFoodX >= width || newFoodY < 0 || newFoodY >= height)
    {
        file.close();
        return false;
    }
    clearSnake();
    for (int i = 0; i < newLength; i++)
    {
        int x, y;
        file.read((char*)&x, sizeof(x));
        file.read((char*)&y, sizeof(y));
        if (!file || x < 0 || x >= width || y < 0 || y >= height)
        {
            clearSnake();
            file.close();
            return false;
        }
        addMapState(x, y);
    }

    file.close();

    score = newScore;
    direction = newDirection;
    foodX = newFoodX;
    foodY = newFoodY;
    return true;
}

void addRecord()
{
    Record r;

    cout << "Введите имя игрока: ";
    cin >> ws;
    cin.getline(r.name, 50);

    r.score = score;
    ofstream file("records.bin", ios::binary | ios::app);
    if (file)
    {
        file.write((char*)&r, sizeof(r));
        file.close();
    }
}

void showRecords()
{
    ifstream file("records.bin", ios::binary);
    system("cls");

    cout << "Рекорды: " << endl;
    if (!file)
    {
        cout << "Рекордов нет" << endl;
    }
    else
    {
        Record r;
        while (file.read((char*)&r, sizeof(r)))
        {
            cout << "Игрок: " << r.name << " | Счет: " << r.score << endl;
        }
        file.close();
    }
    cout << endl;
    cout << "Нажмите любуую кнопку";
    _getch();
}

bool oppositeDirection(int d)
{
    if (direction == Up && d == Down) return true;
    if (direction == Down && d == Up) return true;
    if (direction == Left && d == Right) return true;
    if (direction == Right && d == Left) return true;
    return false;
}

int moveSnake()
{
    if (_kbhit())
    {
        int key = _getch();

        if (key == 0 || key == 224)
        {
            key = _getch();
            int d = direction;
            if (key == 72) d = Up;
            if (key == 77) d = Right;
            if (key == 80) d = Down;
            if (key == 75) d = Left;
            if (!oppositeDirection(d)) direction = d;
        }
        else
        {
            if (key == 's' || key == 'S')
                saveGame();

            if (key == 'q' || key == 'Q')
                return 0;
        }
    }

    int x = head->x;
    int y = head->y;

    if (direction == Up) y--;
    if (direction == Right) x++;
    if (direction == Down) y++;
    if (direction == Left) x--;

    if (x < 0 || x >= width ||
        y < 0 || y >= height)
    {
        return -1;
    }

    bool eat = (x == foodX && y == foodY);

    MapState* p = head;

    while (p != nullptr)
    {
        if (p->x == x && p->y == y)
        {
            if (p == tail && !eat)
                break;

            return -1;
        }

        p = p->next;
    }

    MapState* newHead = new MapState;

    newHead->x = x;
    newHead->y = y;
    newHead->next = head;

    head = newHead;
    length++;

    if (eat)
    {
        score += 10;
        createFood();
    }
    else
    {
        MapState* p = head;
        while (p->next != tail) p = p->next;
        delete tail;
        tail = p;
        tail->next = nullptr;
        length--;
    }

    return 1;
}

void playGame()
{
    int result = 1;

    while (result == 1)
    {
        drawField();
        result = moveSnake();
        Sleep(10);
    }

    if (result == -1)
    {
        drawField();

        cout << endl;
        cout << "Игра - ВСЁ" << endl;
        cout << "Ваш счет: " << score << endl;
        if (score == 0)
        {
            cout << "Слишком плохо" << endl;
        }
        else 
        {
            addRecord();
            cout << "Результат сохранен" << endl;
        }
        Sleep(1500);
    }
    clearSnake();
}

int main()
{
    setlocale(LC_ALL, "RU");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    srand(time(NULL));

    int choice = 0;

    do
    {
        system("cls");

        cout << "Выберите действие: " << endl;
        cout << "1. Новая игра" << endl;
        cout << "2. Загрузить игру" << endl;
        cout << "3. Таблица рекордов" << endl;
        cout << "4. Выход" << endl;

        cin >> choice;

        if (choice == 1)
        {
            newGame();
            playGame();
        }
        else if (choice == 2)
        {
            if (loadGame())
            {
                cout << "Игра загружена" << endl;
                Sleep(1000);
                playGame();
            }
            else
            {
                cout << "Сохранение не найдено" << endl;
                Sleep(1500);
            }
        }
        else if (choice == 3)
        {
            showRecords();
        }
        else if (choice != 4)
        {
            cout << "Это мы ещё посмотрим" << endl;
            Sleep(1000);
        }

    } while (choice != 4);
    clearSnake();
    return 0;
}