#include <vector>
#include <unistd.h>
#include <cmath>
#include <limits>
#include <omp.h>
#include <algorithm>

std::vector<double> dijkstra(std::vector<double> graph);
std::vector<double> johnson(std::vector<double> graph);
std::vector<double> floyd(std::vector<double> graph);