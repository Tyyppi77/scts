#pragma once

#include <sstream>
#include <cstdint>
#include <exception>
#include <type_traits>

namespace scts {
	struct invalid_lexical_cast : std::exception {
		invalid_lexical_cast(const std::string& string) : m_String("Invalid lexical_cast source: " + string) { }
		const char* what() const override { return m_String.c_str(); }
	private:
		const std::string m_String;
	};

	template <typename T>
	struct lexical_caster {
		template <typename StringLike>
		static T cast(const StringLike& source) {
			static constexpr auto default_locale = "C";

			std::stringstream stream;
			stream.imbue(std::locale(default_locale));
			stream << source;

			// Save in case we need to throw an exception.
			const auto converted_source = stream.str();

			T value;
			stream >> value;

			if (stream.fail() || stream.peek() != std::stringstream::traits_type::eof()) {
				throw invalid_lexical_cast(converted_source);
			}
			return value;
		}
	};

	// Annoyingly, we need to handle 8-bit types separately, as those are not treated as numbers by default.
	template <> struct lexical_caster<std::uint8_t> {
		template <typename StringLike>static std::uint8_t cast(const StringLike& source) {
			return static_cast<std::uint8_t>(lexical_caster<std::uint16_t>::cast(source));
		}
	};
	template <> struct lexical_caster<std::int8_t> {
		template <typename StringLike>static std::int8_t cast(const StringLike& source) {
			return static_cast<std::int8_t>(lexical_caster<std::int16_t>::cast(source));
		}
	};

	template <typename T, typename StringLike>
	inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type lexical_cast(const StringLike& source) {
		return lexical_caster<T>::cast(source);
	}
}