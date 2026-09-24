#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void usage(char* progname) {
    std::cout << "Использование:" << std::endl;
    std::cout << progname << " <алгоритм> " << " <входной файл> " << std::endl;
}

std::vector<std::string> parse_argv(int argc, char** argv) {
    std::vector<std::string> result = {};
    // Проверка числа аргументов
    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        std::cout << "Ошибка: недостаточно аргументов" << std::endl;
        return result;
    }
    std::string algo = argv[1];
    std::string filename = argv[2];
    // Проверка на то, что был задан существующий алгоритм
    if (!(algo == "dijkstra" || algo == "jonhson" || algo == "floyd")) {
        usage(argv[0]);
        std::cout << "Ошибка: неизвестный алгоритм " << algo << std::endl;
        return result;
    }
    // Проверка на существование доступа к входному файлу
    std::ifstream file(filename.c_str());
    if (!file.good()) {
        usage(argv[0]);
        std::cout << "Ошибка: файл " << filename << " не может быть открыт" << std::endl;
        return result; 
    }
    result.push_back(algo);
    result.push_back(filename);
    return result;
}

int main(int argc, char** argv) {
    std::vector args = parse_argv(argc, argv);
    if (args.empty()) return 1;
    std::cout << args[0] << " " << args[1] << std::endl;
    return 0;
}