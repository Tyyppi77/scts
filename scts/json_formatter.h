#pragma once

#include "json_writer.h"
#include "json_reader.h"

namespace scts {
	struct json_formatter : json_writer, json_reader { 
		static constexpr bool requires_names = true;
	};
}