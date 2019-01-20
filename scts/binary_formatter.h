#pragma once

#include "stream.h"

namespace scts {
	struct binary_formatter {
		static constexpr bool requires_names = false;

		// We do not need to pre or post handle writing or reading.
		static void prepare_read(scts::in_stream&) { }
		static void prepare_write(scts::out_stream&) { }
		static void post_write(scts::out_stream&) { }
		// The formatter does not use separators.
		static void write_inherited_object_separator(scts::out_stream&) { }

		template <typename T>
		static void read_member(T& member, scts::in_stream& stream) { }

		template <typename T>
		static scts::out_stream& write_member(const T& member, scts::out_stream& stream, bool is_last) {
			return stream;
		}
	};
}