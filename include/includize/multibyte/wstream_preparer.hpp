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