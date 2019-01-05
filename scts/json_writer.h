#pragma once

#include "stream.h"
#include "builtin_types.h"

#include <string>

namespace scts {
	struct json_writer {
		struct formatting {
			const bool pretty = false;
			const std::string_view indentation;
		};

		static constexpr formatting compact = formatting{};
		static constexpr formatting pretty_with_tabs = formatting{ true, "\t" };
		static constexpr formatting pretty_with_4spaces = formatting{ true, "    " };

		static constexpr bool requires_names = true;

		constexpr json_writer(const formatting& f) : m_formatting(f) { }

		// Wrapping curly braces.
		void prepare_write(scts::out_stream& stream) { 
			start_subobject(stream);
		}
		void post_write(scts::out_stream& stream) { 
			end_subobject(stream);
		}

		template <typename T>
		scts::out_stream& write_member(const T& value, scts::out_stream& stream, const std::string_view& name, bool is_last) {
			static_assert(scts::is_serializable_v<T>);

			write_indentation(stream);
			write_wrapped_in_quotes(name, stream) << ":";
			if (m_formatting.pretty) stream << " ";
			return write_value(value, stream, is_last);
		}

		static void write_inherited_object_separator(scts::out_stream& stream) {
			stream << ",";
		}
	private:
		template <typename T>
		typename std::enable_if<is_builtin_type<T>::value, scts::out_stream&>::type write_value(const T& value, scts::out_stream& stream, bool is_last) {
			return builtin_type_writer<T>::write(*this, value, stream, is_last);
		}

		template <typename T>
		typename std::enable_if<!is_builtin_type<T>::value, scts::out_stream&>::type write_value(const T& value, scts::out_stream& stream, bool is_last) {
			start_subobject(stream);
			scts::register_type<T>::descriptor.save(*this, value, stream);
			end_subobject(stream);
			return write_separator_if_required(stream, is_last);
		}

		template <typename StringLike>
		static scts::out_stream& write_wrapped_in_quotes(const StringLike& value, scts::out_stream& stream) {
			stream << '"' << value << '"';
			return stream;
		}

		scts::out_stream& write_separator_if_required(scts::out_stream& stream, bool is_last) {
			if (!is_last) {
				stream << ",";
				write_newline(stream);;
			}
			return stream;
		}

		void write_indentation(scts::out_stream& stream) {
			if (!m_formatting.pretty) return;
			for (std::size_t i = 0; i < m_indentation_level; ++i) {
				stream << m_formatting.indentation;
			}
		}

		void write_newline(scts::out_stream& stream) {
			if (!m_formatting.pretty) return;
			stream << "\n";
		}

		void start_subobject(scts::out_stream& stream) {
			stream << "{";
			m_indentation_level++;
			write_newline(stream);
		}

		void end_subobject(scts::out_stream& stream) {
			write_newline(stream);
			stream << "}";
			m_indentation_level--;
		}

		template <typename T>
		struct builtin_list_writer {
			static scts::out_stream& write(json_writer& writer, const T& values, scts::out_stream& stream, bool is_last) {
				return write(writer, values.begin(), values.end(), stream, is_last);
			}
		protected:
			template <typename Iterator>
			static scts::out_stream& write(json_writer& writer, const Iterator& begin, const Iterator& end, scts::out_stream& stream, bool is_last) {
				stream << '[';
				for (auto current = begin; current != end; ++current) {
					writer.write_value(*current, stream, std::next(current) == end);
				}
				stream << ']';
				return writer.write_separator_if_required(stream, is_last);
			}
		};

		// Strings and arithmetic types.
		template <typename T, typename = void>
		struct builtin_type_writer {
			static scts::out_stream& write(json_writer& writer, const T& value, scts::out_stream& stream, bool is_last) {
				write(value, stream);
				return writer.write_separator_if_required(stream, is_last);
			}
		private:
			template <typename T> static void write(const T& value, scts::out_stream& stream) { stream << value; }
			template <> static void write<int8_t>(const int8_t& value, scts::out_stream& stream) { stream << static_cast<int>(value); }
			template <> static void write<uint8_t>(const uint8_t& value, scts::out_stream& stream) { stream << static_cast<int>(value); }
			template <> static void write<bool>(const bool& value, scts::out_stream& stream) {
				if (value) stream << "true";
				else stream << "false";
			}
			template <> static void write<std::string>(const std::string& value, scts::out_stream& stream) {
				write_wrapped_in_quotes(value, stream);
			}
		};

		// Enums.
		template <typename Enum>
		struct builtin_type_writer<Enum, std::enable_if_t<std::is_enum_v<Enum>>> {
			static scts::out_stream& write(json_writer& writer, const Enum& value, scts::out_stream& stream, bool is_last) {
				stream << static_cast<std::underlying_type_t<Enum>>(value);
				return writer.write_separator_if_required(stream, is_last);
			}
		};

		// C-style pointers and arrays.
		template <typename T>
		struct builtin_type_writer<T*> {
			static scts::out_stream& write(json_writer& writer, const T* value, scts::out_stream& stream, bool is_last) {
				if (value == nullptr) {
					stream << "null";
					return writer.write_separator_if_required(stream, is_last);
				}
				else {
					// Decays to the type that the pointer object is constructed from.
					return writer.write_value<T>(*value, stream, is_last);
				}
			}
		};

		template <typename T, std::size_t C>
		struct builtin_type_writer<T[C]> : builtin_list_writer<T> {
			static scts::out_stream& write(json_writer& writer, const T(&values)[C], scts::out_stream& stream, bool is_last) {
				return builtin_list_writer<T>::write(writer, std::begin(values), std::end(values), stream, is_last);
			}
		};

		// Standard library containers and classes.
		template <typename T>
		struct builtin_type_writer<std::vector<T>> : builtin_list_writer<std::vector<T>> { };

		template <typename T, std::size_t C>
		struct builtin_type_writer<std::array<T, C>> : builtin_list_writer<std::array<T, C>> { };


		template <typename V>
		struct builtin_type_writer<std::map<std::string, V>> {
			static scts::out_stream& write(json_writer& writer, const std::map<std::string, V>& values, scts::out_stream& stream, bool is_last) {
				stream << "{";
				for (auto it = values.begin(); it != values.end(); ++it) {
					write_wrapped_in_quotes((*it).first, stream);
					stream << ":";
					writer.write_value<V>((*it).second, stream, std::next(it) == values.end());
				}
				stream << "}";
				return writer.write_separator_if_required(stream, is_last);
			}
		};

		template <typename T>
		struct builtin_type_writer<std::optional<T>> {
			static scts::out_stream& write(json_writer& writer, const std::optional<T>& value, scts::out_stream& stream, bool is_last) {
				if (value.has_value()) {
					writer.write_value<T>(value.value(), stream, true);
				}
				else {
					stream << "null";
				}
				writer.write_separator_if_required(stream, is_last);
				return stream;
			}
		};

		// Standard library smart pointers.
		template <typename T>
		struct builtin_type_writer<std::unique_ptr<T>> {
			static scts::out_stream& write(json_writer& writer, const std::unique_ptr<T>& value, scts::out_stream& stream, bool is_last) {
				return builtin_type_writer<T*>::write(writer, value.get(), stream, is_last);
			}
		};

		const formatting m_formatting;
		std::size_t m_indentation_level = 0;
	};
}