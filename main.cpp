#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include "algs.hpp"
#include "omp.h"

void usage(char* progname) {
    std::cout << "Использование:" << std::endl;
    std::cout << progname << " <алгоритм>" << " <входной файл>" << " <число потоков>" << std::endl;
}

// Парсировка аргументов командной строки
std::vector<std::string> parse_argv(int argc, char** argv) {
    std::vector<std::string> result = {};
    // Проверка числа аргументов
    if (argc != 4) {
        usage(argv[0]);
        std::cout << "Ошибка: недостаточно аргументов" << std::endl;
        return result;
    }
    std::string algo = argv[1];
    std::string filename = argv[2];
    // Проверка на то, что был задан существующий алгоритм
    if (!(algo == "dijkstra" || algo == "johnson" || algo == "floyd")) {
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
    try {
        int n_threads = std::atoi(argv[3]);
    }
    catch (std::exception const& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
        return result;
    }
    result.push_back(algo);
    result.push_back(filename);
    result.push_back(argv[3]);
    return result;
}

// Выполнение алгоритма, запись результата, вывод времени работы
bool execute(
    // Тип функции
    std::vector<double> (*func)(std::vector<double>), 
    // Сама матрица графа
    std::vector<double> graph,
    // Название алгоритма и входного файла 
    std::string algo, std::string filename) {
        float start = omp_get_wtime();
        auto result_matrix = func(graph);
        float end = omp_get_wtime();

        if (result_matrix.empty()) {
            std::cout << "Ошибка: Алгоритм не мог быть выполнен" << std::endl;
            return false;
        }

        std::cout << "Время выполнения (с):" << std::endl;
        std::cout << end - start << std::endl;

        std::string outfilename = algo + "_" + filename + ".txt";
        std::ofstream outfile(outfilename);
        if (!outfile.good()) {
            std::cout << "Ошибка: файл для вывода " << outfilename << " не мог быть открыть" << std::endl;
            return false;
        }
        
        for (auto el : result_matrix) {
            outfile << el;
            outfile << " ";
        }
        
        outfile.close();
        return true;
    }

int main(int argc, char** argv) {
    std::vector args = parse_argv(argc, argv);
    if (args.empty()) return 1;

    std::ifstream infile(args[1]);
    std::istream_iterator<double> start(infile), end;
    std::vector<double> graph(start, end);

    // for (auto el : graph) {
    //     std::cout << el << " " << std::endl;
    // }

    int n_threads = std::atoi(args[2].c_str());
    omp_set_num_threads(n_threads);

    bool result;
    if (args[0] == "dijkstra") {
        std::cout << "dijkstra" << std::endl;
        result = execute(dijkstra, graph, args[1], args[0]);
    }
    else if (args[0] == "johnson")
    {
        std::cout << "johnson" << std::endl;
        result = execute(johnson, graph, args[1], args[0]);
    }
    // Здесь точно будет "floyd"
    else {
        std::cout << "floyd" << std::endl;
        result = execute(floyd, graph, args[1], args[0]);
    }

    if (!result) {
        return 2;
    }
    return 0;
}