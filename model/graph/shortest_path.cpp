#include "shortest_path.h"
#include "graph.h"

#include <cmath>
#include <algorithm>

auto shortest_path(const Graph &graph, int start_vertex, int end_vertex) -> std::vector<int>
{
    std::map<int, double> distance          {};
    std::map<int, int>    parent            {};

    std::vector<int>      vertices          {graph.get_vertices()};
    std::vector<int>      adjacent_vertices {};

    double                min               {};
    int                   last_vertex       {};

    for (int i {0}; i < static_cast<int>(graph.get_vertices().size()); ++i)
    {
        distance[i] = std::numeric_limits<double>::infinity();
    }

    distance[start_vertex] = 0;

    while (!vertices.empty())
    {
        min = std::numeric_limits<double>::infinity();

        for (const auto& vertex: vertices)
        {
            if (distance[vertex] < min)
            {
                last_vertex = vertex;
                min         = distance[vertex];
            }
        }

        if (std::isinf(min))
        {
            return std::vector<int> {};
        }

        if (last_vertex == end_vertex)
        {
             return build_path(parent, start_vertex, end_vertex);
        }

        adjacent_vertices = graph.get_adjacent_vertices(last_vertex);

        for (const auto& vertex: adjacent_vertices)
        {
            if (distance[vertex] > distance[last_vertex] + graph.edge_weight(last_vertex, vertex))
            {
                distance[vertex] = distance[last_vertex] + graph.edge_weight(last_vertex, vertex);
                parent[vertex]   = last_vertex;
            }
        }

        auto iter {std::find(vertices.begin(), vertices.end(), last_vertex)};

        vertices.erase(iter);
    }

    return std::vector<int> {};
}

auto build_path(std::map<int, int>& parent, int start_vertex, int end_vertex) -> std::vector<int>
{
    if (start_vertex == end_vertex)
    {
        return std::vector<int> {};
    }

    std::vector<int> result = {end_vertex};
    int              vertex = end_vertex;

    while (vertex != start_vertex)
    {
        vertex = parent[vertex];

        result.push_back(vertex);
    }

    std::reverse(result.begin(), result.end());

    return result;
}
