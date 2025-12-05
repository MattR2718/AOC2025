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

struct Input{
    std::vector<std::pair<uint64_t, uint64_t>> rs;
    std::vector<uint64_t> vs;
};

auto parse_input(std::string input_file){
    Timer::ScopedTimer t_("Input Parsing");

    // Parse to u64 without allocation
    auto to_u64 = [](std::string_view sv) {
        uint64_t val = 0;
        std::from_chars(sv.data(), sv.data() + sv.size(), val);
        return val;
    };

    auto parse_line = [&to_u64](std::string_view linetxt, bool& start, auto& i){
        if(linetxt.empty()){ start = false; return; }
        if(start){
            std::size_t d = linetxt.find('-');
            if (d != std::string_view::npos) {
                i.rs.emplace_back(
                    to_u64(linetxt.substr(0, d)), 
                    to_u64(linetxt.substr(d + 1))
                );
            }
        }else{
            i.vs.emplace_back(to_u64(linetxt));
        }
    };
    
    Input i;
    bool start = true;

    if(!input_file.empty()){
        std::println("Using file {}", input_file);
        std::string linetxt;
        std::ifstream f(input_file);

        while(std::getline(f, linetxt)){
            parse_line(linetxt, start, i);
        }
    }
#ifdef __cpp_pp_embed
    else{
static constexpr const char _input[] = {
    #embed "../inputs/day5/input.txt"
    , '\0'   
};
static constexpr std::string_view input_sv = _input;

        std::println("Using embedded file");

        std::size_t s = 0, e = 0;
        while((e = input_sv.find('\n', s)) != std::string_view::npos){
            auto l = input_sv.substr(s, e - s);

            parse_line(l, start, i);

            s = e + 1;
        }

    }
#else
    if(!input_file.empty()){
        std::println("Reading from std::cin");
        std::string linetxt;

        while(std::getline(std::cin, linetxt)){
            parse_line(linetxt, start, i);
        }
    }
#endif

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