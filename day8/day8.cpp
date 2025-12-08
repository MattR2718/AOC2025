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
    uint64_t distSquared;
    int p1_index;
    int p2_index;

    auto operator<=>(const PointPair&) const = default;
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

// Radix sort for O(n) sorting by distSquared
// Significantly faster than std::sort
void radix_sort_pairs(std::vector<PointPair>& source) {
    const size_t n = source.size();
    if (n == 0) return;

    std::vector<PointPair> buffer(n);
    
    PointPair* in = source.data();
    PointPair* out = buffer.data();

    // Compute Histograms for all passes in one go.
    size_t counts[8][256] = {0};

    for (size_t i = 0; i < n; ++i) {
        uint64_t val = in[i].distSquared;
        // Unroll loop for scalar pipeline efficiency
        counts[0][val & 0xFF]++;
        counts[1][(val >> 8) & 0xFF]++;
        counts[2][(val >> 16) & 0xFF]++;
        counts[3][(val >> 24) & 0xFF]++;
        counts[4][(val >> 32) & 0xFF]++;
        counts[5][(val >> 40) & 0xFF]++;
        counts[6][(val >> 48) & 0xFF]++;
        counts[7][(val >> 56) & 0xFF]++;
    }

    // Convert counts to offsets
    size_t offsets[8][256];
    for(int pass = 0; pass < 8; ++pass) {
        size_t current_offset = 0;
        for (int i = 0; i < 256; ++i) {
            offsets[pass][i] = current_offset;
            current_offset += counts[pass][i];
        }
    }

    // Scatter
    for (int shift = 0; shift < 64; shift += 8) {
        int pass = shift / 8;
        size_t* pass_offsets = offsets[pass];

        for (size_t i = 0; i < n; i++) {
            uint8_t byte = (in[i].distSquared >> shift) & 0xFF;
            out[pass_offsets[byte]++] = in[i];
        }
        std::swap(in, out);
    }

    // If the final result ended up in buffer, copy it back to source
    if (in != source.data()) {
        source = buffer;
    }
}

// Find an 'upper bound' threshold distance
// Use a rough heuristic that isnt neccessarily correct for the full input
//      Look at all points from each 1D axis projection
//      Pick the WINDOW nearedt neighbors along that axis
// Find the largest distance within that mst
// That is the maximum threshold distance
// Since we have a complete graph where all edges are less than or equal to that distance
// Then the mst will only contain edges less than or equal to that distance
// So any edge greater than that distance can be safely ignored
auto find_threshold(const std::vector<Point>& points){
    int n = points.size();
    std::vector<PointPair> rough_edges;
    rough_edges.reserve(n * 20); // 3 dimensions * ~6 neighbors

    auto collect = [&](auto get_coord){
        std::vector<int> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::ranges::sort(idx, [&](int a, int b){
            return get_coord(points[a]) < get_coord(points[b]);
        });

        int window = 6; // hopefully enough to get a graph
        for(int i = 0; i < n; i++){
            for(int w = 1; w <= window && (i + w) < n; w++){
                int u = idx[i];
                int v = idx[i + w];

                uint64_t dx = static_cast<uint64_t>(points[u].x - points[v].x);
                uint64_t dy = static_cast<uint64_t>(points[u].y - points[v].y);
                uint64_t dz = static_cast<uint64_t>(points[u].z - points[v].z);
                rough_edges.push_back({dx*dx + dy*dy + dz*dz, u, v});
            }
        }
    };

    collect([](const Point& p){ return p.x; });
    collect([](const Point& p){ return p.y; });
    collect([](const Point& p){ return p.z; });

    // Erase duplicates
    rough_edges.erase(std::unique(rough_edges.begin(), rough_edges.end()), rough_edges.end());

    radix_sort_pairs(rough_edges);

    // Kruskal to find mst on rough edges
    std::vector<int> parent(n);
    std::iota(parent.begin(), parent.end(), 0);
    auto find = [&](int i){
        while(parent[i] != i){
            parent[i] = parent[parent[i]];
            i = parent[i];
        }
        return i;
    };

    int edges_count = 0;
    uint64_t max_weight = 0;

    for(const auto& edge : rough_edges){
        int u_root = find(edge.p1_index);
        int v_root = find(edge.p2_index);
        if(u_root != v_root){
            parent[u_root] = v_root;
            max_weight = edge.distSquared;
            edges_count++;
            if(edges_count == n - 1) break;
        }
    }

    if(edges_count < n - 1){
        return std::numeric_limits<uint64_t>::max();
    }

    return max_weight;
}

auto parse_input(std::string input_file = ""){
    Timer::ScopedTimer t_("Input Parsing");

    static auto line_collector = [](std::string_view line, std::vector<Point>& points) {
        auto vs = StringUtils::extract_numbers<int64_t>(line);
        points.emplace_back(Point{vs[0], vs[1], vs[2]});
    };

    auto input = InputUtils::parse_input<std::vector<Point>>(line_collector, input_file);

    int n = input.size();

    uint64_t limit = find_threshold(input);


    std::vector<PointPair> pairs;
    pairs.reserve(n * 20); // rough estimate
    for (int i = 0; i < n; ++i) {
        const auto& p1 = input[i];
        for (int j = i + 1; j < n; ++j) {
            const auto& p2 = input[j];

            uint64_t dx = static_cast<uint64_t>(p1.x - p2.x);
            uint64_t dy = static_cast<uint64_t>(p1.y - p2.y);
            uint64_t dz = static_cast<uint64_t>(p1.z - p2.z);

            uint64_t d2 = dx*dx + dy*dy + dz*dz;
            if(d2 > limit) continue;

            pairs.push_back({d2, i, j});
        }
    }

    radix_sort_pairs(pairs);

    return std::pair<std::vector<Point>, std::vector<PointPair>>{input, pairs};
}



auto p1(const auto& input_){
    Timer::ScopedTimer _t("Part 1");

    const auto& [input, pairs] = input_;


    int n = input.size();
    

    // Find the top k
    int k = input.size() == 1000 ? 1000 : 10;

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
    const auto& [input, pairs] = input_;

    int n = input.size();
    
    std::vector<int> in_mst(n, 0);
    uint64_t p2 = 0;

    int nodes_in_mst = 0;
    for(const auto& pair : pairs){
        if(in_mst[pair.p1_index] == 0 || in_mst[pair.p2_index] == 0){
            nodes_in_mst += (in_mst[pair.p1_index] == 0) ? 1 : 0;
            nodes_in_mst += (in_mst[pair.p2_index] == 0) ? 1 : 0;
            in_mst[pair.p1_index] = 1;
            in_mst[pair.p2_index] = 1;
            
            p2 = input[pair.p1_index].x * input[pair.p2_index].x;
        }
        if(nodes_in_mst == n) break;
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
[Timer] Input Parsing (Global): 4.340 ms
[Timer] Part 1 (Global): 90.202 µs
Part 1: 57970
[Timer] Part 2 (Global): 21.278 µs
Part 2: 8520040659
[Timer] Total (Global): 4.505 ms

*/