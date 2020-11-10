// -*- mode: c++ -*-

#ifndef YPDF_EXAMPLES_OPTIONS_HH
#define YPDF_EXAMPLES_OPTIONS_HH

#include <memory>

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>
namespace po = boost::program_options;

struct options_t
{
    using value_type = ::po::variable_value;

    using reference = value_type &;
    using const_reference = const value_type &;

public:
    options_t();
    options_t(int, char **);

    bool have(const char *) const;

    const_reference operator[](const char *key) const;

    const boost::program_options::options_description &description() const
    {
        return *desc_;
    }

private:
    ::po::variables_map                         map_;
    std::shared_ptr< ::po::options_description > desc_;
};

const options_t &global_options();

options_t global_options(options_t);

inline bool have_global_option(const char *s)
{
    return global_options().have(s);
}

inline bool have_global_option(const std::string &s)
{
    return have_global_option(s.c_str());
}

#endif // YPDF_EXAMPLES_OPTIONS_HH
