#pragma once

#include <array>
#include <string_view>
#include <initializer_list>

#include "stream.h"
#include "formatters.h"
#include "register_type.h"

namespace scts {
	template <typename O, typename Formatter = scts::json_formatter>
	inline scts::out_stream& serialize(const O& object, scts::out_stream& stream) {
		static_assert(scts::is_registered_type_v<O>, "cannot serialize an object type that is not registerd");
		Formatter::prepare_write(stream);
		scts::register_type<O>::descriptor.save<Formatter>(object, stream);
		Formatter::post_write(stream);
		return stream;
	}

	template <typename O, typename Formatter = scts::json_formatter>
	inline scts::out_stream serialize(const O& object) {
		scts::out_stream stream;
		serialize(object, stream);
		return stream;
	}

	template <typename O, typename Formatter = scts::json_formatter>
	inline O& deserialize(O& object, const scts::in_stream& stream) {
		static_assert(scts::is_registered_type_v<O>, "cannot deserialize an object type that is not registerd");

		auto copy = stream;
		Formatter::prepare_read(copy);
		return scts::register_type<O>::descriptor.load<Formatter>(object, copy);
	}
}