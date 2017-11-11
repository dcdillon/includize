#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <regex>
#include <cassert>

#include "cpptoml.h"
#include "includize.hpp"

template< typename CHAR_TYPE >
struct toml_spec
{
};

template<>
struct toml_spec< char >
{
    static constexpr char header_start() { return '#'; }
    static constexpr const char *regex()
    {
        return R"..(\s*\[\[include\s*"(([^"]|\")+)"\s*]])..";
    }
    
    static constexpr std::size_t file_name_index() { return 1; };
    static constexpr bool discard_characters_after_include() { return true; }
};

template<>
struct toml_spec< wchar_t >
{
    static constexpr wchar_t header_start() { return L'#'; }
    static constexpr const wchar_t *regex()
    {
        return LR"..(\s*\[\[include\s*"(([^"]|\")+)"\s*]])..";
    }
    
    static constexpr std::size_t file_name_index() { return 1; };
    static constexpr bool discard_characters_after_include() { return true; }
};



int main(int argc, char *argv[])
{
#ifdef INCLUDIZE_UTF16
    using char_type = wchar_t;
#define OUTSTREAM std::wcout
#else
    using char_type = char;
#define OUTSTREAM std::cout
#endif

    using streambuf = includize::basic_streambuf< toml_spec< char_type >, char_type >;
    
    streambuf::ifstream_type raw_infile(argv[1], std::ios::in | std::ios::binary);
    
#ifdef INCLUDIZE_UTF16
    raw_infile.imbue(std::locale(raw_infile.getloc(),
        new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
#endif
    
    streambuf buf(raw_infile);
    streambuf::istream_type infile(&buf);

#ifndef INCLUDIZE_UTF16    
    cpptoml::parser parser(infile);
    
    std::shared_ptr< cpptoml::table > table = parser.parse();
    
    OUTSTREAM << (*table) << std::endl;
#else

    std::wostringstream out;
        
    while (infile.good())
    {
        streambuf::string_type line;
        std::getline(infile, line);
        out << line << std::endl;
    }
    
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    std::string converted_output = converter.to_bytes(out.str());
    
    std::istringstream parsable(converted_output);
    
    cpptoml::parser parser(parsable);
    
    std::shared_ptr< cpptoml::table > table = parser.parse();
    
    std::cout << (*table) << std::endl;
#endif
    
    return 0;
}
