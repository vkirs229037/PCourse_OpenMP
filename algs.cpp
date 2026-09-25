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
    sleep(2);
    return graph;
}

std::vector<double> floyd(std::vector<double> graph) {
    sleep(3);
    return graph;
}

