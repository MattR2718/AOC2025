#include <print>
#include <vector>
#include <string>
#include <iostream>
#include <string_view>
#include <cmath>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <string_view>
#include <fstream>

#include <timer.h>

#define AOC_INPUT_FILE_PATH "../inputs/day4/input.txt"
#include <input.h>
#include <grid.h>

auto parse_input(std::string input_file){
    Timer::ScopedTimer t_("Input Parsing");

    auto line_collector = [](std::string_view line, std::vector<std::string>& lines) {
        lines.emplace_back(line);
    };

    auto lines = InputUtils::parse_input<std::vector<std::string>>(line_collector, input_file);

    return Grid::Grid<char>(lines, 1, '.');
}

auto p1(auto input){
    Timer::ScopedTimer _t("Part 1");

    auto indices = std::views::iota(0u, input.data.size());

    return std::ranges::count_if(indices, [&](std::size_t idx){
        if(input[idx] != '@') return false;
        return input.count_neighbours(idx, '@') < 4;
    });
}

auto p2(auto input){
    Timer::ScopedTimer _t("Part 2");

    std::vector<int> counts(input.data.size(), 0);
    std::vector<size_t> q;
    q.reserve(input.data.size() / 4);

    // Calculate neighbour counts for all '@'
    for(std::size_t i = 0; i < input.data.size(); i++){
        if(input[i] == '@'){
            counts[i] = input.count_neighbours(i, '@');
        }
    }

    // Identify initial deaths
    int removed_count = 0;
    for(std::size_t i = 0; i < input.data.size(); i++){
        if(input[i] == '@' && counts[i] < 4){
            q.push_back(i);
            input[i] = '.';
            removed_count++;
        }
    }

    // Handle effect of deaths
    std::size_t head = 0;
    while(head < q.size()){
        std::size_t curr = q[head++];
        
        for(int off : input.offsets_8){
            std::size_t n_idx = curr + off;

            // Only update neighbors that are currently alive
            if(input[n_idx] == '@'){                
                // If this specific decrement caused it to cross the threshold
                if(--counts[n_idx] == 3){
                    input[n_idx] = '.';
                    q.push_back(n_idx);
                    removed_count++;
                }
            }
        }
    }

    return removed_count;
}

int main(int argc, char** argv){
    Timer::ScopedTimer t_("Total");
    auto input = parse_input((argc == 2 ? std::string(argv[1]) : ""));
    std::println("Part 1: {}", p1(input));
    std::println("Part 2: {}", p2(input));
}


/*
Using embedded file
[Timer] Input Parsing (Global): 56.149 µs
[Timer] Part 1 (Global): 151.018 µs
Part 1: 1523
[Timer] Part 2 (Global): 645.097 µs
Part 2: 9290
[Timer] Total (Global): 917.070 µs

*/