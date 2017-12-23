#ifndef INCLUDIZE_WUNIVERSAL_HPP
#define INCLUDIZE_WUNIVERSAL_HPP

#include "../universal.hpp"
#include <codecvt>

namespace includize
{
template <>
struct universal_spec< wchar_t >
{
    static constexpr wchar_t header_start() { return L'['; }
    static constexpr const wchar_t *regex()
    {
        return LR"..(\[\s*#includize\s*"(([^"]|\")+)"\s*]])..";
    }

    static constexpr std::size_t file_name_index() { return 1; };

    static constexpr bool discard_characters_after_include() { return true; }

    static std::string convert_filename(const std::wstring &str)
    {
        std::wstring_convert< std::codecvt_utf8_utf16< wchar_t >, wchar_t >
            converter;
        return converter.to_bytes(str);
    }

    static std::string unescape_filename(const std::string &str)
    {
        return std::regex_replace(str, std::regex("\\\\\""), "\"");
    }
};

}  // namespace includize

#endif