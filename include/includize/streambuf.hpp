#ifndef INCLUDIZE_STREAMBUF_HPP
#define INCLUDIZE_STREAMBUF_HPP

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <unistd.h>

#include "null_stream_preparer.hpp"

namespace includize
{
template < typename INCLUDE_SPEC,
           typename CHAR_T,
           typename TRAITS = std::char_traits< CHAR_T >,
           typename STREAM_PREPARER = null_stream_preparer< CHAR_T, TRAITS > >
class basic_streambuf : public std::basic_streambuf< CHAR_T, TRAITS >
{
public:
    using stream_preparer_type = STREAM_PREPARER;
    using include_spec_type = INCLUDE_SPEC;
    using base_type = typename std::basic_streambuf< CHAR_T, TRAITS >;
    using char_type = typename base_type::char_type;
    using traits_type = typename base_type::traits_type;
    using int_type = typename base_type::int_type;
    using pos_type = typename base_type::pos_type;
    using off_type = typename base_type::off_type;
    using istream_type = typename std::basic_istream< char_type, traits_type >;
    using ifstream_type =
        typename std::basic_ifstream< char_type, traits_type >;
    using string_type = typename std::basic_string< char_type, traits_type >;
    using regex_type = typename std::basic_regex< char_type >;
    using regex_match_type =
        typename std::match_results< typename string_type::const_iterator >;

public:
    basic_streambuf(std::basic_istream< char_type, traits_type > &s,
                    const std::string &path = "")
        : stream_(s)
        , included_file_(NULL)
        , included_file_pp_(NULL)
        , included_stream_(NULL)
    {
        base_type::setg(nullptr, nullptr, nullptr);

        path_ = path;

        if (path.size() && !(*path.rbegin() == '/'))
        {
            path_ += "/";
        }
    }

    basic_streambuf(basic_streambuf &&) = default;
    basic_streambuf(basic_streambuf &) = delete;

    ~basic_streambuf() { remove_included_stream(); }

protected:
    int_type underflow() override
    {
        buffer_next();

        if (!included_buffer_.empty())
        {
            return *included_buffer_.begin();
        }
        else if (!buffer_.empty())
        {
            int_type c = *buffer_.begin();
            buffer_.erase(buffer_.begin());

            if (check_for_include(c))
            {
                return buffer_next();
            }
            else
            {
                buffer_.insert(0, 1, static_cast< char_type >(c));

                if (buffer_.empty())
                {
                    buffer_next();
                }

                return c;
            }

            return *buffer_.begin();
        }

        return traits_type::eof();
    }

    int_type uflow() override
    {
        underflow();

        if (!included_buffer_.empty())
        {
            int_type c = *included_buffer_.begin();
            included_buffer_.erase(included_buffer_.begin());

            if (included_buffer_.empty())
            {
                underflow();
            }

            return c;
        }

        if (!buffer_.empty())
        {
            int_type c = *buffer_.begin();
            buffer_.erase(buffer_.begin());

            if (check_for_include(c))
            {
                return uflow();
            }
            else
            {
                if (buffer_.empty())
                {
                    buffer_next();
                }

                return c;
            }
        }

        return traits_type::eof();
    }

private:
    int_type buffer_next()
    {
        if (included_file_pp_)
        {
            int_type c = included_stream_->get();

            if (c != traits_type::eof())
            {
                included_buffer_.push_back(c);
                return *included_buffer_.begin();
            }

            remove_included_stream();
        }

        int_type c = get_next_from_stream();

        if (c != traits_type::eof())
        {
            buffer_.push_back(c);
            return *buffer_.begin();
        }

        assert(c == traits_type::eof());
        return c;
    }

    int_type get_next_from_stream()
    {
        if (stream_.good())
        {
            const int_type c = stream_.get();
            return c;
        }

        return traits_type::eof();
    }

    void remove_included_stream()
    {
        if (included_file_pp_)
        {
            delete included_stream_;
            delete included_file_pp_;
            delete included_file_;

            included_stream_ = nullptr;
            included_file_pp_ = nullptr;
            included_file_ = nullptr;
        }
    }

    bool open_included_stream(const string_type &file_name)
    {
        std::string name = include_spec_type::unescape_filename(
            include_spec_type::convert_filename(file_name));

        std::string path = get_file_path(name);

        if (name[0] != '/')
        {
            name = path_ + name;
        }

        included_file_ =
            new ifstream_type(name.c_str(), std::ios::in | std::ios::binary);
        stream_preparer_type::prepare_ifstream(*included_file_);
        included_file_pp_ = new basic_streambuf(*included_file_, path);
        included_stream_ = new istream_type(included_file_pp_);

        if (included_stream_->good())
        {
            buffer_next();
            return true;
        }

        return false;
    }

    void buffer_line_from_stream()
    {
        do
        {
            int_type c = get_next_from_stream();

            if (c == traits_type::eof())
            {
                break;
            }
            else if (static_cast< char_type >(c) != stream_.widen('\n'))
            {
                buffer_.push_back(c);
            }
            else
            {
                buffer_.push_back(c);
                break;
            }
        } while (true);
    }

    bool check_for_include(int_type c)
    {
        if (c == include_spec_type::header_start())
        {
            string_type line;

            typename string_type::size_type pos = string_type::npos;

            if (!buffer_.empty())
            {
                pos = buffer_.find(stream_.widen('\n'));

                if (pos != string_type::npos)
                {
                    line = buffer_.substr(0, pos);
                }
                else
                {
                    buffer_line_from_stream();
                    line = buffer_;
                }
            }
            else
            {
                buffer_line_from_stream();
                line = buffer_;
            }

            regex_match_type match;

            if (std::regex_search(
                    line, match, regex_type(include_spec_type::regex())))
            {
                string_type file_name =
                    match[include_spec_type::file_name_index()];

                if (pos != string_type::npos && pos < buffer_.size())
                {
                    buffer_.erase(0, pos);
                }
                else
                {
                    buffer_.clear();
                }

                if (!include_spec_type::discard_characters_after_include())
                {
                    buffer_ += match.suffix();
                }

                return open_included_stream(file_name);
            }
        }

        return false;
    }

    std::string get_file_path(const std::string file_name)
    {
        if (file_name.length())
        {
            std::string::size_type pos = file_name.rfind("/");
            std::string path =
                (pos != std::string::npos) ? file_name.substr(0, pos + 1) : "";

            return (file_name[0] != '/') ? path_ + path : path;
        }

        return "";
    }

private:
    istream_type &stream_;
    ifstream_type *included_file_;
    basic_streambuf *included_file_pp_;
    istream_type *included_stream_;
    string_type included_buffer_;
    string_type buffer_;
    std::string path_;
};
}

#endif