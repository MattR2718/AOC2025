#ifndef STRING_UTILS_H
#define STRING_UTILS_H
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <numeric>
#include <ranges>


namespace StringUtils {

// Single number parser
template<typename T = int64_t>
T to_num(std::string_view sv) {
    T val = 0;
    std::from_chars(sv.data(), sv.data() + sv.size(), val);
    return val;
}

// Trim spaces from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Trim spaces from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Trim spaces from both ends (in place)
inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// Trim spaces from start
inline std::string_view ltrim(std::string_view s) {
    auto it = std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    // Shrink the view from the left
    s.remove_prefix(std::distance(s.begin(), it));
    return s;
}

// Trim spaces from end
inline std::string_view rtrim(std::string_view s) {
    auto it = std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    // Shrink the view from the right
    s.remove_suffix(std::distance(s.rbegin(), it));
    return s;
}

// Trim spaces from both ends
inline std::string_view trim(std::string_view s) {
    return ltrim(rtrim(s));
}

// Split string by delimiter
inline std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Split string_view by delimiter into a vector of views
inline std::vector<std::string_view> split(std::string_view s, char delimiter) {
    std::vector<std::string_view> parts;
    size_t start = 0;
    size_t end;
    while ((end = s.find(delimiter, start)) != std::string_view::npos) {
        parts.push_back(s.substr(start, end - start));
        start = end + 1;
    }
    // Add the last part
    parts.push_back(s.substr(start));
    return parts;
}

// Split string_view by a string delimiter (e.g. ", ")
inline std::vector<std::string_view> split(std::string_view s, std::string_view delimiter) {
    std::vector<std::string_view> parts;
    size_t start = 0;
    size_t end;
    while ((end = s.find(delimiter, start)) != std::string_view::npos) {
        parts.push_back(s.substr(start, end - start));
        start = end + delimiter.size();
    }
    parts.push_back(s.substr(start));
    return parts;
}

template <typename T = int64_t>
std::vector<T> extract_numbers(std::string_view sv) {
    std::vector<T> numbers;
    const char* ptr = sv.data();
    const char* end = sv.data() + sv.size();

    while (ptr < end) {
        // Skip non-digit characters
        // If T is signed,  allow '-' as a start char if followed by a digit
        bool is_sign = false;
        if constexpr (std::is_signed_v<T>) {
            if (*ptr == '-') {
                if (ptr + 1 < end && std::isdigit(*(ptr + 1))) {
                    is_sign = true;
                }
            }
        }

        if (!std::isdigit(*ptr) && !is_sign) {
            ++ptr;
            continue;
        }

        // Parse the number
        T value;
        auto result = std::from_chars(ptr, end, value);

        if (result.ec == std::errc()) {
            numbers.push_back(value);
            ptr = result.ptr; // Move pointer to end of parsed number
        } else {
            ++ptr; 
        }
    }
    return numbers;
}

} // namespace StringUtils
#endif // STRING_UTILS_H