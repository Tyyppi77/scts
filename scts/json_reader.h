#pragma once

#include "stream.h"
#include "lexical_cast.h"
#include "builtin_types.h"

#include <cassert>

namespace scts {
	struct json_reader {
		static constexpr bool requires_names = true;

		static void prepare_read(scts::in_stream& stream) {
			// Strips out containing curly braces and removes all extra whitespace.
			bool is_inside_string = false;
			for (std::size_t i = 0; i < stream.length(); ++i) {
				const auto character = stream.at(i);
				if (character == '"') {
					is_inside_string = !is_inside_string;
				}
				else if (!is_inside_string && (character == ' ' || character == '\n' || character == '\t')) {
					stream.erase(i, 1);
					i--;  // Decrement to counter the increment.
				}
			}

			const auto starts_with_brace = stream.front() == '{';
			const auto ends_with_brace = stream.back() == '}';
			assert(starts_with_brace && ends_with_brace);
			stream = stream.substr(1, stream.length() - 2);
		}

		template <typename T>
		static void read_member(T& member, scts::in_stream& stream, const std::string_view& name) {
			const auto content = read_next_section_and_consume_stream_with_name(stream, name);
			if (content.has_value()) {
				read_value(member, content.value());
			}
		}
	private:
		static std::string remove_quotes_from_string(const scts::in_stream& stream) {
			const auto has_quotes = stream.front() == '"' && stream.back() == '"';
			assert(has_quotes);
			return stream.substr(1, stream.length() - 2);
		}

		static std::optional<scts::in_stream> read_next_section_and_consume_stream_with_name(scts::in_stream& stream, const std::string_view& name) {
			const auto search_for = '"' + std::string(name) + '"' + ":";
			const auto name_start = stream.find(search_for);
			if (name_start == std::string::npos) {
				return std::nullopt;
			}
			const auto name_end = name_start + search_for.length();
			return read_next_section_and_consume_stream(stream, name_end, search_for.length());
		}

		static scts::in_stream read_next_section_and_consume_stream(scts::in_stream& stream, std::size_t start, std::size_t name_length) {
			const std::size_t comma_position = [&]() {
				std::size_t open_braces = 0;
				bool is_inside_string = false;
				for (std::size_t i = start; i < stream.length(); ++i) {
					const auto current = stream.at(i);
					if (current == ',' && open_braces == 0) {
						return i;
					}
					else if (current == '"') {
						is_inside_string = !is_inside_string;
					}
					else if (!is_inside_string && (current == '{' || current == '[')) {
						open_braces++;
					}
					else if (!is_inside_string && (current == '}' || current == ']')) {
						assert(open_braces > 0);
						open_braces--;
					}
				}
				return scts::in_stream::npos;
			}();

			if (comma_position == scts::in_stream::npos) {
				const auto copy = stream.substr(start);
				stream.erase(start - name_length, std::string::npos);
				return copy;
			}

			const auto section = stream.substr(start, comma_position - start);
			stream.erase(start - name_length, comma_position - start + name_length + 1);
			return section;
		}

		template <typename T, typename = void>
		struct builtin_type_reader {
			static void read(T& value, const scts::in_stream& stream) {
				value = convert_stream_to_value<T>(stream);
			}
		private:
			template <typename V, typename = void>
			static V convert_stream_to_value(const scts::in_stream& stream) {
				return lexical_cast<T>(stream);
			}
			template <>
			static bool convert_stream_to_value<bool>(const scts::in_stream& stream) {
				return stream == "true";
			}
			template <>
			static std::string convert_stream_to_value<std::string>(const scts::in_stream& stream) {
				return remove_quotes_from_string(stream);
			}
		};

		static void prepare_stream_for_list_processing(scts::in_stream& stream) {
			const bool has_array_braces = stream.front() == '[' && stream.back() == ']';
			assert(has_array_braces);
			stream = stream.substr(1, stream.length() - 2);
		}

		template <typename T>
		struct builtin_type_reader<std::vector<T>> {
			static void read(std::vector<T>& vector, const scts::in_stream& stream) {
				vector.clear();
				auto copy = stream;
				prepare_stream_for_list_processing(copy);
				while (!copy.empty()) {
					const auto section = read_next_section_and_consume_stream(copy, 0);
					T value{};
					read_value(value, section);
					vector.push_back(value);
				}
			}
		};

		template <typename T, std::size_t C>
		struct builtin_type_reader<std::array<T, C>> {
			static void read(std::array<T, C>& array, const scts::in_stream& stream) {
				auto copy = stream;
				prepare_stream_for_list_processing(copy);
				for (std::size_t i = 0; i < array.size(); ++i) {
					const auto section = read_next_section_and_consume_stream(copy, 0);
					read_value(array.at(i), section);
				}
			}
		};

		template <typename V>
		struct builtin_type_reader<std::map<std::string, V>> {
			static void read(std::map<std::string, V>& map, const scts::in_stream& stream) {
				map.clear();
				auto copy = stream;
				prepare_read(copy);
				while (!copy.empty()) {
					const auto name_end = copy.find(":");
					const auto name_source = copy.substr(0, name_end);
					copy.erase(0, name_end + 1);
					const auto name = remove_quotes_from_string(name_source);
					const auto section = read_next_section_and_consume_stream(copy, 0);
					V value{};
					read_value(value, section);
					map.insert(std::make_pair(name, value));
				}
			}
		};

		template <typename Enum>
		struct builtin_type_reader<Enum, typename std::enable_if_t<std::is_enum_v<Enum>>> {
			static void read(Enum& value, const scts::in_stream& stream) {
				value = static_cast<Enum>(std::stoi(stream));
			}
		};

		template <typename T>
		struct builtin_type_reader<T*> {
			static void read(T*& value, const scts::in_stream& stream) {
				if (stream == "null") {
					value = nullptr;
				}
				else {
					assert(value == nullptr);  // TODO: Decide how to handle memory allocation inside the serializer.
					value = new T();
					read_value(*value, stream);
				}
			}
		};

		template <typename T, std::size_t C>
		struct builtin_type_reader<T[C]> {
			static void read(T(&array)[C], const scts::in_stream& stream) {
				auto copy = stream;
				prepare_stream_for_list_processing(copy);
				for (std::size_t i = 0; i < C; ++i) {
					const auto section = read_next_section_and_consume_stream(copy, 0);
					read_value(array[i], section);
				}
			}
		};

		template <typename T>
		struct builtin_type_reader<std::optional<T>> {
			static void read(std::optional<T>& value, const scts::in_stream& stream) {
				if (stream == "null") {
					value = std::nullopt;
				}
				else {
					value = T{};
					read_value(value.value(), stream);
				}
			}
		};

		template <typename T>
		struct builtin_type_reader<std::unique_ptr<T>> {
			static void read(std::unique_ptr<T>& value, const scts::in_stream& stream) {
				if (stream == "null") {
					value = nullptr;
				}
				else {
					value = std::make_unique<T>();
					read_value(*value.get(), stream);  // Dereferences because the pointer pipeline currently manages memory.
				}
			}
		};

		template <typename T>
		static typename std::enable_if<is_builtin_type<T>::value, void>::type read_value(T& member, const scts::in_stream& stream) {
			return builtin_type_reader<T>::template read(member, stream);
		}

		template <typename T>
		static typename std::enable_if<!is_builtin_type<T>::value, void>::type read_value(T& member, const scts::in_stream& stream) {
			auto copy = stream;
			prepare_read(copy);
			scts::register_type<T>::descriptor.load<json_reader>(member, copy);
		}
	};
}