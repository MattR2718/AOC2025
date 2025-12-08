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

#include <timer.h>

#define AOC_INPUT_FILE_PATH "../inputs/day8/input.txt"

#include <input.h>
#include <string_utils.h>
#include <grid.h>

struct Point{
    int64_t x, y, z;

    auto operator<=>(const Point&) const = default;
};

struct PointPair {
    double distSquared;
    int p1_index;
    int p2_index;
};

template <>
struct std::formatter<Point> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    template <typename FormatContext>
    auto format(const Point& p, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "Point(x={}, y={}, z={})", p.x, p.y, p.z);
    }
};

template <>
struct std::formatter<std::vector<Point>> : std::range_formatter<Point> {
    constexpr formatter() {
        this->set_separator("\n");
        this->set_brackets("[", "]");
    }
};

auto parse_input(std::string input_file = ""){
    Timer::ScopedTimer t_("Input Parsing");

    static auto line_collector = [](std::string_view line, std::vector<Point>& points) {
        auto vs = StringUtils::extract_numbers<int64_t>(line);
        points.emplace_back(Point{vs[0], vs[1], vs[2]});
    };

    auto input = InputUtils::parse_input<std::vector<Point>>(line_collector, input_file);

    std::vector<PointPair> pairs;
    int n = input.size();

    pairs.reserve(n * (n - 1) / 2);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double d2 = std::hypot(
                static_cast<double>(input[i].x - input[j].x),
                static_cast<double>(input[i].y - input[j].y),
                static_cast<double>(input[i].z - input[j].z)
            );
            
            pairs.push_back({d2, i, j});
        }
    }

    std::ranges::sort(pairs, [](const PointPair& a, const PointPair& b) {
        return a.distSquared < b.distSquared;
    });

    return std::pair<std::vector<Point>, std::vector<PointPair>>{input, pairs};
}



auto p1(const auto& input_){
    Timer::ScopedTimer _t("Part 1");

    auto [input, pairs] = input_;


    int n = input.size();
    

    // Find the top k

    int k = input.size() == 1000 ? 1000 : 10;

    std::partial_sort(pairs.begin(), pairs.begin() + k, pairs.end(),
        [](const PointPair& a, const PointPair& b) {
            return a.distSquared < b.distSquared;
        }
    );

    std::vector<std::vector<int>> adj(n);
    for (int i = 0; i < k; ++i) {
        int u = pairs[i].p1_index;
        int v = pairs[i].p2_index;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    std::vector<bool> visited(n, false);
    std::vector<int64_t> circuit_sizes;

    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            int64_t current_size = 0;
            
            std::vector<int> q;
            q.push_back(i);
            visited[i] = true;
            
            size_t head = 0;
            while(head < q.size()){
                int u = q[head++];
                current_size++;

                // Check all neighbors
                for(int neighbor : adj[u]){
                    if(!visited[neighbor]){
                        visited[neighbor] = true;
                        q.push_back(neighbor);
                    }
                }
            }
            circuit_sizes.push_back(current_size);
        }
    }

    std::ranges::sort(circuit_sizes, std::greater<>());


    return circuit_sizes[0] * circuit_sizes[1] * circuit_sizes[2];
}

auto p2(const auto& input_){
    Timer::ScopedTimer _t("Part 2");
    auto [input, pairs] = input_;

    int n = input.size();
    
    std::vector<int> in_mst(n, 0);
    uint64_t p2 = 0;

    for(const auto& pair : pairs){
        if(in_mst[pair.p1_index] == 0 || in_mst[pair.p2_index] == 0){
            in_mst[pair.p1_index] = 1;
            in_mst[pair.p2_index] = 1;
            p2 = input[pair.p1_index].x * input[pair.p2_index].x;
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
[Timer] Input Parsing (Global): 38.528 ms
[Timer] Part 1 (Global): 2.633 ms
Part 1: 57970
[Timer] Part 2 (Global): 839.805 µs
Part 2: 8520040659
[Timer] Total (Global): 42.296 ms

*/