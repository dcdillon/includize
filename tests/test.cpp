#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "cpptoml.h"

#include "../include/includize/includize.hpp"
#include "../include/includize/multibyte/wstream_preparer.hpp"
#include "../include/includize/multibyte/wtoml.hpp"
#include "../include/includize/multibyte/wuniversal.hpp"

#include <codecvt>
#include <fstream>
#include <memory>
#include <sstream>

std::string convert(const std::wstring &str)
{
    std::wstring_convert< std::codecvt_utf8_utf16< wchar_t >, wchar_t >
        converter;
    return converter.to_bytes(str);
}

TEST_CASE("toml", "[toml]")
{
    SECTION("char")
    {
        using preprocessor_type = includize::toml_preprocessor;

        preprocessor_type pp("tests/base.toml");
        std::ifstream orig_infile("tests/orig.toml");

        cpptoml::parser included_parser(pp);
        cpptoml::parser orig_parser(orig_infile);

        std::shared_ptr< cpptoml::table > parsed = included_parser.parse();
        std::shared_ptr< cpptoml::table > orig = orig_parser.parse();

        std::ostringstream with_includes;
        with_includes << (*parsed);

        std::ostringstream without_includes;
        without_includes << (*orig);

        REQUIRE(without_includes.str() != "");
        REQUIRE(with_includes.str() == without_includes.str());
    }

    SECTION("wchar_t")
    {
        using preprocessor_type = includize::basic_preprocessor<
            includize::toml_spec< wchar_t >,
            wchar_t,
            std::char_traits< wchar_t >,
            includize::wstream_utf16_header_preparer >;

        preprocessor_type pp("tests/wbase.toml");
        std::ifstream orig_infile("tests/orig.toml");

        std::ostringstream converted;

        while (pp.stream())
        {
            std::wstring line;
            std::getline< std::wstring::value_type,
                          std::wstring::traits_type,
                          std::wstring::allocator_type >(pp, line);
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

        REQUIRE(without_includes.str() != "");
        REQUIRE(with_includes.str() == without_includes.str());
    }
}

TEST_CASE("universal", "[universal]")
{
    SECTION("char")
    {
        using preprocessor_type = includize::universal_preprocessor;

        preprocessor_type pp("tests/base.txt");
        std::ifstream orig("tests/orig.txt");

        std::ostringstream with_includes;

        while (pp.stream())
        {
            std::string line;
            std::getline(pp.stream(), line);
            with_includes << line << std::endl;
            std::cout << line << std::endl;
        }

        std::cout << std::endl;

        std::ostringstream without_includes;

        while (orig.good())
        {
            std::string line;
            std::getline(orig, line);
            without_includes << line << std::endl;
            std::cout << line << std::endl;
        }

        REQUIRE(without_includes.str() != "");
        REQUIRE(with_includes.str() == without_includes.str());
    }

    SECTION("wchar_t")
    {
        using preprocessor_type = includize::basic_preprocessor<
            includize::universal_spec< wchar_t >,
            wchar_t,
            std::char_traits< wchar_t >,
            includize::wstream_utf16_header_preparer >;

        preprocessor_type pp("tests/wbase.txt");
        std::ifstream orig("tests/orig.txt");

        std::ostringstream with_includes;

        while (pp.stream())
        {
            std::wstring line;
            std::getline< std::wstring::value_type,
                          std::wstring::traits_type,
                          std::wstring::allocator_type >(pp, line);
            with_includes << convert(line) << std::endl;
        }

        std::cout << std::endl;

        std::ostringstream without_includes;

        while (orig.good())
        {
            std::string line;
            std::getline(orig, line);
            without_includes << line << std::endl;
            std::cout << line << std::endl;
        }

        REQUIRE(without_includes.str() != "");
        REQUIRE(with_includes.str() == without_includes.str());
    }
}