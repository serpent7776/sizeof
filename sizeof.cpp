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

template <typename Types>
struct for_each_t
{
	static_assert(false_t<Types>::value);
};
template <typename T>
struct for_each_t<typelist_t<T>>
{
	template <template<typename> typename F>
	static void call()
	{
		F<T>{}();
	}
};
template <typename T, typename ...Ts>
struct for_each_t<typelist_t<T, Ts...>>
{
	template <template<typename> typename F>
	static void call()
	{
		F<T>{}();
		for_each_t<typelist_t<Ts...>>::template call<F>();
	}
};


template <typename T>
struct Printer
{
	void operator()()
	{
		std::cout << typeid(T).name() << ' ' << sizeof(T) << '\n';
	}
};

int main()
{
	for_each_t<
		typelist_t<
			int,
			int8_t,
			int16_t,
			int32_t,
			int64_t,
			size_t,
			ptrdiff_t,
			// std::string,
			// std::string_view,
			void*
			// void // cannot size void
		>
	>::call<Printer>();
	return 0;
}
