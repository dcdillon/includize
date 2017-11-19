#ifndef INCLUDIZE_NULL_STREAM_PREPARER_HPP
#define INCLUDIZE_NULL_STREAM_PREPARER_HPP

namespace includize
{
template < typename CHAR_T, typename TRAITS = std::char_traits< CHAR_T > >
struct null_stream_preparer
{
    static void prepare_ifstream(std::basic_ifstream< CHAR_T, TRAITS > &s) {}
};
}

#endif