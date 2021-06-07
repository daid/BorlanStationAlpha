#pragma once

#include <string>
#include <string_view>


static inline void format_to(std::string& str, std::string_view fmt)
{
    str += fmt;
}

static inline std::string format_to_string(const std::string& s)
{
    return s;
}

static inline std::string format_to_string(char c)
{
    return std::string{c};
}

static inline std::string format_to_string(int i)
{
    return std::to_string(i);
}

static inline std::string format_to_string(float f)
{
    return std::to_string(f);
}

template<typename T, typename... ARGS>
void format_to(std::string& str, std::string_view fmt, const T& arg, const ARGS&... args)
{
    for(size_t n=0; n<fmt.size(); n++) {
        if (fmt[n] == '@') {
            str += fmt.substr(0, n);
            str += format_to_string(arg);
            format_to(str, fmt.substr(n + 1), args...);
            return;
        }
    }
    str += fmt;
}

template<typename... ARGS>
std::string format(std::string_view fmt, const ARGS&... args)
{
    std::string result;
    result.reserve(fmt.size());
    format_to(result, fmt, args...);
    return result;
}
