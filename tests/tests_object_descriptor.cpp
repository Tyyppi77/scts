#include "catch.hpp"

#include "../scts/scts.h"

struct simple_test_object {
	int data;
};

template <> struct scts::register_type<simple_test_object> : scts::allow_serialization {
	static constexpr scts::object_descriptor<simple_test_object,
		scts::members<scts::member<&simple_test_object::data>>> descriptor{ "data" };
};

TEST_CASE("object descriptor basics", "[object_descriptor]") {
	STATIC_REQUIRE(scts::register_type<simple_test_object>::descriptor.has_names);
}