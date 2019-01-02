#include "catch.hpp"

#include <cstdint>

#include "../scts/lexical_cast.h"

TEST_CASE("lexical_cast valid conversions", "[lexical_cast]") {
	SECTION("basic data types") {
		REQUIRE(scts::lexical_cast<int>("12") == 12);
		REQUIRE(scts::lexical_cast<int>(std::string("-124")) == -124);
		REQUIRE(scts::lexical_cast<float>(std::string_view("0.5")) == 0.5f);
		REQUIRE(scts::lexical_cast<double>("0.0") == 0.0);
		REQUIRE(scts::lexical_cast<char>("3") == 3);
	}

	SECTION("cstdints are handled properly") {
		// The 8-bit ones cause trouble because the standard library wants to treat them as characters, not integers.
		REQUIRE(scts::lexical_cast<std::int8_t>("-127") == -127);
		REQUIRE(scts::lexical_cast<std::uint8_t>("255") == 255);
		REQUIRE(scts::lexical_cast<std::int32_t>("-8724178") == -8724178);
	}
}

TEST_CASE("lexical_cast invalid conversions", "[lexical_cast]") {
	SECTION("overflows correctly") {
		REQUIRE(scts::lexical_cast<std::uint8_t>("256") == 0);
	}

	SECTION("exceptions are thrown") {
		REQUIRE_THROWS(scts::lexical_cast<int>("12.5124"));
		REQUIRE_THROWS(scts::lexical_cast<float>("15121;sad"));
	}
}