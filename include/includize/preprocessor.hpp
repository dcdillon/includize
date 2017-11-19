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
};

#endif