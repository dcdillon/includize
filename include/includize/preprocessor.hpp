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

#ifndef INCLUDIZE_PREPROCESSOR_HPP
#define INCLUDIZE_PREPROCESSOR_HPP

#include "null_stream_preparer.hpp"
#include "streambuf.hpp"

#include <memory>

namespace includize
{
template < typename INCLUDE_SPEC,
           typename CHAR_T,
           typename TRAITS = std::char_traits< CHAR_T >,
           typename STREAM_PREPARER = null_stream_preparer< CHAR_T, TRAITS > >
class basic_preprocessor
{
public:
    using stream_preparer_type = STREAM_PREPARER;
    using include_spec_type = INCLUDE_SPEC;
    using char_type = CHAR_T;
    using traits_type = TRAITS;

    using istream_type = typename std::basic_istream< char_type, traits_type >;
    using ifstream_type =
        typename std::basic_ifstream< char_type, traits_type >;
    using streambuf_type = basic_streambuf< include_spec_type,
                                            char_type,
                                            traits_type,
                                            stream_preparer_type >;

public:
    basic_preprocessor(const std::string &file_name)
    {
        std::string path = "";

        if (file_name[0] != '/')
        {
            char buf[8192];
            if (getcwd(buf, 8192))
            {
                std::string path = buf;

                if (*path.rbegin() != '/')
                {
                    path += '/';
                }
            }
        }

        path += extract_path(file_name);

        fstream_.reset(new ifstream_type(file_name.c_str(),
                                         std::ios::in | std::ios::binary));
        stream_preparer_type::prepare_ifstream(*fstream_);
        streambuf_.reset(new streambuf_type(*fstream_, path));
        stream_.reset(new istream_type(streambuf_.get()));
    }

    istream_type &stream() { return *stream_; }

    operator istream_type &() { return *stream_; }

private:
    static std::string extract_path(const std::string file_name)
    {
        std::string::size_type pos = file_name.rfind("/");
        std::string path =
            (pos != std::string::npos) ? file_name.substr(0, pos + 1) : "";

        return path;
    }

    std::unique_ptr< istream_type > stream_;
    std::unique_ptr< ifstream_type > fstream_;
    std::unique_ptr< streambuf_type > streambuf_;
};
}

#endif
