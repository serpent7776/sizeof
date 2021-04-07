#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <variant>

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

template <typename ...Ts>
struct for_each_t
{
	template <typename F>
	static void call()
	{
		(F{}(typeinfo_t<Ts> {}), ...);
	}
};


struct SizePrinter
{
	template <typename T>
	void operator()(typeinfo_t<T> i)
	{
		std::cout << sizeof(typename decltype(i)::type) << '\t' << i.name() << '\n';
	}
};

#define NAMED(...) named_t<strlen_t{#__VA_ARGS__}.length, string_buf_t{#__VA_ARGS__}, __VA_ARGS__>

int main()
{
	for_each_t<
		char,
		NAMED(short),
		NAMED(int),
		NAMED(long),
		NAMED(long long),
		NAMED(float),
		NAMED(double),
		NAMED(int16_t),
		NAMED(int32_t),
		NAMED(int64_t),
		NAMED(size_t),
		NAMED(ptrdiff_t),
		NAMED(void*),
		NAMED(long[10]),
		NAMED(std::string),
		NAMED(std::string_view),
		NAMED(std::vector<bool>),
		NAMED(std::vector<std::string>),
		NAMED(std::set<int>),
		NAMED(std::unordered_set<int>),
		NAMED(std::map<int, int>),
		NAMED(std::unordered_map<int, int>),
		NAMED(std::variant<std::string>),
		NAMED(SizePrinter)
	>::call<SizePrinter>();
	return 0;
}
