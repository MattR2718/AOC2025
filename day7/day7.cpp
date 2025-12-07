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

#define AOC_INPUT_FILE_PATH "../inputs/day7/input.txt"

#include <input.h>
#include <string_utils.h>
#include <grid.h>

auto parse_input(std::string input_file = ""){
    Timer::ScopedTimer t_("Input Parsing");


    static auto line_collector = [](std::string_view line, std::vector<std::string>& lines) {
        lines.emplace_back(line);
    };

    auto lines = InputUtils::parse_input<std::vector<std::string>>(line_collector, input_file);

    static auto map = [](char c) { 
        if(c == '^') return int64_t{-1};
        if(c == 'S') return int64_t{1};
        return int64_t{0};
    };

    return Grid::Grid<int64_t>(lines, map);
}

auto p1_2(auto& input){
    Timer::ScopedTimer _t("Parts 1 and 2");
    int splits = 0;
    for(const auto row : std::views::iota(0u, input.rows)){
        for(const auto column : std::views::iota(0u, input.cols)){
            if(input(row, column) > int64_t{0} && input(row + 1, column) == int64_t{-1}){
                input(row + 1, column + 1) += input(row, column);
                input(row + 1, column - 1) += input(row, column);
                splits++;
            }else if(input(row, column) > int64_t{0}){
                input(row + 1, column) += input(row, column);
            }
        }
    }

    return std::pair<int, int64_t>{splits, std::ranges::fold_left(std::views::iota(0u, input.cols), int64_t{0}, [&input](int64_t acc, auto v){
        return acc + input(input.rows - 1, v);
    })};
}

int main(int argc, char** argv){
    Timer::ScopedTimer t_("Total");
    auto input = parse_input((argc == 2 ? std::string(argv[1]) : ""));
    std::println("Part (1, 2): {}", p1_2(input));
}


/*

Using embedded input
[Timer] Input Parsing (Global): 103.426 µs
[Timer] Parts 1 and 2 (Global): 67.421 µs
Part (1, 2): (1553, 15811946526915)
[Timer] Total (Global): 219.478 µs

*/