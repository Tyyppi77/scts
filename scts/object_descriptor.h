#pragma once

#include <type_traits>

#include "io.h"
#include "helpers.h"
#include "formatters.h"
#include "register_type.h"

namespace scts {
	template <typename... Parents>
	struct inherits_from {
		template <typename Formatter, typename O>
		static void write(Formatter& formatter, const O& object, scts::out_stream& stream) {
			write_detail::template write<Formatter, O, Parents...>(formatter, object, stream);
		}

		template <typename Formatter, typename O>
		static void read(Formatter& formatter, O& object, scts::in_stream& stream) {
			read_detail::template read<Formatter, O, Parents...>(formatter, object, stream);
		}
	private:
		struct write_detail {
			template <typename Formatter, typename O>
			static void write(Formatter&, const O&, scts::out_stream&) { }

			template <typename Formatter, typename O, typename Parent>
			static void write(Formatter& formatter, const O& object, scts::out_stream& stream) {
				write_parent<Formatter, O, Parent>(formatter, object, stream);
			}

			template <typename Formatter, typename O, typename Parent, typename Second, typename... Rest>
			static void write(Formatter& formatter, const O& object, scts::out_stream& stream) {
				write_parent<Formatter, O, Parent>(formatter, object, stream);
				write<Formatter, O, Second, Rest...>(formatter, object, stream);
			}

			template <typename Formatter, typename O, typename Parent>
			static void write_parent(Formatter& formatter, const O& object, scts::out_stream& stream) {
				scts::register_type<Parent>::descriptor.save(formatter, object, stream);
				formatter.write_inherited_object_separator(stream);
			}
		};

		struct read_detail {
			template <typename Formatter, typename O>
			static void read(Formatter&, O&, scts::in_stream&) { }

			template <typename Formatter, typename O, typename Parent>
			static void read(Formatter& formatter, O& object, scts::in_stream& stream) {
				read_parent<Formatter, O, Parent>(formatter, object, stream);
			}

			template <typename Formatter, typename O, typename Parent, typename Second, typename... Rest>
			static void read(Formatter& formatter, O& object, scts::in_stream& stream) {
				read_parent<Formatter, O, Parent>(formatter, object, stream); 
				read<Formatter, O, Second, Rest...>(formatter, object, stream);
			}

			template <typename Formatter, typename O, typename Parent>
			static void read_parent(Formatter& formatter, O& object, scts::in_stream& stream) {
				scts::register_type<Parent>::descriptor.load(formatter, object, stream);
			}
		};
	};

	template <auto Ptr>
	struct member {
		using value_type = typename scts::deduce_member_ptr_type<decltype(Ptr)>::type;

		static_assert(scts::is_builtin_type_v<value_type> || scts::is_registered_type_v<value_type>,
			"member needs to be a basic value or a registered type!");

		// Currently the initialization in the serializer requires that all objects are default constructible.
		static_assert(std::is_default_constructible_v<value_type>, "member needs to be default constructible!");

		template <typename O>
		static constexpr value_type& get(O& object) noexcept { return object.*Ptr; }
		template <typename O>
		static constexpr const value_type& get(const O& object) noexcept { return object.*Ptr; }
	};

	template <typename... Members>
	struct members { 
		static constexpr auto member_count = sizeof...(Members);
		using name_container = std::array<std::string_view, member_count>;

		template <typename Formatter, typename O>
		static scts::out_stream& save(Formatter& formatter, const O& object, scts::out_stream& stream, const name_container& names) {
			if constexpr (formatter.requires_names) {
				return writer<O, name_container>::template write<Formatter, Members...>(formatter, object, stream, names, 0);
			}
			else {
				names;
				return writer_no_names<O>::template write<Formatter, Members...>(formatter, object, stream);
			}
		}

		template <typename Formatter, typename O>
		static O& load(Formatter& formatter, O& object, scts::in_stream& stream, const name_container& names) {
			if constexpr (formatter.requires_names) {
				return reader<O, name_container>::template read<Formatter, Members...>(formatter, object, stream, names, 0);
			}
			else {
				names;
				return reader_no_names<O>::template read<Formatter, Members...>(formatter, object, stream);
			}
		}
	};

	template <typename O, typename Members, typename InheritsFrom = inherits_from<>>
	struct object_descriptor {
		// It's possible to construct an object descriptor without any names.
		// That will restrict the serialization to formatters that only serialize without names, though.
		constexpr object_descriptor() noexcept : has_names(false) { }

		template <typename... Names>
		constexpr object_descriptor(Names... names) noexcept 
			: has_names(true), m_names({ names... }) {
			static_assert(sizeof...(Names) == Members::member_count, "object_descriptor needs the correct amount of names");
		}

		template <typename Formatter>
		scts::out_stream& save(Formatter& formatter, const O& object, scts::out_stream& stream) const {
			InheritsFrom::write(formatter, object, stream);
			return Members::save(formatter, object, stream, m_names);
		}

		template <typename Formatter>
		O& load(Formatter& formatter, O& object, scts::in_stream& stream) const {
			InheritsFrom::read(formatter, object, stream);
			return Members::load(formatter, object, stream, m_names);
		}

		const bool has_names;
	private:
		const typename Members::name_container m_names;
	};
}