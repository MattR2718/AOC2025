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

#define AOC_INPUT_FILE_PATH "../inputs/day6/input.txt"

#include <input.h>
#include <string_utils.h>
#include <grid.h>

#define DOUBLE_PARSING true

struct Input{
    std::vector<std::vector<uint64_t>> vs;
    std::vector<char> ops;
};


auto parse_input(std::string input_file = ""){
    Timer::ScopedTimer t_("Input Parsing");

    

    auto line_collector = [](std::string_view line, std::vector<std::string>& lines) {
        lines.emplace_back(line);
    };

    auto lines = InputUtils::parse_input<std::vector<std::string>>(line_collector, input_file);

    Grid::Grid i2 = Grid::Grid<char>(lines, 1, ' ');

#if DOUBLE_PARSING
    auto parse_line = [&](std::string_view linetxt, auto& i){
        if(linetxt.front() == '*' || linetxt.front() == '+'){
            auto ops = std::views::filter(linetxt, [](char c){ return c != ' '; });
            i.ops.insert(i.ops.begin(), ops.begin(), ops.end());
        }else{
            i.vs.emplace_back(std::vector<uint64_t>());
            i.vs.back() = StringUtils::extract_numbers<uint64_t>(linetxt);
        }
    };
    
    Input i1 =  InputUtils::parse_input<Input>(parse_line, input_file);

    return std::pair<Input, Grid::Grid<char>>{i1, i2};
#else
    return i2;
#endif
}

auto p1(const auto& input){
    Timer::ScopedTimer _t("Part 1");

    auto accumulator = std::views::iota(0u, input.ops.size()) 
                        | std::views::transform([&](auto i){ return input.ops[i] == '+' ? 0 : 1; })
                        | std::ranges::to<std::vector<uint64_t>>();


    std::ranges::for_each(input.vs, [&](const auto& v){ 
        std::ranges::for_each(std::views::iota(0u, v.size()), [&](auto i){
            if(input.ops[i] == '+'){
                accumulator[i] += v[i];
            }else{
                accumulator[i] *= v[i];
            }
        });
    });


    return std::ranges::fold_left(accumulator, uint64_t(0), std::plus<>());
}

inline auto parse_block_add(const std::size_t start_col, const auto& input){
    uint64_t block_total = 0;

    for (std::size_t row = 0; row < input.rows - 1; row++) {
        const char* row_ptr = (const char*)(input.row_ptr(row));
        std::size_t c = start_col;
        
        // Skip leading spaces efficiently
        while (c < input.cols && row_ptr[c] == ' ') c++;

        if (c < input.cols) {
            uint64_t val = 0;
            while (c < input.cols) {
                char d = row_ptr[c];
                if (d == ' ') break; // Found end of number
                val = val * 10 + (d - '0');
                c++;
            }

            block_total += val;
        }
    }
    return block_total;
}

inline auto parse_block_mul(const std::size_t start_col, const auto& input){
    uint64_t block_total = 1;

    for (std::size_t row = 0; row < input.rows - 1; row++) {
        const char* row_ptr = (const char*)(input.row_ptr(row));
        std::size_t c = start_col;
        
        // Skip leading spaces efficiently
        while (c < input.cols && row_ptr[c] == ' ') c++;

        if (c < input.cols) {
            uint64_t val = 0;
            while (c < input.cols) {
                char d = row_ptr[c];
                if (d == ' ') break; // Found end of number
                val = val * 10 + (d - '0');
                c++;
            }

            block_total *= val;
        }
    }
    return block_total;
}

auto p1_2(const auto& input){
    Timer::ScopedTimer _t("Part 1_2");

    // Extract ops
    std::vector<std::pair<char, int>> ops;
    for(std::size_t i = 0; i < input.cols; i++){
        if(input(input.rows - 1, i) != ' '){
            ops.emplace_back(input(input.rows - 1, i), i);
        }
    }

    uint64_t total = 0;
    for(const auto& [op, col_start] : ops){
        total += (op == '+') ? parse_block_add(col_start, input) : parse_block_mul(col_start, input);
    }

    return total;
}

auto p2(const auto& input){
    Timer::ScopedTimer _t("Part 2");

    // Print grid
    /*
    for(std::size_t r = 0; r < input.rows; r++){
        for(std::size_t c = 0; c < input.cols; c++){
            std::print("{}", input(r, c));
        }
        std::print("\n");
    }
        */

    // Extract ops
    std::vector<char> ops;
    for(std::size_t i = 0; i < input.cols; i++){
        if(input(input.rows - 1, i) != ' '){
            ops.emplace_back(input(input.rows - 1, i));
        }
    }

    uint64_t total = 0, curr_num = 0, block_total = 0;
    bool gap = true;
    int op = ops.size() - 1;
    for(std::size_t c = input.cols - 1; c != std::numeric_limits<std::size_t>::max(); c--){
        curr_num = 0;
        for(std::size_t r = 0; r < input.rows - 1; r++){
            if(input(r, c) != ' '){
                gap = false;
                curr_num = curr_num * 10 + (input(r, c) - '0');
            }
        }
        if(!gap){
            if(ops[op] == '+'){
                block_total += curr_num;
            }else{
                block_total *= curr_num;
            }
            gap = true;
        }else{
            // End of block
            total += block_total;
            op--;
            block_total = ops[op] == '+' ? 0 : 1;
        }
    }

    return total + block_total;
}

int main(int argc, char** argv){
    Timer::ScopedTimer t_("Total");
#if DOUBLE_PARSING
    std::println("Double parsing");
    auto[input1, input2] = parse_input((argc == 2 ? std::string(argv[1]) : ""));
    std::println("Part 1: {}", p1(input1));
    std::println("Part 2: {}", p2(input2));
#else
    std::println("Single parsing");
    auto input = parse_input((argc == 2 ? std::string(argv[1]) : ""));
    std::println("Part 1_2: {}", p1_2(input));
    std::println("Part 2: {}", p2(input));
#endif
}


/*


// Double parsing
// Slower overall, much faster p1
Using embedded input
Using embedded input
[Timer] Input Parsing (Global): 166.584 µs
[Timer] Part 1 (Global): 11.972 µs
Part 1: 5171061464548
[Timer] Part 2 (Global): 52.914 µs
Part 2: 10189959087258
[Timer] Total (Global): 274.646 µs

// Single parsing
// Faster overall, slower p1
Using embedded input
[Timer] Input Parsing (Global): 48.373 µs
[Timer] Part 1_2 (Global): 50.844 µs
Part 1_2: 5171061464548
[Timer] Part 2 (Global): 54.214 µs
Part 2: 10189959087258
[Timer] Total (Global): 188.062 µs

*/