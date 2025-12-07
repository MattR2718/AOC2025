#ifndef GRID_H
#define GRID_H

#include <vector>
#include <string>
#include <algorithm>
#include <print>
#include <ranges>
#include <algorithm>
#include <numeric>

namespace Grid{

template<typename T>
struct Grid {
    std::size_t rows = 0;
    std::size_t cols = 0;
    std::size_t padding = 1;
    std::size_t stride = 0;  // Physical width = cols + (2 * padding)
    
    std::vector<T> data;
    
    // Pre-calculated offsets for 1D navigation
    std::vector<int> offsets_8; 
    std::vector<int> offsets_4; 

    T border_value;

    Grid() = default;

    Grid(std::size_t r, std::size_t c, std::size_t pad = 1, T fill_val = T{}, T border_val = T{})
        : rows(r), cols(c), padding(pad), stride(c + 2 * pad), border_value(border_val) 
    {
        data.assign((r + 2 * pad) * stride, border_val);
        
        // Fill the inner area
        for(size_t i = 0; i < r; ++i) {
            std::fill_n(row_begin(i), c, fill_val);
        }
        init_offsets();
    }

    // From Vector of Strings
    Grid(const std::vector<std::string>& lines, std::size_t pad = 1, T border_val = T{}) 
        : padding(pad), border_value(border_val)
    {
        if (lines.empty()) return;
        rows = lines.size();
        cols = lines[0].size();
        stride = cols + 2 * padding;
        
        // Fill entire grid with border value first
        data.assign((rows + 2 * padding) * stride, border_val);

        // Copy input data into the center
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                operator()(i, j) = static_cast<T>(lines[i][j]);
            }
        }
        init_offsets();
    }

    template <typename Func>
    requires std::invocable<Func, char>
    Grid(const std::vector<std::string>& lines, Func transform, std::size_t pad = 1, T border_val = T{}) 
        : padding(pad), border_value(border_val)
    {
        if (lines.empty()) return;
        rows = lines.size();
        cols = lines[0].size();
        stride = cols + 2 * padding;
        
        // Fill entire grid with border value first
        data.assign((rows + 2 * padding) * stride, border_val);

        // Copy input data into the center using the transformer
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                // Apply the transform function
                operator()(i, j) = static_cast<T>(transform(lines[i][j]));
            }
        }
        init_offsets();
    }

    void init_offsets() {
        int s = static_cast<int>(stride);
        offsets_8 = { -s-1, -s, -s+1, -1, 1, s-1, s, s+1 };
        offsets_4 = { -s, -1, 1, s };
    }

    // Accessors
    
    // 2D Access
    // (0, 0) is top of active content
    // Access (row, column)
    constexpr T& operator()(std::size_t r, std::size_t c) {
        return data[(r + padding) * stride + (c + padding)];
    }
    constexpr const T& operator()(std::size_t r, std::size_t c) const {
        return data[(r + padding) * stride + (c + padding)];
    }

    // 1D Raw Access
    constexpr T& operator[](std::size_t idx) { return data[idx]; }
    constexpr const T& operator[](std::size_t idx) const { return data[idx]; }

    // Iterator to start of row 'r' in the active area
    auto row_begin(size_t r) { return data.begin() + (r + padding) * stride + padding; }

    // Pointer to start of row 'r' in the active area
    auto row_ptr(size_t r) const { return data.data() + (r + padding) * stride + padding; }

    // Coordinate conversions
    constexpr std::size_t index_of(std::size_t r, std::size_t c) const {
        return (r + padding) * stride + (c + padding);
    }

    // Should only use on valid active area indices
    constexpr std::pair<std::size_t, std::size_t> coord_of(std::size_t idx) const {
        std::size_t r = (idx / stride) - padding;
        std::size_t c = (idx % stride) - padding;
        return {r, c};
    }

    // Count neighbors with specific value
    int count_neighbours(std::size_t idx, T val, bool diagonal = true) const {
        int cnt = 0;
        const auto& offs = diagonal ? offsets_8 : offsets_4;
        for (int off : offs) {
            if (data[idx + off] == val) cnt++;
        }
        return cnt;
    }

    T peek(std::size_t idx, int dy, int dx) const {
        return data[idx + (dy * (int)stride) + dx];
    }

    std::size_t find(T val) const {
        auto it = std::ranges::find(data, val);
        if (it == data.end()) return std::string::npos;
        return std::distance(data.begin(), it);
    }

    void print() const {
        for(std::size_t j = 0; j < rows; j++){
            for(std::size_t i = 0; i < cols; i++){
                if constexpr (std::is_same_v<T, char>)
                    std::print("{}", operator()(j, i));
                else
                    std::print("{} ", operator()(j, i));
            }
            std::println("");
        }
    }
};

} // namespace Grid

#endif // GRID_H