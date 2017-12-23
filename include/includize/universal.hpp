#ifndef INCLUDIZE_UNIVERSAL_HPP
#define INCLUDIZE_UNIVERSAL_HPP

#include "includize.hpp"
#include "null_stream_preparer.hpp"
#include <regex>

// The universal spec will look like this:
//
//     [[ #includize "path/to/some/file.inc" ]]
//
// This should be unique enough that it can be used as-is in most situations.
// Obviously if this is not good enough for a use case, another specification
// can be used.  Additionally it will remove any trailing text on that
// particular line for no reason other than I like it that way.

namespace includize
{
template < typename CHAR_TYPE >
struct universal_spec
{
};

template <>
struct universal_spec< char >
{
    static constexpr char header_start() { return '['; }
    static constexpr const char *regex()
    {
        return R"..(\[\s*#includize\s*"(([^"]|\")+)"\s*]])..";
    }

    static constexpr std::size_t file_name_index() { return 1; };

    static constexpr bool discard_characters_after_include() { return true; }

    static std::string convert_filename(const std::string &str) { return str; }

    static std::string unescape_filename(const std::string &str)
    {
        return std::regex_replace(str, std::regex("\\\\\""), "\"");
    }
};

template < typename CHAR_T,
           typename TRAITS = std::char_traits< CHAR_T >,
           typename STREAM_PREPARER = null_stream_preparer< CHAR_T, TRAITS > >
using basic_universal_preprocessor =
    basic_preprocessor< universal_spec< CHAR_T >,
                        CHAR_T,
                        TRAITS,
                        STREAM_PREPARER >;

using universal_preprocessor = basic_universal_preprocessor< char >;

}  // namespace includize

#endif