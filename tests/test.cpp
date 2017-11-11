#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "cpptoml.h"

#include "../include/includize/toml.hpp"

#include <fstream>
#include <sstream>
#include <codecvt>
#include <memory>

std::string convert(const std::wstring &str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.to_bytes(str);
}

TEST_CASE("toml", "[toml]")
{
    SECTION("char")
    {
        using streambuf_type = includize::toml_streambuf;
        using ifstream_type = std::ifstream;
        using istream_type = std::istream;
        
        ifstream_type raw_infile("tests/base.toml", std::ios::in | std::ios::binary);
        streambuf_type streambuf = streambuf_type(raw_infile, "tests/");
        
        REQUIRE(raw_infile.is_open());
        
        istream_type infile(&streambuf);
        std::ifstream orig_infile("tests/orig.toml");
        
        cpptoml::parser included_parser(infile);
        cpptoml::parser orig_parser(orig_infile);
        
        std::shared_ptr< cpptoml::table > parsed = included_parser.parse();
        std::shared_ptr< cpptoml::table > orig = orig_parser.parse();
        
        std::ostringstream with_includes;
        with_includes << (*parsed);
        
        std::ostringstream without_includes;
        without_includes << (*orig);
        
        REQUIRE(with_includes.str() == without_includes.str());
    }
    
    SECTION("wchar_t")
    {
        using streambuf_type = includize::wtoml_streambuf;
        using ifstream_type = std::wifstream;
        using istream_type = std::wistream;
        
        ifstream_type raw_infile("tests/wbase.toml", std::ios::in | std::ios::binary);
        raw_infile.imbue(std::locale(raw_infile.getloc(),
            new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
        
        REQUIRE(raw_infile.is_open());
        
        streambuf_type streambuf = streambuf_type(raw_infile, "tests/");
        
        istream_type infile(&streambuf);
        std::ifstream orig_infile("tests/orig.toml");
        
        std::ostringstream converted;
        
        while (infile)
        {
            std::wstring line;
            std::getline(infile, line);
            converted << convert(line) << std::endl;
        }
        
        std::istringstream converted_infile(converted.str());
        
        cpptoml::parser included_parser(converted_infile);
        cpptoml::parser orig_parser(orig_infile);
        
        std::shared_ptr< cpptoml::table > parsed = included_parser.parse();
        std::shared_ptr< cpptoml::table > orig = orig_parser.parse();
        
        std::ostringstream with_includes;
        with_includes << (*parsed);
        
        std::ostringstream without_includes;
        without_includes << (*orig);
        
        REQUIRE(with_includes.str() == without_includes.str());
    }
}