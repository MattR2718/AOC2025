#include <cstdint>
#include <ctre.hpp>
#include <iostream>
#include <print>
#include <vector>
#include <numeric>
#include <ranges>
#include <algorithm>
#include <cmath>

#include <timer.h>

struct range{
    std::uint64_t from;
    std::uint64_t to;
};

auto parse_input(){
    std::string linetxt;
    std::vector<range> ranges;
    static constexpr auto re = ctll::fixed_string{R"((\d+)-(\d+))"};
    while(std::getline(std::cin, linetxt)){
        for (auto m : ctre::search_all<re>(linetxt)) {
            auto first  = m.get<1>().to_number<std::uint64_t>();
            auto second = m.get<2>().to_number<std::uint64_t>();

            ranges.emplace_back(range{first, second});
        }
    }
    return ranges;
}

std::uint64_t p1(const auto& ranges){
    Timer::ScopedTimer t("Part 1");
    return std::ranges::fold_left(ranges, 0Ull, [](auto acc, const auto r){ // Iterate over all pairs of boundaries
        auto v = std::views::iota(r.from, r.to + 1)             // Create range of numbers
                    | std::views::filter([](std::uint64_t x){   // Filter based on whether the first half is the same as the second half
                        int nd = std::ceil(std::log10(x));
                        return (x / std::uint64_t(std::pow(10, nd / 2))) == (x % std::uint64_t(std::pow(10, nd / 2)));
                    });
        return std::ranges::fold_left(v, acc, std::plus{}); // Sum up filtered range
    });
}

std::uint64_t p2(const auto& ranges){
    Timer::ScopedTimer t("Part 2");

    return std::ranges::fold_left(ranges, 0Ull, [](auto acc, const auto r){ // Iterate over all pairs of boundaries
        auto v = std::views::iota(r.from, r.to + 1)     // Create range of numbers
                    | std::views::filter([](std::uint64_t x){   // Filter based on whether the number is made of repeating components
                        int nd = (x == 0) ? 1 : static_cast<int>(std::floor(std::log10(x))) + 1;    // Get number of digits
                        auto digits = std::views::iota(0, nd)                       // Convert number into a range, e.g. 123 -> {1, 2, 3}
                                    | std::views::transform([=](int i){
                                        int pow = 1;
                                        for(int k = 0; k < nd - i - 1; k++){ pow *= 10; }
                                        return (x / pow) % 10;
                                    });
                        return std::ranges::any_of(std::views::iota(1, nd), // Check if any possible length of chunk produces an invalid id
                                                    [nd, digits](int chunk_size){
                                                        if(nd % chunk_size != 0){ return false; }
                                                        auto chunks = digits | std::views::chunk(chunk_size);
                                                        return std::ranges::all_of(chunks, [&](auto c){
                                                            return std::ranges::equal(c, *chunks.begin());
                                                        });
                                                    });
                    });
        return std::ranges::fold_left(v, acc, std::plus{}); // Sum up all invalid ids
    });
}

std::uint64_t p2_2(const auto& ranges){
    Timer::ScopedTimer t("Part 2");

    return std::ranges::fold_left(ranges, 0Ull, [](auto acc, const auto r){ // Iterate over all pairs of boundaries
        auto v = std::views::iota(r.from, r.to + 1)     // Create range of numbers
                    | std::views::filter([](std::uint64_t x){   // Filter based on whether the number is made of repeating components
                        int nd = (x == 0) ? 1 : static_cast<int>(std::floor(std::log10(x))) + 1;    // Get number of digits
                        for(int k = 0; k < (nd / 2) + 1; k++){
                            if(nd % k == 0){
                                int mask = (int(std::pow(10, nd)) - 1) / (int(std::pow(10, k)) - 1);

                                if(x % mask == 0){
                                    return true;
                                }
                            }
                        }
                        return false;
                    });
        return std::ranges::fold_left(v, acc, std::plus{}); // Sum up all invalid ids
    });
}

int main(){
    auto input = parse_input();
    std::println("Part 1: {}", p1(input));
    std::println("Part 2: {}", p2(input));
}