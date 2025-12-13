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

#define AOC_INPUT_FILE_PATH "../inputs/day12/input.txt"

#include <input.h>
#include <string_utils.h>
#include <grid.h>

struct Present{
    std::vector<std::vector<char>> shape;

    static std::vector<Present> generate_rotations(const std::vector<std::vector<char>>& base_shape){
        std::vector<Present> rotations;

        auto current_shape = base_shape;
        for(int i = 0; i < 4; ++i){
            rotations.push_back(Present{current_shape});
            // rotate 90 degrees clockwise
            int rows = current_shape.size();
            int cols = current_shape[0].size();
            std::vector<std::vector<char>> new_shape(cols, std::vector<char>(rows, ' '));
            for(int r = 0; r < rows; ++r){
                for(int c = 0; c < cols; ++c){
                    new_shape[c][rows - 1 - r] = current_shape[r][c];
                }
            }
            current_shape = new_shape;
        }

        return rotations;
    }

    static std::vector<Present> generate_flipped_rotations(const std::vector<std::vector<char>>& base_shape){
        std::vector<Present> all_rotations = generate_rotations(base_shape);
        
        auto flipped_shape = base_shape;
        for(auto& row : flipped_shape) {
            std::reverse(row.begin(), row.end());
        }
        
        std::vector<Present> flipped_rotations = generate_rotations(flipped_shape);
        all_rotations.insert(all_rotations.end(), flipped_rotations.begin(), flipped_rotations.end());
        return all_rotations;
    }


    void print() const {
        for(const auto& row : shape){
            for(const auto& c : row){
                std::cout << c;
            }
            std::cout << "\n";
        }
    }
};

struct Region{
    int width, height;
    std::vector<int> present_ids_to_place;

    void print() const {
        std::cout << "Region " << width << "x" << height << ": ";
        for(const auto& n : present_ids_to_place){
            std::cout << n << " ";
        }
        std::cout << "\n";
    }
};

struct Input{
    std::vector<std::vector<Present>> presents;
    std::vector<Region> regions;
};


auto parse_input(std::string input_file = ""){
    Timer::ScopedTimer t_("Input Parsing");

    bool presents = true;
    static auto parse_line = [&presents](std::string_view line, Input& input) {
        static constexpr auto region_pattern = ctll::fixed_string{ 
            R"(^(\d+)x(\d+): ((\d+ )*\d+)$)" 
        };

        if(line == ""){
            return;
        }

        if(auto m = ctre::match<R"((\d+):)">(line); m){
            input.presents.push_back({});
        }else if (auto m = ctre::match<region_pattern>(line); m) {
            presents = false;
            auto width = m.get<1>().to_number<int>();
            auto height = m.get<2>().to_number<int>();
            auto nums_sv = m.get<3>().to_view();
            auto counts = StringUtils::extract_numbers<int>(nums_sv);
            std::vector<int> expanded_ids;
            for(std::size_t id = 0; id < counts.size(); ++id){
                int qty = counts[id];
                for(int k=0; k<qty; ++k){
                    expanded_ids.push_back(id);
                }
            }
            input.regions.push_back(Region{width, height, expanded_ids});
        } else{
            if(input.presents.back().empty()){
                input.presents.back().push_back({});
            }
            input.presents.back().back().shape.push_back(std::vector<char>(line.begin(), line.end()));
        }

        

    };

    return InputUtils::parse_input<Input>(parse_line, input_file);
}

auto stamp_present(std::vector<std::vector<char>>& grid, const Present& pres, int top, int left){
    auto tgrid = grid;
    
    int pres_height = pres.shape.size();
    int pres_width = pres.shape[0].size();

    for(int r = 0; r < pres_height; ++r){
        for(int c = 0; c < pres_width; ++c){
            if(pres.shape[r][c] == '#'){
                if(grid[top + r][left + c] != '.'){
                    return false;
                }
            }
        }
    }

    for(int r = 0; r < pres_height; ++r){
        for(int c = 0; c < pres_width; ++c){
            if(pres.shape[r][c] == '#'){
                grid[top + r][left + c] = '#';
            }
        }
    }
    return true;
}

auto unstamp_present(std::vector<std::vector<char>>& grid, const Present& pres, int top, int left){
    int pres_height = pres.shape.size();
    int pres_width = pres.shape[0].size();

    for(int r = 0; r < pres_height; r++){
        for(int c = 0; c < pres_width; c++){
            if(pres.shape[r][c] == '#'){
                grid[top + r][left + c] = '.';
            }
        }
    }
}

// backtracking to place presents
auto backtrack(std::size_t idx, std::vector<std::vector<char>>& grid, const Region& region, const std::vector<std::vector<Present>>& presents){
    if(idx >= region.present_ids_to_place.size()){
        return true;
    }
    
    int pres_id = region.present_ids_to_place[idx];

    for(const auto& pres : presents[pres_id]){
        int pres_height = pres.shape.size();
        int pres_width = pres.shape[0].size();

        for(int r = 0; r <= region.height - pres_height; ++r){
            for(int c = 0; c <= region.width - pres_width; ++c){
                if(stamp_present(grid, pres, r, c)){
                    if(backtrack(idx + 1, grid, region, presents)){
                        return true;
                    }
                    unstamp_present(grid, pres, r, c);
                }
            }
        }
    }
    return false;
}

auto solve_region(const Region& region, const std::vector<std::vector<Present>>& presents){
    std::vector<std::vector<char>> grid(region.height, std::vector<char>(region.width, '.'));

    return backtrack(0, grid, region, presents);
}

bool check_region(const Region& region, const std::vector<std::vector<Present>>& presents){
    int total_present_area = 0;
    
    std::vector<int> areas;
    for(const auto& p_variants : presents) {
        int a = 0;
        for(const auto& row : p_variants[0].shape)
            for(char c : row) if(c == '#') a++;
        areas.push_back(a);
    }

    for(int id : region.present_ids_to_place) {
        total_present_area += areas[id];
    }

    return total_present_area <= (region.width * region.height);
}

bool check_trivial(const Region& region){
    int num_presents = region.present_ids_to_place.size();

    return ((region.width / 3) * (region.height / 3)) >= num_presents;
}

auto p1(auto& input){
    Timer::ScopedTimer _t("Part 1");
    int p1 = 0;
    for(const auto& region : input.regions){
        if(!check_region(region, input.presents)){
            continue;
        } else if(check_trivial(region)){
            p1 += 1;        
        } else if(solve_region(region, input.presents)){ // Never used, may not even work 
            p1 += 1;
        }
    }
    return p1;
}


int main(int argc, char** argv){
    Timer::ScopedTimer t_("Total");
    auto input = parse_input((argc == 2 ? std::string(argv[1]) : ""));

    for(auto& pres : input.presents){
        // Only generate from the base shape
        auto all_variants = Present::generate_flipped_rotations(pres[0].shape);
        
        std::set<std::vector<std::vector<char>>> unique_shapes;
        for(const auto& r : all_variants){
            unique_shapes.insert(r.shape);
        }

        // Replace the vector with all unique variants
        pres.clear();
        for(const auto& us : unique_shapes){
            pres.push_back(Present{us});
        }
    }


    std::println("Part 1: {}", p1(input));
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