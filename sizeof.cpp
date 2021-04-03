#include <cstddef>
#include <cstdint>
#include <iostream>

template <typename T>
struct false_t
{
	static constexpr bool value = false;
};

template <typename ...T>
struct typelist_t
{
};

template <size_t N>
struct string_buf_t
{
	char buf[N];

	static constexpr size_t lenght = N;

	constexpr string_buf_t(const char* s)
	{
		for (size_t i = 0; i < lenght; ++i)
		{
			buf[i] = s[i];
		}
	}

	constexpr operator const char*() const
	{
		return buf;
	}
};
template<size_t N>
string_buf_t(const char (&)[N]) -> string_buf_t<N>;

template <size_t N, string_buf_t<N> s, typename T>
struct named_t
{
	static constexpr const char* name = (const char*)s;
	using type = T;
};

template <typename T>
struct typeinfo_t
{
	using type = T;

	const char* name()
	{
		return typeid(T).name();
	}
};
template <size_t N, string_buf_t<N> s, typename T>
struct typeinfo_t<named_t<N, s, T>>
{
	using type = T;

	const char* name()
	{
		return s;
	}
};

template <typename Types>
struct for_each_t
{
	static_assert(false_t<Types>::value);
};
template <size_t N, string_buf_t<N> s, typename T>
struct for_each_t<typelist_t<named_t<N, s, T>>>
{
	template <typename F>
	static void call()
	{
		F{}(typeinfo_t<named_t<N, s, T>> {});
	}
};
template <typename T>
struct for_each_t<typelist_t<T>>
{
	template <typename F>
	static void call()
	{
		F{}(typeinfo_t<T> {});
	}
};
template <size_t N, string_buf_t<N> s, typename T, typename ...Ts>
struct for_each_t<typelist_t<named_t<N, s, T>, Ts...>>
{
	template <typename F>
	static void call()
	{
		F{}(typeinfo_t<named_t<N, s, T>> {});
		for_each_t<typelist_t<Ts...>>::template call<F>();
	}
};
template <typename T, typename ...Ts>
struct for_each_t<typelist_t<T, Ts...>>
{
	template <typename F>
	static void call()
	{
		F{}(typeinfo_t<T> {});
		for_each_t<typelist_t<Ts...>>::template call<F>();
	}
};


struct Printer
{
	template <typename T>
	void operator()(typeinfo_t<T> i)
	{
		std::cout << i.name() << ' ' << sizeof(typename decltype(i)::type) << '\n';
	}
};

int main()
{
	for_each_t<
		typelist_t<
			int,
			named_t<4, string_buf_t{"int"}, int>,
			named_t<8, string_buf_t{"int16_t"}, int16_t>,
			named_t<8, string_buf_t{"int32_t"}, int32_t>,
			named_t<8, string_buf_t{"int64_t"}, int64_t>,
			named_t<7, string_buf_t{"size_t"}, size_t>,
			named_t<10, string_buf_t{"ptrdiff_t"}, ptrdiff_t>,
			named_t<6, string_buf_t{"void*"}, void*>
		>
	>::call<Printer>();
	return 0;
}
