#pragma once

#include "stream.h"

// A dummy formatter that doesn't actually do anything, but documents the formatter interface.
// Note that enforcing strong SFINAE here is not really necessary, since this is not a public facing API.
// Everything that is passed onto the formatters should already be validated by the object descriptors.
struct dummy_formatter {
	static constexpr bool requires_names = true;

	// Reading:
	// Called before any actual reading happens. Allows you to strip out any wrappers necessary.
	static void prepare_read(scts::in_stream&) { }
	// Deserializes a single member from an input stream containing everything that is left to deserialize.
	// The version taking in a name needs to only be available if requires_names is true.
	template <typename T>
	static void read_member(T&, scts::in_stream&) { }
	template <typename T>
	static void read_member(T&, scts::in_stream&, const std::string_view&) { }

	// Writing:
	// Called before and after writing. Allows you to wrap the serialized data into anything or post-process it.
	static void prepare_write(scts::out_stream&) { }
	static void post_write(scts::out_stream&) { }
	// Serializes a single member without a name. Needs to be only available if requires_names is false.
	template <typename T>
	static scts::out_stream& write_member(const T&, scts::out_stream& stream, bool) {
		return stream;
	}
	// Serializes a single member with the given name. Needs to be only available if requires_names is true.
	template <typename T>
	static scts::out_stream& write_member(const T&, scts::out_stream& stream, const std::string_view&, bool) {
		return stream;
	}
};

#include "json_formatter.h"
