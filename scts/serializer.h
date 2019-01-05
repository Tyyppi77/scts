#pragma once

#include <array>
#include <string_view>
#include <initializer_list>

#include "stream.h"
#include "formatters.h"
#include "register_type.h"

namespace scts {
	template <typename O, typename Formatter = scts::json_formatter>
	inline scts::out_stream& serialize(const O& object, scts::out_stream& stream, Formatter formatter = Formatter()) {
		static_assert(scts::is_registered_type_v<O>, "cannot serialize an object type that is not registerd");
		static_assert(scts::is_valid_formatter_v<Formatter>, "formatter needs to be a valid formatter");

		formatter.prepare_write(stream);
		scts::register_type<O>::descriptor.save(formatter, object, stream);
		formatter.post_write(stream);
		return stream;
	}

	template <typename O, typename Formatter = scts::json_formatter>
	inline scts::out_stream serialize(const O& object, Formatter formatter = Formatter()) {
		scts::out_stream stream;
		serialize(object, stream, formatter);
		return stream;
	}

	template <typename O, typename Formatter = scts::json_formatter>
	inline O& deserialize(O& object, const scts::in_stream& stream, Formatter formatter = Formatter()) {
		static_assert(scts::is_registered_type_v<O>, "cannot deserialize an object type that is not registerd");
		static_assert(scts::is_valid_formatter_v<Formatter>, "formatter needs to be a valid formatter");

		auto copy = stream;
		formatter.prepare_read(copy);
		return scts::register_type<O>::descriptor.load(formatter, object, copy);
	}

	template <typename O, typename Formatter = scts::json_formatter>
	inline O deserialize(const scts::in_stream& stream, Formatter formatter = Formatter()) {
		O object;
		deserialize(object, stream, formatter);
		return object;
	}
}