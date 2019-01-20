#pragma once

#include "stream.h"

#include <array>
#include <memory>
#include <type_traits>

namespace scts {
	// Represents a trivial type as an array of unsigned chars, that can be easily written to a stream.
	template <typename T>
	struct value_as_binary {
		using byte = unsigned char;

		static_assert(std::is_trivial_v<T>);

		static constexpr auto size = sizeof(T);

		value_as_binary(T val) noexcept { 
			const byte* begin = reinterpret_cast<const byte*>(std::addressof(val));
			const byte* end = begin + size;
			std::copy(begin, end, std::begin(m_bytes));
		}

		value_as_binary(scts::in_stream& stream) {
			for (auto& b : m_bytes) {
				b = stream.front();
				stream.erase(0, 1);
			}
		}

		constexpr T value() const {
			T value;
			byte* value_begin = reinterpret_cast<byte*>(std::addressof(value));
			std::copy(m_bytes.begin(), m_bytes.end(), value_begin);
			return value;
		}

		void write(scts::out_stream& stream) const {
			for (const auto b : m_bytes) { stream << b; }
		}
	private:
		std::array<byte, size> m_bytes;
	};
}