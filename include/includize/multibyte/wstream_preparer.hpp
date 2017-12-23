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

#ifndef INCLUDIZE_WSTREAM_PREPARER_HPP
#define INCLUDIZE_WSTREAM_PREPARER_HPP

#include <codecvt>
#include <fstream>

namespace includize
{
struct wstream_utf16_header_preparer
{
    static void prepare_ifstream(std::basic_ifstream< wchar_t > &s)
    {
        s.imbue(std::locale(
            s.getloc(),
            new std::codecvt_utf16< wchar_t, 0x10ffff, std::consume_header >));
    }
};

struct wstream_utf16_big_endian_preparer
{
    static void prepare_ifstream(std::basic_ifstream< wchar_t > &s)
    {
        s.imbue(std::locale(s.getloc(),
                            new std::codecvt_utf16< wchar_t, 0x10ffff >));
    }
};

struct wstream_utf16_little_endian_preparer
{
    static void prepare_ifstream(std::basic_ifstream< wchar_t > &s)
    {
        s.imbue(std::locale(
            s.getloc(),
            new std::codecvt_utf16< wchar_t, 0x10ffff, std::little_endian >));
    }
};
}

#endif