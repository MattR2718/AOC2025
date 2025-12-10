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


#include <Highs.h>
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

    Highs highs;

    // No output printing
    highs.setOptionValue("output_flag", false);

    for(const auto& machine : input){
        highs.clearModel();

        int num_vars = machine.buttons.size();
        int num_constraints = machine.joltage.size();

        // Columns represent buttons
        // Each column is one button
        std::vector<double> col_cost(num_vars, 1.0);
        std::vector<double> col_lower(num_vars, 0.0);
        std::vector<double> col_upper(num_vars, highs.getInfinity());

        // Rows represent joltage constraints
        // Each row is one constraint
        // Upper and lower bounds are equal to the joltage required
        std::vector<double> row_lower(num_constraints);
        std::vector<double> row_upper(num_constraints);
        for(const auto& [i, val] : std::views::enumerate(machine.joltage)){
            row_lower[i] = static_cast<double>(val);
            row_upper[i] = static_cast<double>(val);
        }

        // Matrix A represents which buttons affect which constraints
        // A[i][j] = 1 if button j affects constraint i, else 0
        std::vector<double> a_value;
        std::vector<HighsInt> a_index;
        std::vector<HighsInt> a_start(num_vars + 1, 0);

        int curr_idx = 0;
        for(int j = 0; j < num_vars; j++){
            for(int row_idx : machine.buttons[j]){
                a_index.push_back(static_cast<HighsInt>(row_idx));
                a_value.push_back(1.0);
                curr_idx++;
            }
            a_start[j + 1] = static_cast<HighsInt>(curr_idx);
        }


        HighsLp lp;
        lp.num_col_ = static_cast<HighsInt>(num_vars);
        lp.num_row_ = static_cast<HighsInt>(num_constraints);
        lp.col_cost_ = col_cost;
        lp.col_lower_ = col_lower;
        lp.col_upper_ = col_upper;
        lp.row_lower_ = row_lower;
        lp.row_upper_ = row_upper;
        lp.a_matrix_.start_ = a_start;
        lp.a_matrix_.index_ = a_index;
        lp.a_matrix_.value_ = a_value;
        lp.a_matrix_.format_ = MatrixFormat::kColwise;

        // Enforce integer variables
        lp.integrality_.resize(num_vars, HighsVarType::kInteger);

        HighsStatus status = highs.passModel(lp);

        if(status != HighsStatus::kOk){
            std::cerr << "Error passing model to HiGHS\n";
            continue;
        }

        status = highs.run();
        if(status == HighsStatus::kOk){

            double min_presses = highs.getInfo().objective_function_value;;
            p2 += static_cast<uint64_t>(std::round(min_presses));
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
[Timer] Input Parsing (Global): 253.346 µs
[Timer] Part 1 (Global): 3.722 ms
Part 1: 401
[Timer] Part 2 (Global): 221.917 ms
Part 2: 15017
[Timer] Total (Global): 226.096 ms

*/