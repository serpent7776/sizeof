#include <cstddef>
#include <cstdint>
#include <algorithm>
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

	static constexpr size_t length = N;

	constexpr string_buf_t(const char* s)
	{
		std::copy(s, s + length, buf);
	}

	constexpr operator const char*() const
	{
		return buf;
	}
};
template<size_t N>
string_buf_t(const char (&)[N]) -> string_buf_t<N>;

template <size_t N>
struct strlen_t
{
	static constexpr size_t length = N;

	constexpr strlen_t(const char (&)[N])
	{}
};
template<size_t N>
strlen_t(const char (&)[N]) -> strlen_t<N>;

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


struct SizePrinter
{
	template <typename T>
	void operator()(typeinfo_t<T> i)
	{
		std::cout << i.name() << ' ' << sizeof(typename decltype(i)::type) << '\n';
	}
};

#define NAMED(t) named_t<strlen_t{#t}.length, string_buf_t{#t}, t>

int main()
{
	for_each_t<
		typelist_t<
			char,
			NAMED(int),
			NAMED(int16_t),
			NAMED(int32_t),
			NAMED(int64_t),
			NAMED(size_t),
			NAMED(ptrdiff_t),
			NAMED(void*)
		>
	>::call<SizePrinter>();
	return 0;
}
