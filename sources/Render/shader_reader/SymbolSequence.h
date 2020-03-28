#pragma once
#include <vector>
#include <string>
#include <string.h>
#include <spdlog/fmt/fmt.h>


namespace ShaderReader
{
	class Symbol
	{
	public:
		Symbol();
		Symbol(const Symbol& other);
		Symbol(char c, int position = -1);
		bool operator == (const Symbol& other) const;
		bool operator == (const char& other) const;
		operator char() const;
		int GetPosition() const;
		bool IsWhiteSpace() const;
		bool IsAlphabetic() const;
		bool IsDigit() const;
	private:
		char value;
		int position;
	};

	class SymbolSequence
	{
	public:
		typedef std::vector<Symbol>::const_iterator const_iterator;

		SymbolSequence() = default;
		SymbolSequence(const const_iterator& begin, const const_iterator& end);

		operator std::string();

		void Construct(const char* str);

		const_iterator const_begin() const;
		const_iterator const_end() const;

		std::string PrintAndHighLight(int position) const;
		std::string PrintAndHighLight(int start, int end) const;

	private:
		std::vector<Symbol> m_sequence;
	};

	inline Symbol::Symbol() :value('\0'), position(-1)
	{}

	inline Symbol::Symbol(const Symbol& other) : value(other.value), position(other.position)
	{}

	inline Symbol::Symbol(char c, int position /*= -1*/) : value(c), position(position)
	{}

	inline bool Symbol::operator == (const Symbol& other) const
	{
		return value == other.value;
	}

	inline bool Symbol::operator == (const char& other) const
	{
		return value == other;
	}

	inline Symbol::operator char() const
	{
		return value;
	}

	inline int Symbol::GetPosition() const
	{
		return position;
	}

	inline bool Symbol::IsWhiteSpace() const
	{
		return value == ' ' || value == '\t' || value == '\n' || value == '\f' || value == '\r';
	}

	inline bool Symbol::IsAlphabetic() const
	{
		return ((value <= 'z' && value >= 'a') || (value <= 'Z' && value >= 'A') || value == '_');
	}

	inline bool Symbol::IsDigit() const
	{
		return value >= '0' && value <= '9';
	}

	inline SymbolSequence::SymbolSequence(const SymbolSequence::const_iterator& begin,
	                               const SymbolSequence::const_iterator& end)
	{
		for (SymbolSequence::const_iterator it = begin; it != end; ++it)
		{
			m_sequence.push_back(*it);
		}
		m_sequence.emplace_back('\0', m_sequence.size());
	}

	inline SymbolSequence::operator std::string()
	{
		std::string tmp;
		for (const auto& c : m_sequence)
		{
			if (c != '\0')
			{
				tmp.push_back(c);
			}
		}
		return tmp;
	}

	inline void SymbolSequence::Construct(const char* str)
	{
		int length(strlen(str));
		m_sequence.resize(length);
		for (int i = 0; i < length; ++i)
		{
			m_sequence[i] = Symbol(str[i], i);
		}
		m_sequence.emplace_back('\0', m_sequence.size());
	}

	inline SymbolSequence::const_iterator SymbolSequence::const_begin() const
	{
		return m_sequence.begin();
	}

	inline SymbolSequence::const_iterator SymbolSequence::const_end() const
	{
		return m_sequence.end();
	}

	inline std::string SymbolSequence::PrintAndHighLight(int position) const
	{
		return PrintAndHighLight(position, position + 1);
	}

	inline std::string SymbolSequence::PrintAndHighLight(int startPos, int endPos) const
	{
		auto linestart = const_begin();
		auto startIt = const_end();
		int line = 1;
		for (auto it = const_begin(); it != const_end(); ++it)
		{
			if (*it == '\n')
			{
				linestart = it;
				++linestart;
				++line;
			}
			else
			{
				if (it->GetPosition() == startPos)
				{
					startIt = it;
					break;
				}
			}
		}
		std::string result = fmt::format("{:5}: ", line);
		for (auto it = linestart; it != const_end(); ++it)
		{
			if (*it == '\n' || *it == '\0')
			{
				break;
			}
			result += static_cast<char>(*it);
		}
		result += "\n       ";
		for (auto it = linestart; it != const_end() && it != startIt; ++it)
		{
			result += " ";
		}
		for (auto it = startIt; it != const_end() && it->GetPosition() != endPos; ++it)
		{
			result += "~";
		}
		result += "\n";
		return result;
	}
}
