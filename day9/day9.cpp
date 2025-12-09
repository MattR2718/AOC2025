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

#define AOC_INPUT_FILE_PATH "../inputs/day9/input.txt"

#include <input.h>
#include <string_utils.h>
#include <grid.h>

struct Point {
    int64_t x;
    int64_t y;

    auto operator<=>(const Point&) const = default;
};

struct PointHash {
    size_t operator()(const Point& p) const {
        return std::hash<int64_t>()(p.x) ^ (std::hash<int64_t>()(p.y) << 1);
    }
};

template <>
struct std::formatter<Point> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    template <typename FormatContext>
    auto format(const Point& p, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "Point(x={}, y={})", p.x, p.y);
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

    static auto parse_line = [](std::string_view line, std::vector<Point>& points) {
        auto nums = StringUtils::extract_numbers<int64_t>(line);
        if(nums.size() == 2){
            points.emplace_back(Point{static_cast<int>(nums[0]), static_cast<int>(nums[1])});
        }
    };

    return InputUtils::parse_input<std::vector<Point>>(parse_line, input_file);
}

int64_t cross_product(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

int64_t dist_sq(const Point& a, const Point& b) {
    return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y);
}


// Grahams scan algorithm to find convex hull
auto convex_hull(std::vector<Point>& points) {

    if(points.size() < 3) {
        return points;
    }

    // Move bottom most point to the front
    auto it = std::min_element(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return (a.y < b.y) || (a.y == b.y && a.x < b.x);
    });
    std::iter_swap(points.begin(), it);
    Point p0 = points[0];

    // Sort the rest of the points based on polar angle relative to p0
    std::sort(points.begin() + 1, points.end(), [&](const Point& a, const Point& b) {
        int64_t cp = cross_product(p0, a, b);
        if (cp == 0) {
            // If collinear, put the one CLOSER to p0 first
            return dist_sq(p0, a) < dist_sq(p0, b); 
        }
        return cp > 0;
    });

    auto stack = std::vector<Point>{p0, points[1]};

    for(const auto& p : points | std::views::drop(1)) {
        while (stack.size() > 1){
            Point top = stack.back();
            Point next_to_top = stack[stack.size() - 2];

            if(cross_product(next_to_top, top, p) <= 0) {
                stack.pop_back();
            } else {
                break;
            }
        }
        stack.push_back(p);
    }
    return stack;
}

auto p1(auto input){
    Timer::ScopedTimer _t("Part 1");

    auto ch = convex_hull(const_cast<std::vector<Point>&>(input));

    int64_t max = 0;
    for(const auto& p : ch){
        for(const auto& q : ch){
            if(p == q) continue;
            int64_t area = (std::abs(p.x - q.x) + 1) * (std::abs(p.y - q.y) + 1);
            max = std::max(max, area);
        }
    }

    return max;
}

bool is_point_inside_polygon(double test_x, double test_y, const std::vector<Point>& points) {
    bool inside = false;
    size_t j = points.size() - 1; // Previous vertex to check wrap around
    for (size_t i = 0; i < points.size(); ++i) {
        if ((points[i].y > test_y) != (points[j].y > test_y)) { // If edge vertically spans test_y (both points arent above or below)

            // x = x1 + (x2 - x1) * (y - y1) / (y2 - y1)
            double intersect_x = (points[j].x - points[i].x) * (test_y - points[i].y) / 
                                 (double)(points[j].y - points[i].y) + points[i].x;

            // If the intersection point is to the right of the test point, it's inside
            if (test_x < intersect_x) {
                inside = !inside;
            }
        }
        j = i;
    }
    return inside;
}

bool is_valid_rectangle(const Point& bl, const Point& tr, const std::vector<Point>& points) {
    // If a polygon vertex is strictly inside the rectangle
    for (const auto& p : points) {
        if (p.x > bl.x && p.x < tr.x && p.y > bl.y && p.y < tr.y) {
            return false;
        }
    }

    // Check for Edges cutting through
    size_t j = points.size() - 1;
    for (size_t i = 0; i < points.size(); ++i) {
        const auto& p1 = points[i];
        const auto& p2 = points[j];
        
        // Vertical Edge
        // Check if it separates the left and right sides of the rectangle
        // if it doesnt span the entire height it will be caught by vertex check
        if (p1.x == p2.x) {
            if (p1.x > bl.x && p1.x < tr.x) {
                int64_t min_y = std::min(p1.y, p2.y);
                int64_t max_y = std::max(p1.y, p2.y);
                if (min_y <= bl.y && max_y >= tr.y) return false;
            }
        }
        // Horizontal Edge
        // Check if it separates the top and bottom sides of the rectangle
        else if (p1.y == p2.y) {
            if (p1.y > bl.y && p1.y < tr.y) {
                int64_t min_x = std::min(p1.x, p2.x);
                int64_t max_x = std::max(p1.x, p2.x);
                if (min_x <= bl.x && max_x >= tr.x) return false;
            }
        }
        j = i;
    }

    // Rectangle is either fully inside shape or fully outside
    // Check midpoint
    double mid_x = (bl.x + tr.x) / 2.0;
    double mid_y = (bl.y + tr.y) / 2.0;
    
    return is_point_inside_polygon(mid_x, mid_y, points);
}

auto p2(const auto& input){
    Timer::ScopedTimer _t("Part 2");

    int64_t max_area = -1;

    // Go through all pairs of points and check for a valid rectangle
    for(const auto& i : std::views::iota(0u, input.size() - 1)){
        for(const auto& j : std::views::iota(i, input.size())){
            const auto& p = input[i];
            const auto& q = input[j];

            Point bl{std::min(p.x, q.x), std::min(p.y, q.y)};
            Point tr{std::max(p.x, q.x), std::max(p.y, q.y)};
            if(bl.x == tr.x && bl.y == tr.y) continue; // Same points, skip

            int64_t area = (tr.x - bl.x + 1) * (tr.y - bl.y + 1);
            if(area <= max_area) continue; // No need to check smaller areas

            if(is_valid_rectangle(bl, tr, input)){
                max_area = area;
            }
        }
    }

    return max_area;
}

int main(int argc, char** argv){
    Timer::ScopedTimer t_("Total");
    auto input = parse_input((argc == 2 ? std::string(argv[1]) : ""));
    std::println("Part 1: {}", p1(input));
    std::println("Part 2: {}", p2(input));
}


/*

Using embedded input
[Timer] Input Parsing (Global): 56.476 µs
[Timer] Part 1 (Global): 30.771 µs
Part 1: 4758121828
[Timer] Part 2 (Global): 8.874 ms
Part 2: 1577956170
[Timer] Total (Global): 9.018 ms

*/