#pragma once

#include "stream.h"
#include "builtin_types.h"
#include "value_as_binary.h"

namespace scts {
	struct binary_writer {
		template <typename T>
		typename std::enable_if<is_builtin_type<T>::value, scts::out_stream&>::type write_value(const T& value, scts::out_stream& stream) {
			return builtin_type_writer<T>::write(*this, value, stream);
		}

		template <typename T>
		typename std::enable_if<!is_builtin_type<T>::value, scts::out_stream&>::type write_value(const T& value, scts::out_stream& stream) {
			return scts::register_type<T>::descriptor.save(*this, value, stream);
		}
	private:
		template <typename T>
		struct builtin_type_writer {
			static scts::out_stream& write(const T& value, scts::out_stream& stream, bool) {
				scts::value_as_binary(value).write(stream);
				return stream;
			}
		};
	};
}