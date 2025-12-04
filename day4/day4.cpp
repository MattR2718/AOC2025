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

template<class T>
struct array2d {
    std::size_t rows = 0;
    std::size_t cols = 0;
    std::size_t stride = 0;
    std::vector<T> data;
    std::vector<int> neighbour_offsets;

    array2d() = default;

    array2d(std::size_t r, std::size_t c, const T& value = T{})
        : rows(r), cols(c), stride(c + 2), data((r + 2) * (c + 2), '.') {
            for(size_t i = 0; i < r; ++i) {
                std::fill_n(data.begin() + (i + 1) * stride + 1, c, value);
            }

            int s = (int)stride;
            neighbour_offsets = { -s-1, -s, -s+1, -1, 1, s-1, s, s+1 };
        }

    template<class Cont>
    array2d(std::size_t r, std::size_t c, Cont&& container)
        : rows(r), cols(c), stride(c + 2),  data((r + 2) * (c + 2), '.') 
    {
        auto it = std::begin(container);
        for(size_t i = 0; i < r; ++i) {
            std::copy_n(it + i * c, c, data.begin() + (i + 1) * stride + 1);
        }
        int s = (int)stride;
            neighbour_offsets = { -s-1, -s, -s+1, -1, 1, s-1, s, s+1 };
    }

    // Access raw index
    T& operator[](std::size_t idx) { return data[idx]; }
    const T& operator[](std::size_t idx) const { return data[idx]; }

    // Access y, x
    T& operator()(std::size_t r, std::size_t c) {
        return data[(r + 1) * stride + (c + 1)];
    }
    const T& operator()(std::size_t r, std::size_t c) const {
        return data[(r + 1) * stride + (c + 1)];
    }

    T* raw() { return data.data(); }
    const T* raw() const { return data.data(); }

    int count_neighbours(std::size_t idx) const {
        int cnt = 0;
        for (int off : neighbour_offsets) {
            if (data[idx + off] == '@') cnt++;
        }
        return cnt;
    }

    void print(){
        for(std::size_t j = 0; j < rows; j++){
            for(std::size_t i = 0; i <  cols; i++){
                std::print("{}", operator()(j, i));
            }
            std::println("");
        }
    }
};

#ifdef __cpp_pp_embed
constexpr const char _input[] = {
    #embed "../inputs/day4/input.txt"
    , '\0'   
};
constexpr std::string_view input_sv = _input;
#endif

auto parse_input(std::string input_file){
    Timer::ScopedTimer t_("Input Parsing");
    if(!input_file.empty()){
        std::println("Using file {}", input_file);
        std::string linetxt;
        std::string input = "";
        std::size_t w = 0, h = 0;
        std::ifstream f(input_file);
        while(std::getline(f, linetxt)){
            input += linetxt;
            h++;
            w = linetxt.length();
        }
        return array2d<char>{w, h, input};
    }
#ifdef __cpp_pp_embed
    else{

        std::println("Using embedded file");
        std::size_t w = input_sv.find('\n');
        std::size_t h = std::ranges::count(input_sv, '\n');
        if (!input_sv.empty() && input_sv.back() != '\n') h++;
        std::string clean_input;
        clean_input.reserve(input_sv.size()); 
        
        for(char c : input_sv) {
            if(c != '\n') clean_input.push_back(c);
        }
        return array2d<char>{h, w, clean_input};
    }
#else
    if(!input_file.empty()){
        std::println("Reading from std::cin");
        std::string linetxt;
        std::string input = "";
        std::size_t w = 0, h = 0;
        while(std::getline(std::cin, linetxt)){
            input += linetxt;
            h++;
            w = linetxt.length();
        }
        return array2d<char>{w, h, input};
    }
#endif

    
}

auto p1(auto input){
    Timer::ScopedTimer _t("Part 1");

    auto indices = std::views::iota(0u, input.data.size());

    return std::ranges::count_if(indices, [&](std::size_t idx){
        if(input[idx] != '@') return false;
        return input.count_neighbours(idx) < 4;
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
            counts[i] = input.count_neighbours(i);
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
        
        for(int off : input.neighbour_offsets){
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