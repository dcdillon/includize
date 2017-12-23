# includize - C++ Stream for Preprocessing Include Directives

### Short Description

`includize` is a header-only C++ library that allows for include directive processing in a C++ `std::istream`.  

Some languages do not come with their own include directives (markdown, yaml, json, etc.) yet it is often quite useful to have them.  I found myself writing preprocessors for the files in python or perl that created temporary files which would then be fed to a C++ process and used.  It seemed like there should be some way to keep this whole process transparent in C++, thus `includize` was born.

`includize` allows a user to define a fairly simple specification for an include directive using regular expressions and have it then simply apply to a C++ `std::istream` (usually a `std::ifstream` as there needs to be some sort of anchor for include paths).  Once specified, an `includize::preprocessor` can be instantiated which will simply include the appropriate text as it is read.

### Example - Unversal

`includize` defines a universal include directive that we can use if we so choose.

```c++
#include <includize/includize.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    using preprocessor_type = includize::universal_preprocessor;

    preprocessor_type pp("base.txt")

    while (pp.stream())
    {
        std::string line;
        std::getline(pp, line);
        std::cout << line << std::endl;
    }

    return 0;
}
```

Which will process the following file as indicated:

**base.txt**
```
This is an arbitrary file in which I'd like to include some text from other
files.  Particularly I'd like to include:

[[ #includize "included.txt" ]]

This way I can easily include some boilerplate text into any file!
```

**included.txt**
```
This wonderful included text that I want to store in one place an use in many
different files.
```

**generated output**
```
This is an arbitrary file in which I'd like to include some text from other
files.  Particularly I'd like to include:

This wonderful included text that I want to store in one place an use in many
different files.

This way I can easily include some boilerplate text into any file!

```

### Example - Custom

In some cases, the universal solution will not be quite what we want.  In this case, we can create our own custom include directive.

The [TOML](https://github.com/toml-lang/toml) language has no include directive, and, in fact, has indicated that providing one would go against the "M" in TOML (Minimal).  This does not mean that such a directive would be useless in practice.  So we define an `IncludeSpec` which is more or less a concept as follows:

```c++
struct toml_spec_char
{
    static constexpr char header_start() { return '#'; }
    static constexpr const char *regex()
    {
        return R"..(\s*\[\[include\s*"(([^"]|\")+)"\s*]])..";
    }

    static constexpr std::size_t file_name_index() { return 1; };
    static constexpr bool discard_characters_after_include() { return true; }

    static std::string convert_filename(const std::string &str) { return str; }

    static std::string unescape_filename(const std::string &str)
    {
        return std::regex_replace(str, std::regex("\\\\\""), "\"");
    }
};
```

This class contains several `static` methods that allow `includize` to pares include directives of the form `# [[include "path/to/file"]]` within an input stream.

   * `static char header_start()` - tells `includize` that if it sees this particular character, it _may_ be the start of an include directive and to start looking for one.  In this example `#` seems like a good choice because it is used for line-comments in TOML.  This is convenient because by hiding our include directive behind a comment in the native language, we don't affect the validity of the document we create even if we sprinkle our new include directives in.
   * `static const char *regex()` - defines a regular expression that matches the text following `header_start()` when it is an include directive.  It *must* have some group that can be extracted as the file name.  In this case we grab the text within the quotation marks.  We also allow for escaping of quotation marks within the file name itself.
   * `static std::size_t file_name_index()` - this tells `includize` which `group()` within the `std::regex` match will contain the file name.  In this case there is only one group in the regular expression, so we use that one.
   * `static bool discard_characters_after_include()` - If this function returns true, characters after the include directive, but on the same line in the file, will be discarded by the stream.  Because we started this include directive with a line-comment character, it makes sense that any extraneous characters should be excluded, but this need not be the case for every implementation.
   * `static std::string convert_filename(const std::string &str)` - this function takes a filename as read from the regex (so it may well be a `std::wstring` for multibyte character implementations) and converts it to a `std::string` which is what needs to be passed as a file name to both `std::ifstream` and `std::wifstream`.  `includize` must be able to read text from the native stream, turn it into a file name, and open it, so we will need to be able to make this conversion.  In this case, no conversion is necessary and we simply return what was passed in.
   * `static::std::string unescape_filename(const std::string &str)` - this function should always take a `std::string` as a parameter (the output of `convert_filename()` in fact) and if the user wants to allow for any escape characters, replace them with the approparite representation.  In this case, we replace `\"` with `"`.
   
Once we have created this `IncludeSpec` we can write code as follows to output the file, with all include directives processed to `stdout`.  Obviously this is trivial usage.  The better usage is to pass the `preprocessor` to a parser of the given language which should transparently work (and include the appropriate files as we have specified them).

```c++
#include <includize/includize.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    using preprocessor_type = includize::preprocessor< toml_spec_char >;

    preprocessor_type pp("base.toml")

    while (pp.stream())
    {
        std::string line;
        std::getline(pp, line);
        std::cout << line << std::endl;
    }
    
    return 0;
}
```

For completeness, here is some example input and output adapted from the example files at the [TOML](https://github.com/toml-lang/toml) repo.

**base.toml**
```toml
################################################################################
## Comment

# Speak your mind with the hash symbol. They go from the symbol to the end of
# the line.


################################################################################
## Table

# Tables (also known as hash tables or dictionaries) are collections of
# key/value pairs. They appear in square brackets on a line by themselves.

[table]

key = "value" # Yeah, you can do this.

# [[include "included.toml"]]
```

**included.toml**
```toml
# Nested tables are denoted by table names with dots in them. Name your tables
# whatever crap you please, just don't use #, ., [ or ].

[table.subtable]

key = "another value"
```

**generated output**
```toml
################################################################################
## Comment

# Speak your mind with the hash symbol. They go from the symbol to the end of
# the line.


################################################################################
## Table

# Tables (also known as hash tables or dictionaries) are collections of
# key/value pairs. They appear in square brackets on a line by themselves.

[table]

key = "value" # Yeah, you can do this.

# Nested tables are denoted by table names with dots in them. Name your tables
# whatever crap you please, just don't use #, ., [ or ].

[table.subtable]

key = "another value"
```

It should be noted that relative file paths in `includize` include directives are processed with respect to the path of the including file and absolute paths are processed as absolute paths.

### Future Plans

   * The interface of `IncludeSpec` doesn't seem to be quite satisfactory, so may undergo some changes in the near future.
   * Multibyte character support works (kinda).  It should be made to work better.
   * As this is my first foray into customized stream work in C++ I'm sure there are things that can be done far better, however the functionality should remain the same.

### Licensing

`inlcludize` is distributed under the [New BSD](http://opensource.org/licenses/BSD-3-Clause) (or BSD 3-Clause) license.
