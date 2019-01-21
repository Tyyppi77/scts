#include "catch.hpp"

#include "test_objects.h"

TEST_CASE("basic binary serialization and deserialization", "[binary_formatter]") {
	base_object a{ 0.35, 12 };
	auto a_stream = scts::serialize<base_object, scts::binary_formatter>(a);
	base_object b;
	scts::deserialize(b, a_stream.get_in_stream());
	auto b_stream = scts::serialize(b);

	REQUIRE(a == b);
	REQUIRE(a_stream.str() == b_stream.str());
}

TEST_CASE("inheritance", "[binary_formatter]") {
	derived_object a{ -124.1, 76, 0.15f, "hello" };
	auto a_stream = scts::serialize<derived_object, scts::binary_formatter>(a);
	derived_object b;
	scts::deserialize<derived_object, scts::binary_formatter>(b, a_stream.get_in_stream());
	auto b_stream = scts::serialize<derived_object, scts::binary_formatter>(b);

	REQUIRE(a == b);
	REQUIRE(a_stream.str() == b_stream.str());
}

TEST_CASE("binary_formatter supports all required types", "[binary_formatter]") {
	complete_object a{
		"cool{string[with]specialcharacters,",
		true,
		255,
		state::moving,
		nullptr,
		{15.0f, -1.0f / 3.0f},
		{base_object{1.0, -124}, base_object{-35.23, 0}},
		{75.0, 98.0},
		{{"key1", true}, {"key2", false}},
		std::nullopt,
		std::make_unique<int>(12)
	};
	auto serialized = scts::serialize<complete_object, scts::binary_formatter>(a);
	auto b = scts::deserialize<complete_object, scts::binary_formatter>(serialized.get_in_stream());
	REQUIRE(a == b);
}