#include "algs.hpp"

std::vector<double> dijkstra(std::vector<double> graph) {
    size_t total_size = graph.size();
    if (total_size == 0) return {};

    size_t n = std::sqrt(total_size);
    if (n * n != total_size) return {}; // Защита от неквадратной матрицы

    std::vector<double> result(total_size, 0.0);
    const double INF = std::numeric_limits<double>::infinity();

    // Распараллеливание внешнего цикла по исходным вершинам (алгоритм Флойда-Уоршелла не используется, 
    // так как требуется именно Дейкстра для каждой вершины).
    #pragma omp parallel for
    for (size_t src = 0; src < n; ++src) {
        std::vector<double> dist(n, INF);
        std::vector<bool> visited(n, false);
        dist[src] = 0.0;

        for (size_t count = 0; count < n; ++count) {
            // Поиск непосещенной вершины с минимальным расстоянием
            double min_dist = INF;
            size_t u = 0;
            for (size_t v = 0; v < n; ++v) {
                if (!visited[v] && dist[v] < min_dist) {
                    min_dist = dist[v];
                    u = v;
                }
            }

            // Если все оставшиеся вершины недостижимы, прерываем
            if (min_dist == INF) break;

            visited[u] = true;

            // Обновление расстояний до соседей
            // Примечание: предполагается, что отсутствие ребра в матрице смежности 
            // обозначено значением <= 0 (обычно 0). Веса ребер должны быть > 0.
            for (size_t v = 0; v < n; ++v) {
                double weight = graph[u * n + v];
                if (weight > 0 && !visited[v] && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                }
            }
        }

        // Запись результатов в итоговую матрицу. 
        // Если вершина недостижима (dist == INF), записываем 0.
        for (size_t v = 0; v < n; ++v) {
            result[src * n + v] = (dist[v] == INF) ? 0.0 : dist[v];
        }
    }

    return result;
}

std::vector<double> johnson(std::vector<double> graph) {
    size_t total_size = graph.size();
    if (total_size == 0) return {};

    size_t n = std::sqrt(total_size);
    if (n * n != total_size) return {}; // Защита от неквадратной матрицы

    const double INF = std::numeric_limits<double>::infinity();
    const double EPS = 1e-9; // Эпсилон для проверки наличия ребра (0.0 означает отсутствие ребра)

    // 1. Алгоритм Беллмана-Форда для вычисления потенциалов (h)
    // Добавление виртуальной вершины с нулевыми ребрами ко всем остальным 
    // эквивалентно инициализации массива h нулями.
    std::vector<double> h(n, 0.0);
    
    for (size_t i = 0; i < n; ++i) {
        bool updated = false;
        for (size_t u = 0; u < n; ++u) {
            for (size_t v = 0; v < n; ++v) {
                double w = graph[u * n + v];
                if (std::abs(w) > EPS && h[u] + w < h[v]) {
                    h[v] = h[u] + w;
                    updated = true;
                }
            }
        }
        if (!updated) break;
    }

    // Проверка на наличие цикла отрицательного веса
    for (size_t u = 0; u < n; ++u) {
        for (size_t v = 0; v < n; ++v) {
            double w = graph[u * n + v];
            if (std::abs(w) > EPS && h[u] + w < h[v]) {
                return {}; // Граф содержит цикл отрицательного веса, алгоритм неприменим
            }
        }
    }

    // 2. Перевзвешивание ребер: w'(u, v) = w(u, v) + h[u] - h[v]
    std::vector<double> reweighted_graph(total_size, 0.0);
    for (size_t u = 0; u < n; ++u) {
        for (size_t v = 0; v < n; ++v) {
            double w = graph[u * n + v];
            if (std::abs(w) > EPS) {
                reweighted_graph[u * n + v] = w + h[u] - h[v];
            }
        }
    }

    // 3. Запуск алгоритма Дейкстры для каждой вершины на перевзвешенном графе
    std::vector<double> result(total_size, 0.0);

    #pragma omp parallel for
    for (size_t src = 0; src < n; ++src) {
        std::vector<double> dist(n, INF);
        std::vector<bool> visited(n, false);
        dist[src] = 0.0;

        for (size_t count = 0; count < n; ++count) {
            double min_dist = INF;
            size_t u = 0;
            for (size_t v = 0; v < n; ++v) {
                if (!visited[v] && dist[v] < min_dist) {
                    min_dist = dist[v];
                    u = v;
                }
            }

            if (min_dist == INF) break;
            visited[u] = true;

            for (size_t v = 0; v < n; ++v) {
                if (!visited[v]) {
                    // Проверяем наличие ребра в исходном графе, чтобы избежать проблем 
                    // с ребрами, чей перевзвешенный вес стал равен ровно 0.
                    double orig_w = graph[u * n + v];
                    if (std::abs(orig_w) > EPS) {
                        double w = reweighted_graph[u * n + v];
                        if (dist[u] + w < dist[v]) {
                            dist[v] = dist[u] + w;
                        }
                    }
                }
            }
        }

        // 4. Возврат к исходным весам: d(u, v) = d'(u, v) - h[u] + h[v]
        for (size_t v = 0; v < n; ++v) {
            if (dist[v] != INF) {
                result[src * n + v] = dist[v] - h[src] + h[v];
            } else {
                result[src * n + v] = 0.0; // Если вершина недостижима, записываем 0
            }
        }
    }

    return result;
}

std::vector<double> floyd(std::vector<double> graph) {
    size_t total_size = graph.size();
    if (total_size == 0) return {};

    size_t n = std::sqrt(total_size);
    if (n * n != total_size) return {};

    const double INF = std::numeric_limits<double>::infinity();
    std::vector<double> dist(total_size, INF);

    // 1. Инициализация матрицы расстояний
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i == j) {
                dist[i * n + j] = 0.0;
            } else if (graph[i * n + j] != 0.0) {
                dist[i * n + j] = graph[i * n + j];
            }
        }
    }

    // 2. Алгоритм Флойда-Уоршелла
    // Внешний цикл (по k) нельзя параллелить из-за строгой зависимости итераций.
    // Параллелим средний цикл (по i), так как для фиксированного k обновление строк независимы.
    for (size_t k = 0; k < n; ++k) {
        #pragma omp parallel for
        for (size_t i = 0; i < n; ++i) {
            double dist_ik = dist[i * n + k];
            if (dist_ik == INF) continue; // Оптимизация: путь через k невозможен

            for (size_t j = 0; j < n; ++j) {
                double dist_kj = dist[k * n + j];
                if (dist_kj == INF) continue;

                double new_dist = dist_ik + dist_kj;
                if (new_dist < dist[i * n + j]) {
                    dist[i * n + j] = new_dist;
                }
            }
        }
    }

    // 3. Замена INF на 0 для недостижимых вершин
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (dist[i * n + j] == INF) {
                dist[i * n + j] = 0.0;
            }
        }
    }

    return dist;
}
