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

template <typename ...T>
struct typelist_t
{
	template <template <typename> typename F>
	using map = typelist_t<F<T>...>;

	template <template <typename> typename F>
	static constexpr void apply()
	{
		(F<T>{}(), ...);
	}
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

	static const char* name()
	{
		return typeid(T).name();
	}
};
template <size_t N, string_buf_t<N> s, typename T>
struct typeinfo_t<named_t<N, s, T>>
{
	using type = T;

	static const char* name()
	{
		return s;
	}
};

template <typename typelist>
struct for_each_t
{
	template <template <typename> typename F>
	static void call()
	{
		using typeinfo_list = typename typelist::template map<typeinfo_t>;
		typeinfo_list::template apply<F>();
	}
};


template <typename TypeInfo>
struct SizePrinter
{
	void operator()()
	{
		std::cout << sizeof(typename TypeInfo::type) << '\t' << TypeInfo::name() << '\n';
	}
};

#define NAMED(...) named_t<strlen_t{#__VA_ARGS__}.length, string_buf_t{#__VA_ARGS__}, __VA_ARGS__>

int main()
{
	using types = typelist_t<
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
		NAMED(SizePrinter<typeinfo_t<char>>)
	>;
	for_each_t<types>::call<SizePrinter>();
	return 0;
}
