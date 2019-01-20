#include "catch.hpp"

#include "../scts/value_as_binary.h"

template <typename T>
T write_and_read(T value) {
	const auto as_binary = scts::value_as_binary(value);
	scts::out_stream stream;
	as_binary.write(stream);
	auto in_stream = stream.get_in_stream();
	return scts::value_as_binary<T>(in_stream).value();
}

TEST_CASE("value as binary conversion", "[value_as_binary]") {
	REQUIRE(write_and_read(12.5f) == 12.5f);
	REQUIRE(write_and_read(-54) == -54);
}