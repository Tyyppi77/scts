#pragma once

namespace scts {
	template <typename O, typename Names>
	struct writer {
		template <typename Formatter, typename Member>
		static scts::out_stream& write(Formatter& formatter, const O& object, scts::out_stream& stream, const Names& names, std::size_t name_index) {
			formatter.write_member(Member::get(object), stream, names.at(name_index), true);
			return stream;
		}

		template <typename Formatter, typename Member, typename Second, typename... Rest>
		static scts::out_stream& write(Formatter& formatter, const O& object, scts::out_stream& stream, const Names& names, std::size_t name_index) {
			formatter.write_member(Member::get(object), stream, names.at(name_index), false);
			return write<Formatter, Second, Rest...>(formatter, object, stream, names, name_index + 1);
		}
	};

	template <typename O>
	struct writer_no_names {
		template <typename Formatter, typename Member>
		static scts::out_stream& write(Formatter& formatter, const O& object, scts::out_stream& stream) {
			formatter.write_member(object, stream, true);
			return stream;
		}

		template <typename Formatter, typename Member, typename Second, typename... Rest>
		static scts::out_stream& write(Formatter& formatter, const O& object, scts::out_stream& stream) {
			formatter.write_member(object, stream, false);
			return write<Formatter, Second, Rest...>(formatter, object, stream);
		}
	};

	template <typename O, typename Names>
	struct reader {
		template <typename Formatter, typename Member>
		static O& read(Formatter& formatter, O& object, scts::in_stream& stream, const Names& names, std::size_t name_index) {
			formatter.read_member(Member::get(object), stream, names.at(name_index));
			return object;
		}

		template <typename Formatter, typename Member, typename Second, typename... Rest>
		static O& read(Formatter& formatter, O& object, scts::in_stream& stream, const Names& names, std::size_t name_index) {
			formatter.read_member(Member::get(object), stream, names.at(name_index));
			return read<Formatter, Second, Rest...>(formatter, object, stream, names, name_index + 1);
		}
	};

	template <typename O>
	struct reader_no_names {
		template <typename Formatter, typename Member>
		static O& read(Formatter& formatter, O& object, scts::in_stream& stream) {
			formatter.read_member(Member::get(object), stream);
			return object;
		}

		template <typename Formatter, typename Member, typename Second, typename... Rest>
		static O& read(Formatter& formatter, O& object, scts::in_stream& stream) {
			formatter.read_member(Member::get(object), stream);
			return read<Formatter, Second, Rest...>(formatter, object, stream);
		}
	};
}