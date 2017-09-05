#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-juicy");
    BOOST_CHECK(GetBoolArg("-juicy"));
    BOOST_CHECK(GetBoolArg("-juicy", false));
    BOOST_CHECK(GetBoolArg("-juicy", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-juicyo"));
    BOOST_CHECK(!GetBoolArg("-juicyo", false));
    BOOST_CHECK(GetBoolArg("-juicyo", true));

    ResetArgs("-juicy=0");
    BOOST_CHECK(!GetBoolArg("-juicy"));
    BOOST_CHECK(!GetBoolArg("-juicy", false));
    BOOST_CHECK(!GetBoolArg("-juicy", true));

    ResetArgs("-juicy=1");
    BOOST_CHECK(GetBoolArg("-juicy"));
    BOOST_CHECK(GetBoolArg("-juicy", false));
    BOOST_CHECK(GetBoolArg("-juicy", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nojuicy");
    BOOST_CHECK(!GetBoolArg("-juicy"));
    BOOST_CHECK(!GetBoolArg("-juicy", false));
    BOOST_CHECK(!GetBoolArg("-juicy", true));

    ResetArgs("-nojuicy=1");
    BOOST_CHECK(!GetBoolArg("-juicy"));
    BOOST_CHECK(!GetBoolArg("-juicy", false));
    BOOST_CHECK(!GetBoolArg("-juicy", true));

    ResetArgs("-juicy -nojuicy");  // -juicy should win
    BOOST_CHECK(GetBoolArg("-juicy"));
    BOOST_CHECK(GetBoolArg("-juicy", false));
    BOOST_CHECK(GetBoolArg("-juicy", true));

    ResetArgs("-juicy=1 -nojuicy=1");  // -juicy should win
    BOOST_CHECK(GetBoolArg("-juicy"));
    BOOST_CHECK(GetBoolArg("-juicy", false));
    BOOST_CHECK(GetBoolArg("-juicy", true));

    ResetArgs("-juicy=0 -nojuicy=0");  // -juicy should win
    BOOST_CHECK(!GetBoolArg("-juicy"));
    BOOST_CHECK(!GetBoolArg("-juicy", false));
    BOOST_CHECK(!GetBoolArg("-juicy", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--juicy=1");
    BOOST_CHECK(GetBoolArg("-juicy"));
    BOOST_CHECK(GetBoolArg("-juicy", false));
    BOOST_CHECK(GetBoolArg("-juicy", true));

    ResetArgs("--nojuicy=1");
    BOOST_CHECK(!GetBoolArg("-juicy"));
    BOOST_CHECK(!GetBoolArg("-juicy", false));
    BOOST_CHECK(!GetBoolArg("-juicy", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-juicy", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-juicy", "eleven"), "eleven");

    ResetArgs("-juicy -bar");
    BOOST_CHECK_EQUAL(GetArg("-juicy", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-juicy", "eleven"), "");

    ResetArgs("-juicy=");
    BOOST_CHECK_EQUAL(GetArg("-juicy", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-juicy", "eleven"), "");

    ResetArgs("-juicy=11");
    BOOST_CHECK_EQUAL(GetArg("-juicy", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-juicy", "eleven"), "11");

    ResetArgs("-juicy=eleven");
    BOOST_CHECK_EQUAL(GetArg("-juicy", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-juicy", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-juicy", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-juicy", 0), 0);

    ResetArgs("-juicy -bar");
    BOOST_CHECK_EQUAL(GetArg("-juicy", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-juicy=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-juicy", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-juicy=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-juicy", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--juicy");
    BOOST_CHECK_EQUAL(GetBoolArg("-juicy"), true);

    ResetArgs("--juicy=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-juicy", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nojuicy");
    BOOST_CHECK(!GetBoolArg("-juicy"));
    BOOST_CHECK(!GetBoolArg("-juicy", true));
    BOOST_CHECK(!GetBoolArg("-juicy", false));

    ResetArgs("-nojuicy=1");
    BOOST_CHECK(!GetBoolArg("-juicy"));
    BOOST_CHECK(!GetBoolArg("-juicy", true));
    BOOST_CHECK(!GetBoolArg("-juicy", false));

    ResetArgs("-nojuicy=0");
    BOOST_CHECK(GetBoolArg("-juicy"));
    BOOST_CHECK(GetBoolArg("-juicy", true));
    BOOST_CHECK(GetBoolArg("-juicy", false));

    ResetArgs("-juicy --nojuicy");
    BOOST_CHECK(GetBoolArg("-juicy"));

    ResetArgs("-nojuicy -juicy"); // juicy always wins:
    BOOST_CHECK(GetBoolArg("-juicy"));
}

BOOST_AUTO_TEST_SUITE_END()
