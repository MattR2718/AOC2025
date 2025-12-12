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
#include <map>
#include <compare>
#include <limits>
#include <set>
#include <stack>
#include <queue>

#include <ctre.hpp>

#include <timer.h>

#define AOC_INPUT_FILE_PATH "../inputs/day11/input.txt"

#include <input.h>
#include <string_utils.h>
#include <grid.h>


auto parse_input(std::string input_file = ""){
    Timer::ScopedTimer t_("Input Parsing");

    static auto parse_line = [](std::string_view line, std::map<std::string, std::vector<std::string>>& points) {
        static constexpr auto pattern = ctll::fixed_string{ 
            R"((?<key>[^:]+):\s*(?<vals>.*))" 
        };

        for(const auto& match : ctre::search_all<pattern>(line)) {
            auto key = match.get<"key">().to_view();
            auto vals_str = match.get<"vals">().to_view();
            auto vals = StringUtils::split(vals_str, ' ');
            points[std::string(key)] = std::vector<std::string>{};
            for(const auto& v : vals){
                points[std::string(key)].push_back(std::string(v));
            }
        }

    };

    return InputUtils::parse_input<std::map<std::string, std::vector<std::string>>>(parse_line, input_file);
}


auto p1(auto& input){
    Timer::ScopedTimer _t("Part 1");

    auto start = "you";
    auto end = "out";

    std::stack<std::string> s;
    s.push(start);

    int p1 = 0;

    while(!s.empty()){
        auto curr = s.top();
        s.pop();

        if(curr == end){
            p1++;
            continue;
        }
        for(const auto& neighbor : input.at(curr)){
            s.push(neighbor);
        }
    }

    return p1;
}

auto from_to_count(std::string curr, std::string target, auto& input, auto& cache){
    if(curr == target){
        return uint64_t{1};
    }

    if(cache.contains(curr)){
        return cache[curr];
    }

    uint64_t total{0};
    for(const auto& neigh : input[curr]){
        total += from_to_count(neigh, target, input, cache);
    }
    cache[curr] = total;
    return total;
}

auto p2(auto& input){
    Timer::ScopedTimer _t("Part 2");

    std::map<std::string, uint64_t> cache;
    auto get_segment_count = [&](std::string from, std::string to) {
        cache.clear(); 
        return from_to_count(from, to, input, cache);
    };    

    auto svr_to_fft = get_segment_count("svr", "fft");
    auto fft_to_dac = get_segment_count("fft", "dac");
    auto dac_to_out = get_segment_count("dac", "out");

    auto svr_to_dac = get_segment_count("svr", "dac");
    auto dac_to_fft = get_segment_count("dac", "fft");
    auto fft_to_out = get_segment_count("fft", "out");

    return svr_to_fft * fft_to_dac * dac_to_out + svr_to_dac * dac_to_fft * fft_to_out;
}

int main(int argc, char** argv){
    Timer::ScopedTimer t_("Total");
    auto input = parse_input((argc == 2 ? std::string(argv[1]) : ""));
    std::println("Part 1: {}", p1(input));
    std::println("Part 2: {}", p2(input));
}


/*

Using embedded input
[Timer] Input Parsing (Global): 419.760 µs
[Timer] Part 1 (Global): 115.181 µs
Part 1: 753
[Timer] Part 2 (Global): 1.667 ms
Part 2: 450854305019580
[Timer] Total (Global): 2.293 ms

*/