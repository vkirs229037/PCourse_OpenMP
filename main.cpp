#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include "algs.hpp"
#include "omp.h"

void usage(char* progname) {
    std::cout << "Использование:" << std::endl;
    std::cout << progname << " <алгоритм>" << " <входной файл>" << std::endl;
}

// Парсировка аргументов командной строки
std::vector<std::string> parse_argv(int argc, char** argv) {
    std::vector<std::string> result = {};
    // Проверка числа аргументов
    if (argc != 3) {
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

// Выполнение алгоритма, запись результата, вывод времени работы
bool execute(
    // Тип функции
    std::vector<float> (*func)(std::vector<float>), 
    // Сама матрица графа
    std::vector<float> graph,
    // Название алгоритма и входного файла 
    std::string algo, std::string filename) {
        bool result = true;
        double start = omp_get_wtime();
        auto result_matrix = func(graph);
        double end = omp_get_wtime();

        std::cout << "Время выполнения (с):" << std::endl;
        std::cout << end - start << std::endl;

        std::string outfilename = algo + "_" + filename + ".txt";
        std::ofstream outfile(outfilename);
        if (!outfile.good()) {
            std::cout << "Ошибка: файл для вывода " << outfilename << " не мог быть открыть" << std::endl;
            result = false;
        }
        else {
            for (auto el : result_matrix) {
                outfile << el;
                outfile << " ";
            }
        }
        outfile.close();
        return result;
    }

int main(int argc, char** argv) {
    std::vector args = parse_argv(argc, argv);
    if (args.empty()) return 1;

    std::ifstream infile(args[1]);
    std::istream_iterator<float> start(infile), end;
    std::vector<float> graph(start, end);

    for (auto el : graph) {
        std::cout << el << " " << std::endl;
    }

    bool result;
    if (args[1] == "dijkstra") {
        result = execute(dijkstra, graph, args[1], args[0]);
    }
    else if (args[1] == "johnson")
    {
        result = execute(johnson, graph, args[1], args[0]);
    }
    // Здесь точно будет "floyd"
    else {
        result = execute(floyd, graph, args[1], args[0]);
    }

    if (!result) {
        return 2;
    }
    return 0;
}