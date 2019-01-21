#include "catch.hpp"

#include "test_objects.h"

TEST_CASE("basic json serialization and deserialization", "[json_formatter]") {
	base_object a{ 0.35, 12 };
	auto a_stream = scts::serialize(a);
	base_object b;
	scts::deserialize(b, a_stream.get_in_stream());
	auto b_stream = scts::serialize(b);

	REQUIRE(a == b);
	REQUIRE(a_stream.str() == b_stream.str());
}

TEST_CASE("inheritance", "[json_formatter]") {
	derived_object a{ -124.1, 76, 0.15f, "hello" };
	auto a_stream = scts::serialize(a);
	derived_object b;
	scts::deserialize(b, a_stream.get_in_stream());
	auto b_stream = scts::serialize(b);

	REQUIRE(a == b);
	REQUIRE(a_stream.str() == b_stream.str());
}

TEST_CASE("basic human written json", "[json_formatter]") {
	const auto in_stream = R"(
	{
		"integer": 124,
		"data": 0.12,
		"floating": 3.14
	})";
	derived_object a;
	a.string = "derived";
	scts::deserialize(a, in_stream);
	derived_object expected{ 0.12, 124, 3.14f, "derived" };

	REQUIRE(a == expected);
}

TEST_CASE("pretty formatting", "[json_formatter]") {
	const auto expected = R"({
	"data": 10.5,
	"integer": -15
})";
	base_object a{ 10.5, -15 };
	scts::json_formatter formatter{ scts::json_writer::pretty_with_tabs };
	auto stream = scts::serialize(a, formatter);
	REQUIRE(stream.str() == expected);
}

TEST_CASE("json_formatter supports all required types", "[json_formatter]") {
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
	auto serialized = scts::serialize(a);
	auto b = scts::deserialize<complete_object>(serialized.get_in_stream());
	REQUIRE(a == b);
}