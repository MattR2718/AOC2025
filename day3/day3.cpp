#include <print>
#include <vector>
#include <string>
#include <iostream>
#include <string_view>
#include <cmath>
#include <ranges>
#include <algorithm>
#include <numeric>

#include <timer.h>

auto parse_input(){
    std::string linetxt;
    std::vector<std::string> batteries;
    while(std::getline(std::cin, linetxt)){
        batteries.push_back(linetxt);
    }

    return batteries;
}

auto inline find_max(std::string_view s, int s_idx, int n){
    int max_v = -1, max_idx = -1;
    for(int i = s_idx; i < (s.size() - n); i++){
        if(s[i] - '0' > max_v){
            max_v = s[i] - '0';
            max_idx = i;
        }
    }

    return std::pair<int, int>{max_v, max_idx};
}

int p1(const auto input){
    Timer::ScopedTimer _t("Part 1");
    return std::ranges::fold_left(input, 0, [](auto acc, auto s){
        auto[v, i]{find_max(s, 0, 1)};
        auto[v2, _]{find_max(s, i + 1, 0)};
        return acc += v * 10 + v2;
    });
}


uint64_t p2(const auto input){
    Timer::ScopedTimer _t("Part 2");
    std::pair<int, int> o{0, -1};
    return std::ranges::fold_left(input, 0ULL, [&](auto acc, auto s){
        
        acc += std::ranges::fold_left(
            std::views::iota(0ULL, 12ULL) | std::views::reverse,
            0ULL,
            [&](auto acc_i, auto pow){
                o = find_max(s, o.second + 1, pow);
                return acc_i + o.first * std::powl(10, pow);
            }
        );
        o = {0, -1};
        return acc;
    });
}

int main(){
    auto input = parse_input();
    std::println("Part 1: {}", p1(input));
    std::println("Part 2: {}", p2(input));
}