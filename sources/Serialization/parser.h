#pragma once
#include "utils/math.h"
#include <ctype.h>


namespace serialization
{
	class Parser
	{
	public:
		enum TokenTypes: char
		{
			NONE = '\0',
			EQ = '=',
			PLUS = '+',
			MINUS = '-',
			MUL = '*',
			DIV = '/',
			LPAR = '(',
			RPAR = ')',
			LBRACES = '{',
			RBRACES = '}',
			LBRACKET = '[',
			RBRACKET = ']',
			MOD = '%',
			POW = '^',
			DOT = '.',
			COMA = ',',
			SEMICOLON = ';',
			STRING = '"',
			END_OF_STRING = '\0',
			IDENTIFIER = 'd',
			FLOAT = 'f',
			INT = 'i',
		};

		explicit Parser(const char* str): m_it(str)
		{}

		bool AcceptFloat(float& value);

		bool AcceptInt(int& value);

		bool AcceptChar(char c);

		bool AcceptCharIgnoreCase(char c);

		bool AcceptStr(const char* str);

		bool AcceptStrIgnoreCase(const char* str);

		bool AcceptDigit(int& d);

		bool AcceptWhiteSpace();

		bool AcceptNonWhiteSpace();

		template<typename T>
		bool Read(T& v);

		bool EOS() const { return *m_it == '\0'; }

	private:
		const char* m_it;
	};

	template<>
	inline bool Parser::Read(int& v)
	{
		return AcceptInt(v);
	}

	template<>
	inline bool Parser::Read(float& v)
	{
		return AcceptFloat(v);
	}

	template<>
	inline bool Parser::Read(bool& v)
	{
		while (AcceptWhiteSpace());
		return (v = AcceptStrIgnoreCase("true")) || AcceptStrIgnoreCase("false");
	}

	inline bool Parser::AcceptChar(char c)
	{
		bool accept = *m_it == c;
		m_it += accept;
		return accept;
	}

	inline bool Parser::AcceptStr(const char* str)
	{
		const char* tmp = m_it;
		while(*str != '\0')
		{
			if (!AcceptChar(*str))
			{
				m_it = tmp;
				return false;
			}
			++str;
		}
		return true;
	}

	inline bool Parser::AcceptStrIgnoreCase(const char* str)
	{
		const char* tmp = m_it;
		while(*str != '\0')
		{
			if (!AcceptCharIgnoreCase(*str))
			{
				m_it = tmp;
				return false;
			}
			++str;
		}
		return true;
	}

	inline bool Parser::AcceptCharIgnoreCase(char c)
	{
		bool accept = tolower(*m_it) == c;
		m_it += accept;
		return accept;
	}

	inline bool Parser::AcceptDigit(int& d)
	{
		bool accept = *m_it >= '0' && *m_it <= '9';
		d = static_cast<int>(*m_it - '0');
		m_it += accept;
		return accept;
	}

	inline bool Parser::AcceptWhiteSpace()
	{
		bool accept = *m_it == ' ' || *m_it == '\n' || *m_it == '\t' || *m_it == '\r';
		m_it += accept;
		return accept;
	}

	inline bool Parser::AcceptNonWhiteSpace()
	{
		bool accept = *m_it != '\0' && !(*m_it == ' ' || *m_it == '\n' || *m_it == '\t' || *m_it == '\r');
		m_it += accept;
		return accept;
	}

	inline bool Parser::AcceptFloat(float& value)
	{
		while (AcceptWhiteSpace());

		bool positive = AcceptChar(PLUS) || !AcceptChar(MINUS);
		int d = 0;
		uint64_t mantissa = 0;
		bool accept = false;
		while (AcceptDigit(d))
		{
			mantissa *= 10;
			mantissa += d;
			accept = true;
		}
		int exponent_of_mantissa = 0;

		if (AcceptChar(DOT))
		{
			while (AcceptDigit(d))
			{
				mantissa *= 10;
				mantissa += d;
				++exponent_of_mantissa;
				accept = true;
			}
		}

		if (!accept)
		{
			return false;
		}

		if (AcceptChar('e') || AcceptChar('E'))
		{
			bool expPositive = AcceptChar(PLUS) || !AcceptChar(MINUS);

			if (AcceptDigit(d))
			{
				int exponent = d;

				while (AcceptDigit(d))
				{
					exponent *= 10;
					exponent += d;
				}
				exponent = expPositive ? exponent : -exponent;
				float x = mantissa * misc::pow10(exponent - exponent_of_mantissa);
				value = positive ? x : -x;
				return true;
			}
		}
		else
		{
			float x = mantissa * misc::pow10(-exponent_of_mantissa);
			value = positive ? x : -x;
			return true;
		}

		return true;
	}

	inline bool Parser::AcceptInt(int& value)
	{
		while (AcceptWhiteSpace());

		bool positive = AcceptChar(PLUS) || !AcceptChar(MINUS);
		int d = 0;

		if (AcceptDigit(d))
		{
			int integer = d;
			while (AcceptDigit(d))
			{
				integer *= 10;
				integer += d;
			}
			value = positive ? integer : -integer;
			return true;
		}

		return false;
	}
}
