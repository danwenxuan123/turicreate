
// Copyright (C) 2008-2018 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0 (see accompanying
// file LICENSE_1_0.txt or a copy at http://www.boost.org/LICENSE_1_0.txt).
// See: http://www.boost.org/doc/libs/release/libs/contract/doc/html/index.html

// Test throw from free function .pre().

#include "../detail/oteststream.hpp"
#include <boost/contract/function.hpp>
#include <boost/contract/check.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <sstream>

boost::contract::test::detail::oteststream out;

struct err {}; // Global decl so visible in MSVC10 lambdas.

void f() {
    boost::contract::check c = boost::contract::function()
        .precondition([] {
            out << "f::pre" << std::endl;
            throw err(); // Test this throws.
        })
        .old([] { out << "f::old" << std::endl; })
        .postcondition([] { out << "f::post" << std::endl; })
        .except([] { out << "f::except" << std::endl; })
    ;
    out << "f::body" << std::endl;
}

int main() {
    std::ostringstream ok;

    boost::contract::set_precondition_failure(
            [] (boost::contract::from) { throw; });

    try {
        out.str("");
        f();
        #ifndef BOOST_CONTRACT_NO_PRECONDITIONS
                BOOST_TEST(false);
            } catch(err const&) {
        #endif
        ok.str(""); ok
            #ifndef BOOST_CONTRACT_NO_PRECONDITIONS
                << "f::pre" << std::endl // Test this threw.
            #else
                #ifndef BOOST_CONTRACT_NO_OLDS
                    << "f::old" << std::endl
                #endif
                << "f::body" << std::endl
                #ifndef BOOST_CONTRACT_NO_POSTCONDITIONS
                    << "f::post" << std::endl
                #endif
            #endif
        ;
        BOOST_TEST(out.eq(ok.str()));
    } catch(...) { BOOST_TEST(false); }

    return boost::report_errors();
}
