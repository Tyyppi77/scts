#pragma once

#include <type_traits>

#include "stream.h"
#include "builtin_types.h"
#include "value_as_binary.h"

namespace scts {
	struct binary_writer {
		static constexpr bool requires_names = false;

		template <typename T>
		static scts::out_stream& write_member(const T& member, scts::out_stream& stream, bool) {
			return binary_writer::write_value(member, stream);
		}
	private:
		template <typename T>
		static typename std::enable_if<is_builtin_type<T>::value, scts::out_stream&>::type write_value(const T& value, scts::out_stream& stream) {
			return builtin_type_writer<T>::write(value, stream);
		}

		template <typename T>
		static typename std::enable_if<!is_builtin_type<T>::value, scts::out_stream&>::type write_value(const T& value, scts::out_stream& stream) {
			binary_writer writer;
			return scts::register_type<T>::descriptor.save(writer, value, stream);
		}

		template <typename T, typename = void>
		struct builtin_type_writer {
			static scts::out_stream& write(const T& value, scts::out_stream& stream) {
				scts::value_as_binary(value).write(stream);
				return stream;
			}
		};

		template <>
		struct builtin_type_writer<std::string> {
			static scts::out_stream& write(const std::string& value, scts::out_stream& stream) {
				scts::value_as_binary(value.length()).write(stream);
				stream << value;
			}
		};

		template <typename T>
		struct builtin_type_writer<T*> {
			static scts::out_stream& write(const T* value, scts::out_stream& stream) {
				const bool exists = value != nullptr;
				stream << exists;
				if (exists) write_value(*value, stream);
			}
		};
	};
}