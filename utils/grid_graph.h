#ifndef GRID_GRAPH_H
#define GRID_GRAPH_H
#include <deque>
#include <optional>
#include <limits>
#include <ranges>

#include "grid.h"

namespace Grid {

template<typename T, typename Pred>
std::optional<std::vector<std::size_t>> bfs_path(
    const Grid<T>& grid,
    std::size_t start,
    std::size_t end,
    Pred can_step,
    bool diagonal = false
){
    std::deque<std::size_t> q;
    q.push_back(start);

    // Keep parent array
    // Keeps track of visited and allows path reconstruction
    constexpr auto NPOS = std::numeric_limits<std::size_t>::max();
    std::vector<std::size_t> parent(grid.data.size(), NPOS);

    parent[start] = start;

    const auto& offsets = diagonal ? grid.offsets_8 : grid.offsets_4;

    // BFS Loop
    bool found = false;
    while(!q.empty()){
        std::size_t curr = q.front();
        q.pop_front();

        if(curr == end){
            found = true;
            break;
        }

        for(int off : offsets){
            std::size_t n_idx = curr + off;

            // Check if already visited
            if(parent[n_idx] != NPOS) continue;

            // Check if can step
            if(!can_step(grid, curr, n_idx)) continue;

            // Mark parent and enqueue
            parent[n_idx] = curr;
            q.push_back(n_idx);
        }
    }
    if(!found) return std::nullopt;

    // Reconstruct path
    std::vector<std::size_t> path;
    std::size_t curr = end;
    while(curr != start){
        path.push_back(curr);
        curr = parent[curr];
    }
    path.push_back(start);
    std::ranges::reverse(path);
    return path;
}

// Generic flood fill
template<typename T, typename Pred>
std::vector<int> bfs_map(
    const Grid<T>& grid,
    std::size_t start,
    Pred can_step,
    bool diagonal = false
){
    std::deque<std::size_t> q;
    q.push_back(start);

    std::vector<int> dist(grid.data.size(), -1);
    dist[start] = 0;

    const auto& offsets = diagonal ? grid.offsets_8 : grid.offsets_4;

    // BFS Loop
    while(!q.empty()){
        std::size_t curr = q.front();
        q.pop_front();

        for(int off : offsets){
            std::size_t n_idx = curr + off;

            // Check if already visited
            if(dist[n_idx] != -1) continue;

            // Check if can step
            if(!can_step(grid, curr, n_idx)) continue;

            // Mark seen and enqueue
            dist[n_idx] = dist[curr] + 1;
            q.push_back(n_idx);
        }
    }
    return dist;
}

} // namespace Grid

#endif // GRID_GRAPH_H