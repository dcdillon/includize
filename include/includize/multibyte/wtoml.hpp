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

#ifndef INCLUDIZE_WTOML_HPP
#define INCLUDIZE_WTOML_HPP

#include "../toml.hpp"
#include <codecvt>

namespace includize
{
template <>
struct toml_spec< wchar_t >
{
    static constexpr wchar_t header_start() { return L'#'; }
    static constexpr const wchar_t *regex()
    {
        return LR"..(\s*\[\[include\s*"(([^"]|\")+)"\s*]])..";
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