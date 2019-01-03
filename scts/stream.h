#pragma once

#include <sstream>

namespace scts {
	using in_stream = std::string;

	struct out_stream : std::stringstream {
		static constexpr std::streamsize max_precision = 100;

		out_stream() { precision(max_precision); }
		out_stream(const out_stream& stream) { str(stream.str()); }

		in_stream get_in_stream() const { return str(); }
	};
}