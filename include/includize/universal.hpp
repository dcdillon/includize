/* Copyright (c) 2017, Daniel C. Dillon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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