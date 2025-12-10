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
#include <queue>
#include <stack>


#include <z3++.h>
#include <ctre.hpp>

#include <timer.h>

#define AOC_INPUT_FILE_PATH "../inputs/day10/input.txt"

#include <input.h>
#include <string_utils.h>
#include <grid.h>

struct Machine {
    uint64_t ind_light = uint64_t{0};
    std::vector<std::vector<int>> buttons;
    std::vector<int> joltage;

    auto operator<=>(const Machine&) const = default;
};

template <>
struct std::formatter<Machine> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    template <typename FormatContext>
    auto format(const Machine& m, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "Machine(ind_light={:#b}, buttons={}, joltage={})", m.ind_light, m.buttons, m.joltage);
    }
};

template <>
struct std::formatter<std::vector<Machine>> : std::range_formatter<Machine> {
    constexpr formatter() {
        this->set_separator("\n");
        this->set_brackets("[", "]");
    }
};

auto parse_input(std::string input_file = ""){
    Timer::ScopedTimer t_("Input Parsing");

    static auto parse_line = [](std::string_view line, std::vector<Machine>& machines) {
        Machine m;
        // CTRE supports named captures using the syntax (?<name>...)
        // Each match is matching NOT <bracket type>
        static constexpr auto pattern = ctll::fixed_string{ 
            R"(\[(?<square>[^\]]*)\]|\((?<round>[^)]*)\)|\{(?<curly>[^}]*)\})" 
        };
        for (auto match : ctre::search_all<pattern>(line)) {
            if (match.get<"square">()) {
                auto pat = match.get<"square">().to_view();
                for(const auto& c : pat | std::views::reverse){
                    m.ind_light <<= 1;
                    if(c == '#'){
                        m.ind_light |= 1;
                    }
                }
            } 
            else if (match.get<"round">()) {
                m.buttons.push_back(StringUtils::extract_numbers<int>(match.get<"round">().to_view()));
            } 
            else if (match.get<"curly">()) {
                m.joltage = StringUtils::extract_numbers<int>(match.get<"curly">().to_view());
            }
        }
        machines.push_back(m);
    };

    return InputUtils::parse_input<std::vector<Machine>>(parse_line, input_file);
}

auto p1(auto input){
    Timer::ScopedTimer _t("Part 1");

    uint64_t p1 = 0;

    for(const auto& machine : input){
        std::set<uint64_t> visited;
        std::queue<std::pair<uint64_t, int>> q;
        q.push({uint64_t{0}, 0});

        while(!q.empty()){
            auto [state, steps] = q.front();
            q.pop();

            if(state == machine.ind_light){
                p1 += steps;
                break;
            }
            if(visited.contains(state)) continue;
            visited.insert(state);

            for(const auto& button : machine.buttons){
                uint64_t new_state = state;
                for(const auto& bit : button){
                    new_state ^= (uint64_t{1} << bit);
                }
                q.push({new_state, steps + 1});
            }
        }
    }

    return p1;
}

auto p2(const auto& input){
    Timer::ScopedTimer _t("Part 2");

    uint64_t p2 = 0;

    for(const auto& machine : input){
        z3::context c;
        z3::optimize opt(c);

        std::vector<z3::expr> button_vars;
        z3::expr total_presses = c.int_val(0);

        // Integer buttons representing how many times each button is pressed
        for(const auto& button_idx : std::views::iota(0u, machine.buttons.size())){
            std::string name = std::format("button_{}", button_idx);
            z3::expr var = c.int_const(name.c_str());

            opt.add(var >= 0);
            button_vars.push_back(var);
            total_presses = total_presses + var;
        }

        // Constraints for the final joltage state
        // Jolt at index is the sum of presses of buttons that toggle that jolt
        for(const auto& jolt_idx : std::views::iota(0u, machine.joltage.size())){
            z3::expr row_sum = c.int_val(0);
            for(const auto& button_idx : std::views::iota(0u, machine.buttons.size())){
                if(std::find(machine.buttons[button_idx].begin(), machine.buttons[button_idx].end(), jolt_idx) != machine.buttons[button_idx].end()){
                    row_sum = row_sum + button_vars[button_idx];
                }
            }
            opt.add(row_sum == c.int_val((int)machine.joltage[jolt_idx]));
        }

        opt.minimize(total_presses);

        if(opt.check() == z3::sat){
            z3::model m = opt.get_model();
            uint64_t presses = 0;
            for(const auto& button_idx : std::views::iota(0u, machine.buttons.size())){
                std::string name = std::format("button_{}", button_idx);
                z3::expr var = c.int_const(name.c_str());
                presses += m.eval(var).get_numeral_uint64();
            }
            p2 += presses;
        }
    }

    return p2;
}

int main(int argc, char** argv){
    Timer::ScopedTimer t_("Total");
    auto input = parse_input((argc == 2 ? std::string(argv[1]) : ""));
    std::println("Part 1: {}", p1(input));
    std::println("Part 2: {}", p2(input));
}


/*

Using embedded input
[Timer] Input Parsing (Global): 990.384 µs
[Timer] Part 1 (Global): 3.348 ms
Part 1: 401
[Timer] Part 2 (Global): 336.051 ms
Part 2: 15017
[Timer] Total (Global): 340.551 ms

*/