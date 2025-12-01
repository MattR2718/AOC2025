#include <string_view>
#include <print>
#include <iostream>
#include <ctre.hpp>

#ifdef __cpp_pp_embed
#   pragma message("Embed supported")
#else
#   pragma message("Embed NOT supported")
#endif

// Embed input at compile time
constexpr const char input[] = {
    #embed "../inputs/day1/input.txt"
    , '\0'   
};
constexpr std::string_view input_sv = input;

// Count lines at compile time
consteval std::size_t count_lines(std::string_view text) {
    std::size_t count = 0;
    for (char c : text)
        if (c == '\n') ++count;
    if (!text.empty() && text.back() != '\n')
        ++count;
    return count;
}

struct Move {
    char direction;
    int value;
};

// Parse line into a move
consteval Move parse_line(std::string_view line) {
    Move m{};
    m.direction = line[0];
    int v = 0;
    for (size_t i = 1; i < line.size(); ++i)
        v = v*10 + (line[i] - '0');
    m.value = v;
    return m;
}

// Loop over lines in input and parse into move array
consteval auto parse_moves() {
    std::array<Move, count_lines(input_sv)> result{};
    size_t idx = 0;

    size_t pos = 0;
    while (pos < input_sv.size()) {
        size_t nl = input_sv.find('\n', pos);
        if (nl == std::string_view::npos) nl = input_sv.size();
        auto line = input_sv.substr(pos, nl - pos);

        result[idx++] = parse_line(line);

        pos = nl + 1;
    }
    return result;
}

consteval std::pair<int, int> p1_2(){
    constexpr auto moves = parse_moves();

    int val = 50;
    int p1 = 0;
    int p2 = 0;
    for(const auto m : moves){
        if(m.direction == 'L'){
            for(int i = 0; i < m.value; i++){
                val--;
                if(!val) p2++;
                if(val < 0){ val = 99; }
            }
        }else{
            for(int i = 0; i < m.value; i++){
                val++;
                if(val == 100){ val = 0; p2++;}
            }
        }
        val %= 100;
        if(!val) p1++;
    }

    return {p1, p2};
}


int main(){

    constexpr auto ans{p1_2()};

    static_assert(ans.first == 0, "P1");
    static_assert(ans.second == 0, "P2");
}