#pragma once

#include "stream.h"

#include <type_traits>

namespace scts {
	// Represents a trivial type as an array of unsigned chars, that can be easily written to a stream.
	template <typename T>
	struct value_as_binary {
		static_assert(std::is_trivial_v<T>);

		static constexpr auto size = sizeof(T);

		constexpr value_as_binary() noexcept { }
		constexpr value_as_binary(T val) noexcept : value(val) { }

		static void write(scts::out_stream& stream, T val) {
			value_as_binary<T> binary{ val };
			binary.write(stream);
		}

		static T read(scts::in_stream& stream) {
			value_as_binary<T> binary;
			binary.read_to(stream);
			return binary.value;
		}

		void write(scts::out_stream& stream) const {
			for (const auto byte : bytes) { stream << byte; }
		}

		void read_to(scts::in_stream& stream) {
			for (auto& byte : bytes) {
				byte = stream.front();
				stream.erase(0, 1);
			}
		}

		union {
			T value;
			unsigned char bytes[size];
		};
	};
}