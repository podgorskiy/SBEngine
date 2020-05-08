#pragma once
#include <stdarg.h>
#include <memory>
#include <string>
#include <exception>
#include <string.h>


inline std::string string_format(const std::string fmt_str, va_list ap)
{
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	while(1) {
		va_list ap_copy;
		va_copy(ap_copy, ap);
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], fmt_str.c_str());
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap_copy);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}

inline std::string string_format(const std::string fmt_str, ...)
{
	va_list ap;
	va_start(ap, fmt_str);
	std::string result = string_format(fmt_str, ap);
	va_end(ap);
	return result;
}

struct vaholder
{
protected:
	va_list ap;
};

class runtime_error: public vaholder, public std::runtime_error
{
public:
	explicit runtime_error(std::string fmt_str, ...): vaholder(),
			std::runtime_error(string_format(fmt_str, (va_start(ap, fmt_str), ap)))
	{
		va_end(ap);
	}

	runtime_error(const runtime_error &) = default;
	runtime_error(runtime_error &&) = default;
	~runtime_error() override = default;
};

