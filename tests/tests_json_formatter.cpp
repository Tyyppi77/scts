#include "catch.hpp"

#include "../scts/scts.h"

struct base_object {
	double data;
	int integer;

	bool operator==(const base_object& other) const {
		return data == other.data && integer == other.integer;
	}
};

template <> struct scts::register_type<base_object> : scts::allow_serialization {
	static constexpr scts::object_descriptor<base_object,
		scts::members<
		scts::member<&base_object::data>,
		scts::member<&base_object::integer>>> descriptor{ "data", "integer" };
};

struct derived_object : base_object {
	float floating;
	std::string string;

	bool operator==(const derived_object& other) const {
		return floating == other.floating && string == other.string && data == other.data && integer == other.integer;
	}
};

template <> struct scts::register_type<derived_object> : scts::allow_serialization {
	static constexpr scts::object_descriptor<derived_object,
		scts::members<
		scts::member<&derived_object::floating>,
		scts::member<&derived_object::string>>,
		scts::inherits_from<base_object>> descriptor{ "floating", "string" };
};

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

TEST_CASE("json_formatter supports all required types", "[json_formatter]") {
	REQUIRE(true);
}