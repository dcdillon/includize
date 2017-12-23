#ifndef INCLUDIZE_HPP
#define INCLUDIZE_HPP

#include "preprocessor.hpp"
#include "streambuf.hpp"
#include "universal.hpp"

namespace includize
{
template < typename INCLUDE_SPEC >
using streambuf = basic_streambuf< INCLUDE_SPEC, char >;

template < typename INCLUDE_SPEC >
using preprocessor = basic_preprocessor< INCLUDE_SPEC, char >;
}

#endif
