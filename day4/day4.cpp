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

template<class T>
struct array2d {
    std::size_t rows = 0;
    std::size_t cols = 0;
    std::vector<T> data;

    array2d() = default;

    array2d(std::size_t r, std::size_t c, const T& value = T{})
        : rows(r), cols(c), data(r * c, value) {}

    template<class Cont>
    array2d(std::size_t r, std::size_t c, Cont&& container)
        : rows(r), cols(c), data(std::begin(container), std::end(container)) 
    {
        if (data.size() != r * c) {
            throw std::runtime_error("Container size does not match dimensions");
        }
    }

    T& operator()(std::size_t r, std::size_t c) {
        return data[r * cols + c];
    }
    const T& operator()(std::size_t r, std::size_t c) const {
        return data[r * cols + c];
    }

    T* raw() { return data.data(); }
    const T* raw() const { return data.data(); }

    void print(){
        for(std::size_t j = 0; j < rows; j++){
            for(std::size_t i = 0; i <  cols; i++){
                std::print("{}", operator()(j, i));
            }
            std::println("");
        }
    }
};

auto parse_input(){
    std::string linetxt;
    std::string input = "";
    std::size_t w = 0, h = 0;
    while(std::getline(std::cin, linetxt)){
        input += linetxt;
        h++;
        w = linetxt.length();
    }


    return array2d<char>{h, w, input};
}

auto p1(auto input){
    Timer::ScopedTimer _t("Part 1");

    auto pairs = std::views::cartesian_product(std::views::iota(0, (int)input.rows), std::views::iota(0, (int)input.cols)) | std::views::filter([&](auto p){return input(std::get<1>(p), std::get<0>(p)) == '@';});

    auto neigh = std::views::cartesian_product(std::views::iota(-1, 2), std::views::iota(-1, 2)) | std::views::filter([](auto p){ return std::get<0>(p) || std::get<1>(p); });

    return std::ranges::count_if(pairs, [&](auto p){
        return std::ranges::count_if(neigh, [&](auto n){
            int x = std::get<0>(p) + std::get<0>(n);
            int y = std::get<1>(p) + std::get<1>(n);
            if(x < 0 || x >= (int)input.cols || y < 0 || y >= (int)input.rows){ return false; }
            if(input(y, x) == '@'){ return true; }
            return false;
        }) < 4;
    });
    
}

auto p2(auto input){
    Timer::ScopedTimer _t("Part 2");

    auto pairs = std::views::cartesian_product(std::views::iota(0, (int)input.rows), std::views::iota(0, (int)input.cols)) | std::views::filter([&](auto p){return input(std::get<1>(p), std::get<0>(p)) == '@';});
    auto neigh = std::views::cartesian_product(std::views::iota(-1, 2), std::views::iota(-1, 2)) | std::views::filter([](auto p){ return std::get<0>(p) || std::get<1>(p); });

    int num = std::ranges::count(input.data, '@');
    int sn = num;

    do{
        auto ps = std::views::filter(pairs, [&](auto p){
            return input(std::get<1>(p), std::get<0>(p)) == '@' && std::ranges::count_if(neigh, [&](auto n){
                int x = std::get<0>(p) + std::get<0>(n);
                int y = std::get<1>(p) + std::get<1>(n);
                if(x < 0 || x >= (int)input.cols || y < 0 || y >= (int)input.rows){ return false; }
                if(input(y, x) == '@'){ return true; }
                return false;
            }) < 4;
        });
        std::ranges::for_each(ps, [&](auto p){input(std::get<1>(p), std::get<0>(p)) = '.';});
        num = std::ranges::distance(ps);
    }while(num);


    return sn - std::ranges::count(input.data, '@');
}

int main(){
    auto input = parse_input();
    std::println("Part 1: {}", p1(input));
    std::println("Part 2: {}", p2(input));
}