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

#define AOC_INPUT_FILE_PATH "../inputs/day5/input.txt"

#include <input.h>
#include <string_utils.h>

struct Input{
    std::vector<std::pair<uint64_t, uint64_t>> rs;
    std::vector<uint64_t> vs;
};


auto parse_input(std::string input_file = ""){
    Timer::ScopedTimer t_("Input Parsing");

    bool start = true;

    auto parse_line = [&](std::string_view linetxt, auto& i){
        if(linetxt.empty()){ start = false; return; }
        if(start){
            std::size_t d = linetxt.find('-');
            if (d != std::string_view::npos) {
                i.rs.emplace_back(
                    StringUtils::to_num(linetxt.substr(0, d)), 
                    StringUtils::to_num(linetxt.substr(d + 1))
                );
            }
        }else{
            i.vs.emplace_back(StringUtils::to_num(linetxt));
        }
    };
    
    Input i = InputUtils::parse_input<Input>(parse_line, input_file);

    std::vector<std::pair<uint64_t, uint64_t>> merged_ranges;
    merged_ranges.reserve(i.rs.size());
    std::ranges::sort(i.rs, [](const auto& a, const auto& b){ return a.first < b.first; });
    merged_ranges.emplace_back(i.rs.front());
    for(const auto& r : i.rs | std::views::drop(1)){
        auto& last = merged_ranges.back();
        if(r.first <= last.second){
            last.second = std::max(last.second, r.second);
        }else{
            merged_ranges.push_back(r);
        }
    }
    i.rs = std::move(merged_ranges);

    return i;
    
}

auto p1(auto input){
    Timer::ScopedTimer _t("Part 1");
    auto in_range = [&input](uint64_t v){
        auto it = std::ranges::lower_bound(input.rs, v,
            std::less<uint64_t>(),
            [](const auto& range){ return range.second; });
        return (it != input.rs.end() && v >= it->first && v <= it->second);
    };
    return std::ranges::count_if(input.vs, in_range);
}

auto p2(auto input){
    Timer::ScopedTimer _t("Part 2");
    return std::ranges::fold_left(input.rs, 0ULL, [](uint64_t acc, const auto& r){
        return acc + (r.second - r.first + 1);
    });
}

int main(int argc, char** argv){
    Timer::ScopedTimer t_("Total");
    auto input = parse_input((argc == 2 ? std::string(argv[1]) : ""));
        
    std::println("Part 1: {}", p1(input));
    std::println("Part 2: {}", p2(input));
}


/*

Using embedded file
[Timer] Input Parsing (Global): 53.580 µs
[Timer] Part 1 (Global): 40.940 µs
Part 1: 611
[Timer] Part 2 (Global): 100.999 ns
Part 2: 345995423801866
[Timer] Total (Global): 134.908 µs

*/