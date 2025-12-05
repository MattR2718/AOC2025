#include <iostream>
#include <print>
#include <vector>
#include <string>
#include <optional>

#include <grid.h>
#include <grid_graph.h>

int main(int, char**){
    std::cout << "Hello, from AOC2025!\n";

    std::vector<std::string> raw_map = {
        "S..#...",
        ".#.#.#.",
        ".......",
        "######.",
        "...E..."
    };

    Grid::Grid<char> grid(raw_map, 1, '#'); 

    size_t start = grid.find('S');
    size_t end = grid.find('E');

    auto logic = [](Grid::Grid<char> grid, size_t from, size_t to) {
        return grid[to] != '#';
    };

    auto result = Grid::bfs_path(grid, start, end, logic);

    if (result) {
        std::println("Path found! Length: {}", result->size());
        
        // Visualize path
        for (size_t idx : *result) {
            if (grid[idx] != 'S' && grid[idx] != 'E') 
                grid[idx] = '*';
        }
        grid.print();
    } else {
        std::println("No path found.");
    }
}
