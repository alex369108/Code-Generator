#include <iostream>
#include <fstream>
#include <unordered_set>
#include <random>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>

//генератор случайной цифры
int randDigit(int min = 0, int max = 9) {
    static std::mt19937 rng(static_cast<unsigned long>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

//проверка валидности кода
bool isValid(const std::string& code) {
    if (code.size() != 12) return false;
    for (char c : code) if (!isdigit(c)) return false;

    if ((code[2] - '0') >= 6) return false;
    if (((code[9] - '0') % 2) != 0) return false;
    if ((code[2] - '0') + (code[4] - '0') <= 9) return false;

    int sum = 0;
    for (int i = 1; i < 12; i++) sum += code[i] - '0';
    while (sum >= 10) {
        int tmp = 0;
        while (sum > 0) { tmp += sum % 10; sum /= 10; }
        sum = tmp;
    }
    if ((code[0] - '0') != sum) return false;

    long long num = std::stoll(code);
    if (num % 7 != 0) return false;
    if (num % 13 != 0) return false;
    if (num % 2 == 0) return false;

    return true;
}

//загрузка кодов из файла
std::unordered_set<std::string> loadCodes(const std::string& filename) {
    std::unordered_set<std::string> codes;
    std::ifstream file(filename);
    std::string code;
    while (file >> code) codes.insert(code);
    return codes;
}

// сохранение кода
void saveCode(const std::string& filename, const std::string& code) {
    std::ofstream file(filename, std::ios::app);
    file << code << "\n";
}

// генерация уникального кода
std::string generateCode(const std::unordered_set<std::string>& generated,
    const std::unordered_set<std::string>& used) {
    std::string code;
    do {
        code.assign(12, '0');
        for (int i = 1; i < 12; i++) code[i] = '0' + randDigit();

        code[2] = '0' + randDigit(0, 5);
        code[9] = '0' + (randDigit(0, 4) * 2);

        do {
            code[2] = '0' + randDigit(0, 5);
            code[4] = '0' + randDigit(0, 9);
        } while ((code[2] - '0') + (code[4] - '0') <= 9);

        int sum = 0;
        for (int i = 1; i < 12; i++) sum += code[i] - '0';
        while (sum >= 10) {
            int tmp = 0;
            while (sum > 0) { tmp += sum % 10; sum /= 10; }
            sum = tmp;
        }
        code[0] = '0' + sum;

    } while (!isValid(code) || generated.find(code) != generated.end() || used.find(code) != used.end());

    return code;
}

void main_menu(){
    std::cout << "1. Создать код\n";
    std::cout << "2. Проверить код\n";
    std::cout << "0. Выйти\n";
    std::cout << ">>  ";
}

//интерфейс
int main() {
    setlocale(LC_ALL, "RUS");

    const std::string genFile = "generated_codes.txt";
    const std::string usedFile = "used_codes.txt";

    int choice;

    while (true) {
        std::cout << "---Ver. 0.3.1---\n";
        main_menu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            system("cls");
            std::cout << "Ошибка ввода.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else {
            break;
        }
    }

    while (choice != 0) {

        auto generatedCodes = loadCodes(genFile);
        auto usedCodes = loadCodes(usedFile);

        switch (choice) {
        case 1: {
            std::string newCode = generateCode(generatedCodes, usedCodes);
            saveCode(genFile, newCode);
            system("cls");
            system("color 02");
            std::cout << "Сгенерирован новый код: " << newCode << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system("color 07");
            break;
        }

        case 2: {
            std::string input;
            std::cout << "Введите код: ";
            std::cin >> input;

            if (!isValid(input)) {
                system("cls");
                system("color 04");
                std::cout << "Неверный код.\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else if (usedCodes.find(input) != usedCodes.end()) {
                system("cls");
                system("color 04");
                std::cout << "Код уже был активирован.\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else if (generatedCodes.find(input) != generatedCodes.end()) {
                saveCode(usedFile, input);
                system("cls");
                system("color 02");
                std::cout << "Активация успешна!\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                system("cls");
                system("color 06");
                std::cout << "Код валиден, но не найден локально.\n";
                std::cout << "Возможен внешний код (создан на другом устройстве).\n";
                char confirm;
                std::cout << "Активировать его (Y/N)? ";
                std::cin >> confirm;
                if (confirm == 'Y' || confirm == 'y') {
                    saveCode(usedFile, input);
                    system("cls");
                    system("color 02");
                    std::cout << "Код активирован как внешний.\n";
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }

            system("color 07");
            break;
        }

        default:
            system("cls");
            std::cout << "Ошибка ввода.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            break;
        }
        while (true) {
            main_menu();
            std::cin >> choice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                system("cls");
                std::cout << "Ошибка ввода.\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                break;
            }
        }
    }

    return 0;
}