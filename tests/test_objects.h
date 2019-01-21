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

enum class state {
	idle, moving
};

struct complete_object {
	std::string string;
	bool boolean;
	uint8_t byte;
	state enumeration;
	base_object* pointer;
	float c_array[2];
	std::vector<base_object> vector_of_objects;
	std::array<double, 2> array_of_doubles;
	std::map<std::string, bool> map_of_booleans;
	std::optional<state> optional_of_enum;
	std::unique_ptr<int> smart_ptr;

	bool operator==(const complete_object& other) const {
		return string == other.string &&
			boolean == other.boolean &&
			byte == other.byte &&
			enumeration == other.enumeration &&
			pointer == other.pointer &&
			c_array[0] == other.c_array[0] &&
			c_array[1] == other.c_array[1] &&
			vector_of_objects == other.vector_of_objects &&
			map_of_booleans == other.map_of_booleans &&
			optional_of_enum == other.optional_of_enum &&
			*smart_ptr == *other.smart_ptr;
	}
};

template <> struct scts::register_type<complete_object> : scts::allow_serialization {
	static constexpr scts::object_descriptor<complete_object,
		scts::members<
		scts::member<&complete_object::string>,
		scts::member<&complete_object::boolean>,
		scts::member<&complete_object::byte>,
		scts::member<&complete_object::enumeration>,
		scts::member<&complete_object::pointer>,
		scts::member<&complete_object::c_array>,
		scts::member<&complete_object::vector_of_objects>,
		scts::member<&complete_object::array_of_doubles>,
		scts::member<&complete_object::map_of_booleans>,
		scts::member<&complete_object::optional_of_enum>,
		scts::member<&complete_object::smart_ptr>>> descriptor{ "string", "boolean", "byte", "enumeration", "pointer", "c_array", "vector_of_objects", "array_of_doubles", "map_of_booleans", "optional_of_enum", "smart_ptr" };
};