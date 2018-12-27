#pragma once

#include "stream.h"
#include "basic_value.h"

#include <cassert>

namespace scts {
	struct json_reader {
		static void prepare_read(scts::in_stream& stream) {
			const auto starts_with_brace = stream.front() == '{';
			const auto ends_with_brace = stream.back() == '}';
			assert(starts_with_brace && ends_with_brace);
			stream = stream.substr(1, stream.length() - 2);
		}

		template <typename T>
		static void read_member(T& member, scts::in_stream& stream) {
			const scts::in_stream content = read_next_section_and_consume_stream_with_name(stream);
			read_value(member, content);
		}
	private:
		static std::string remove_quotes_from_string(const scts::in_stream& stream) {
			const auto has_quotes = stream.front() == '"' && stream.back() == '"';
			assert(has_quotes);
			return stream.substr(1, stream.length() - 2);
		}

		static scts::in_stream read_next_section_and_consume_stream_with_name(scts::in_stream& stream) {
			const auto name_end = stream.find(":") + 1;
			return read_next_section_and_consume_stream(stream, name_end);
		}

		static scts::in_stream read_next_section_and_consume_stream(scts::in_stream& stream, std::size_t start) {
			const std::size_t comma_position = [&]() {
				std::size_t open_braces = 0;
				for (std::size_t i = start; i < stream.length(); ++i) {
					const auto current = stream.at(i);
					if (current == ',' && open_braces == 0) {
						return i;
					}
					else if (current == '{' || current == '[') {
						open_braces++;
					}
					else if (current == '}' || current == ']') {
						assert(open_braces > 0);
						open_braces--;
					}
				}
				return scts::in_stream::npos;
			}();

			if (comma_position == scts::in_stream::npos) {
				const auto copy = stream.substr(start);
				stream.clear();
				return copy;
			}

			const auto section = stream.substr(start, comma_position - start);
			stream.erase(0, comma_position + 1);
			return section;
		}

		template <typename T>
		struct basic_value_reader {
			static void read(T& value, const scts::in_stream& stream) {
				value = convert_stream_to_value<T>(stream);
			}
		private:
			template <typename V> 
			static V convert_stream_to_value(const scts::in_stream& stream) {
				static_assert(false);
			}
			template <> 
			static int convert_stream_to_value<int>(const scts::in_stream& stream) {
				return std::stoi(stream);
			}
			template <> 
			static bool convert_stream_to_value<bool>(const scts::in_stream& stream) {
				return stream == "true";
			}
			template <> 
			static float convert_stream_to_value<float>(const scts::in_stream& stream) {
				return std::stof(stream);
			}
			template <> 
			static double convert_stream_to_value<double>(const scts::in_stream& stream) {
				return std::stod(stream);
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
		struct basic_value_reader<std::vector<T>> {
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
		struct basic_value_reader<std::array<T, C>> {
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
		struct basic_value_reader<std::map<std::string, V>> {
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

		template <typename T>
		static typename std::enable_if<is_basic_value<T>::value, void>::type read_value(T& member, const scts::in_stream& stream) {
			return basic_value_reader<T>::template read(member, stream);
		}

		template <typename T>
		static typename std::enable_if<!is_basic_value<T>::value, void>::type read_value(T& member, const scts::in_stream& stream) {
			auto copy = stream;
			prepare_read(copy);
			scts::register_type<T>::descriptor.load<json_reader>(member, copy);
		}
	};

	struct json_writer {
		static void pre_write(scts::out_stream& stream) { stream << "{"; }
		static void post_write(scts::out_stream& stream) { stream << "}"; }

		template <typename T>
		static scts::out_stream& write_member(const T& value, scts::out_stream& stream, const std::string_view& name, bool is_last) {
			write_wrapped_in_quotes(name, stream) << ":";
			return write_value(value, stream, is_last);
		}
	private:
		template <typename StringLike>
		static scts::out_stream& write_wrapped_in_quotes(const StringLike& value, scts::out_stream& stream) {
			stream << '"' << value << '"'; 
			return stream;
		}
		static scts::out_stream& write_separator_if_required(scts::out_stream& stream, bool is_last) {
			if (!is_last) stream << ",";
			return stream;
		}

		template <typename T>
		struct basic_value_writer {
			static scts::out_stream& write(const T& value, scts::out_stream& stream, bool is_last) {
				write_really_basic_value(value, stream);
				write_separator_if_required(stream, is_last);
				return stream;
			}
		private:
			// We don't need to specialize the whole struct for really basic types, like strings and numbers.
			template <typename T> 
			static void write_really_basic_value(const T& value, scts::out_stream& stream) {
				stream << value;
			}
			template <>
			static void write_really_basic_value<bool>(const bool& value, scts::out_stream& stream) {
				stream << std::boolalpha << value << std::noboolalpha;
			}
			template <>
			static void write_really_basic_value<std::string>(const std::string& value, scts::out_stream& stream) {
				write_wrapped_in_quotes(value, stream);
			}
		};

		template <typename T>
		struct basic_list_writer {
			static scts::out_stream& write(const T& values, scts::out_stream& stream, bool is_last) {
				return write_list(values.begin(), values.end(), stream, is_last);
			}
		private:
			template <typename Iterator> 
			static scts::out_stream& write_list(const Iterator& begin, const Iterator& end, scts::out_stream& stream, bool is_last) {
				stream << '[';
				for (auto current = begin; current != end; ++current) {
					write_value(*current, stream, std::next(current) == end);
				}
				stream << ']';
				write_separator_if_required(stream, is_last);
				return stream;
			}
		};

		template <typename T>
		struct basic_value_writer<std::vector<T>> : basic_list_writer<std::vector<T>> { };

		template <typename T, std::size_t C>
		struct basic_value_writer<std::array<T, C>> : basic_list_writer<std::array<T, C>> { };

		template <typename V>
		struct basic_value_writer<std::map<std::string, V>> {
			static scts::out_stream& write(const std::map<std::string, V>& values, scts::out_stream& stream, bool is_last) {
				stream << "{";
				for (auto it = values.begin(); it != values.end(); ++it) {
					write_wrapped_in_quotes((*it).first, stream);
					stream << ":";
					write_value<V>((*it).second, stream, std::next(it) == values.end());
				}
				stream << "}";
				write_separator_if_required(stream, is_last);
				return stream;
			}
		};

		template <typename T>
		static typename std::enable_if<is_basic_value<T>::value, scts::out_stream&>::type write_value(const T& value, scts::out_stream& stream, bool is_last) {
			return basic_value_writer<T>::write(value, stream, is_last);
		}

		template <typename T>
		static typename std::enable_if<!is_basic_value<T>::value, scts::out_stream&>::type write_value(const T& value, scts::out_stream& stream, bool is_last) {
			stream << "{";
			scts::register_type<T>::descriptor.save<json_writer>(value, stream);
			stream << "}";
			write_separator_if_required(stream, is_last);
			return stream;
		}
	};

	struct json_formatter : json_writer, json_reader { };
}