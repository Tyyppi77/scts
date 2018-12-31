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
		Formatter::pre_write(stream);
		scts::register_type<O>::descriptor.save<Formatter>(object, stream);
		Formatter::post_write(stream);
		return stream;
	}

	template <typename O, typename Formatter = scts::json_formatter>
	inline O& deserialize(O& object, scts::in_stream& stream) {
		static_assert(scts::is_registered_type_v<O>, "cannot deserialize an object type that is not registerd");
		Formatter::prepare_read(stream);
		return scts::register_type<O>::descriptor.load<Formatter>(object, stream);
	}

	template <typename O, typename Formatter = scts::json_formatter>
	inline O& deserialize_do_not_consume(O& object, const scts::in_stream& stream) {
		auto copy = stream;
		return deserialize<O, Formatter>(object, copy);
	}
}