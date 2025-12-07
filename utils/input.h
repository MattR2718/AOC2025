#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <print>
#include <string_view>
#include <concepts>
#include <cstdio>


namespace InputUtils {

// Helper to read entire stream to string
inline std::string read_stream(std::istream& in) {
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

// Require Func to be invocable with (std::string_view, Ret&)
template<typename Func, typename Ret>
concept LineParser = std::invocable<Func, std::string_view, Ret&>;

// Generic input parser
// PARSE_LINE_FUNCTION should match LineParser<RETURN_TYPE>
template<typename RETURN_TYPE, LineParser <RETURN_TYPE> PARSE_LINE_FUNCTION>
RETURN_TYPE parse_input(PARSE_LINE_FUNCTION& parse_line, std::string input_file_path = ""){
    RETURN_TYPE ret;
    std::string file_buffer;
    std::string_view content_view;
    bool loaded_source = false;

    // Prioritise command line file if provided
    if (!input_file_path.empty()) {
        std::println("Using file: {}", input_file_path);
        std::ifstream f(input_file_path);
        if(f.is_open()) {
            file_buffer = read_stream(f);
            content_view = file_buffer;
            loaded_source = true;
        } else {
            std::println(stderr, "Error: Failed to open file '{}'.", input_file_path);
            return ret;
        }
    }
    
    if(!loaded_source){
#if defined(__cpp_pp_embed) && defined(AOC_INPUT_FILE_PATH) && !defined(FORCE_FILE_IO)
    // Use embedded file
    // FORCE_FILE_IO can be used to override
    static constexpr const char _input[] = {
        #embed AOC_INPUT_FILE_PATH
        , '\0'   
    };
    content_view = std::string_view(_input);

    // Prevent unused variable warning
    (void)input_file_path;
    if (!content_view.empty() && content_view.back() == '\0') content_view.remove_suffix(1);
    std::println("Using embedded input");
#else
    std::println("Reading from std::cin");
    file_buffer = read_stream(std::cin);
    content_view = file_buffer;

#endif
    }

    std::size_t start = 0;
    std::size_t end = 0;
    
    // Read line by line and parse
    while ((end = content_view.find('\n', start)) != std::string_view::npos) {
        auto line = content_view.substr(start, end - start);
        
        if (!line.empty() && line.back() == '\r') {
            line.remove_suffix(1);
        }

        parse_line(line, ret);
        start = end + 1;
    }

    // Handle potential last line with no newline at EOF
    if (start < content_view.size()) {
        auto line = content_view.substr(start);
        if (!line.empty() && line.back() == '\r') line.remove_suffix(1);
        parse_line(line, ret);
    }

    return ret;
}

} // namespace InputUtils


#endif // INPUT_H