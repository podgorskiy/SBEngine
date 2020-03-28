#pragma once
#include "Lexer.h"
#include "ErrorHandler.h"
#include "MaterialSource.h"
#include <spdlog/spdlog.h>


namespace ShaderReader
{
	class Parser : public ErrorHandler
	{
	public:
		Parser() {}

		MaterialSource Parse(const char* source);

	private:
		std::string GetLastTokenUnderscore();

		std::string GetUnderscore(Lexer::Token token);

		void NextToken();

		Lexer::Token GetLastToken();

		bool Accept(const Lexer::Token& token);

		bool AcceptIdentifier(const std::string& id);

		bool ExpectIdentifier(const std::string& id);

		bool AcceptNumber(float& n);

		bool Expect(const Lexer::Token& token);

		bool AcceptShader();

		bool AcceptSubShader();

		bool AcceptPass();

		bool AcceptProperties();

		bool AcceptTags(std::map<std::string, std::string>& tags);

		bool AcceptTag(std::string& key, std::string& value);

		bool AcceptProperty();

		Lexer::Token m_lastToken;
		std::unique_ptr<Lexer> m_lexer;

		MaterialSource m_material;
	};
}


inline MaterialSource ShaderReader::Parser::Parse(const char* source)
{
	m_lexer = std::make_unique<Lexer>(source);
	m_material = MaterialSource();

	NextToken();

	if (!AcceptShader())
	{
		std::string result = GetLastTokenUnderscore();
		SetError("Error, expected shader block, but \"%s\" was found at position %d.\n%s",
		         std::string(GetLastToken().GetSymbolSequence()).c_str(), GetLastToken().GetPosition(),
		         result.c_str());
		return MaterialSource();
	}
	return m_material;
}

inline std::string ShaderReader::Parser::GetLastTokenUnderscore()
{
	int start;
	int end;
	GetLastToken().GetPosition(start, end);
	return m_lexer->GetSymbolSequence().PrintAndHighLight(start, end);
}

inline std::string ShaderReader::Parser::GetUnderscore(Lexer::Token token)
{
	int start;
	int end;
	token.GetPosition(start, end);
	return m_lexer->GetSymbolSequence().PrintAndHighLight(start, end);
}

inline void ShaderReader::Parser::NextToken()
{
	m_lastToken = m_lexer->GetNextToken();
}

inline ShaderReader::Lexer::Token ShaderReader::Parser::GetLastToken()
{
	return m_lastToken;
}

inline bool ShaderReader::Parser::Accept(const Lexer::Token& token)
{
	if (GetLastToken() == token)
	{
		NextToken();
		return true;
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptNumber(float& n)
{
	Lexer::Token token;
	token = GetLastToken();
	if (Accept(Lexer::FLOAT))
	{
		n = atof(std::string(token.GetSymbolSequence()).c_str());
		return true;
	}
	if (Accept(Lexer::INT))
	{
		n = (float)atoi(std::string(token.GetSymbolSequence()).c_str());
		return true;
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptIdentifier(const std::string& id)
{
	if (GetLastToken() == Lexer::Token(Lexer::IDENTIFIER))
	{
		if (std::string(GetLastToken().GetSymbolSequence()) == id)
		{
			NextToken();
			return true;
		}
	}
	return false;
}

inline bool ShaderReader::Parser::Expect(const Lexer::Token& token)
{
	if (Accept(token))
	{
		return true;
	}

	char cExpected = token;
	if (static_cast<char>(GetLastToken()) == '\0')
	{
		std::string result = m_lexer->GetSymbolSequence().PrintAndHighLight(GetLastToken().GetPosition());
		SetError("Error: Unexpected end of input\n%s", result.c_str());
	}
	else
	{
		std::string result = GetLastTokenUnderscore();
		std::string cFound = GetLastToken().GetSymbolSequence();
		SetError("Error: Unexpected token. Expected '%c', but '%s' was found at position: %d.\n%s", cExpected,
		         cFound.c_str(), GetLastToken().GetPosition(), result.c_str());
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptShader()
{
	Lexer::Token token;
	std::string name;

	if (AcceptIdentifier("Shader"))
	{
		token = GetLastToken();
		Expect(Lexer::STRING);

		m_material.name = token.GetSymbolSequence();

		Expect(Lexer::LBRACES);

		AcceptProperties();

		token = GetLastToken();
		if (AcceptSubShader())
		{
			while (AcceptSubShader());
			if (Accept(Lexer::RBRACES))
			{
				return true;
			}
		}
		else
		{
			std::string result = GetUnderscore(token);
			SetError("Error, expected subshader, but \"%s\" was found at position %d.\n%s",
			         std::string(token.GetSymbolSequence()).c_str(), token.GetPosition(),
			         result.c_str());
		}
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptProperties()
{
	Lexer::Token token;
	if (AcceptIdentifier("Properties"))
	{
		Expect(Lexer::LBRACES);

		token = GetLastToken();
		if (AcceptProperty())
		{
			while (AcceptProperty());
			if (Accept(Lexer::RBRACES))
			{
				return true;
			}
		}
		else
		{
			std::string result = GetUnderscore(token);
			SetError("Error, expected property, but \"%s\" was found at position %d.\n%s",
			         std::string(token.GetSymbolSequence()).c_str(), token.GetPosition(),
			         result.c_str());
		}
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptSubShader()
{
	Lexer::Token token;

	if (AcceptIdentifier("SubShader"))
	{
		Expect(Lexer::LBRACES);
		std::map<std::string, std::string> tags;

		AcceptTags(tags);

		token = GetLastToken();
		if (AcceptPass())
		{
			while (AcceptPass());
			if (Accept(Lexer::RBRACES))
			{
				return true;
			}
		}
		else
		{
			std::string result = GetUnderscore(token);
			SetError("Error, expected pass, but \"%s\" was found at position %d.\n%s",
			         std::string(token.GetSymbolSequence()).c_str(), token.GetPosition(),
			         result.c_str());
		}
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptPass()
{
	Lexer::Token token;

	if (AcceptIdentifier("Pass"))
	{
		Expect(Lexer::LBRACES);
		std::map<std::string, std::string> tags;

		AcceptTags(tags);

		const char* types[] = {
				"GLSL_VERTEX", "END_GLSL_VERTEX",
				"GLSL_FRAGMENT", "END_GLSL_FRAGMENT",
				nullptr};

		int models[] = {MaterialSource::ExecutionModelVertex, MaterialSource::ExecutionModelFragment};

		while(true)
		{
			bool should_break = true;
			for (int i = 0; types[i] != nullptr; i+=2)
			{
				if (AcceptIdentifier(types[i]))
				{
					token = GetLastToken();
					int start = token.GetPosition();
					int end = start;

					while (!AcceptIdentifier(types[i + 1]))
					{
						NextToken();
						end = GetLastToken().GetPosition();
					}

					std::string source = SymbolSequence(m_lexer->GetSymbolSequence().const_begin() + start,
							m_lexer->GetSymbolSequence().const_begin() + end);

					m_material.m_sources[(MaterialSource::ExecutionModel)models[i / 2]] = source;
					spdlog::info("vertex shader: {}", source);
					should_break = false;
				}
			}
			if (should_break)
			{
				break;
			}
		}
		if (Expect(Lexer::RBRACES))
		{
			return true;
		}
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptTags(std::map<std::string, std::string>& tags)
{
	if (AcceptIdentifier("Tags"))
	{
		Expect(Lexer::LBRACES);
		std::string key;
		std::string value;

		while (AcceptTag(key, value))
		{
			tags[key] = value;
		}
		if (Expect(Lexer::RBRACES))
		{
			return true;
		}
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptTag(std::string& key, std::string& value)
{
	Lexer::Token token;
	token = GetLastToken();
	if (Accept(Lexer::STRING))
	{
		key = token.GetSymbolSequence();
		Expect(Lexer::EQ);

		token = GetLastToken();
		Expect(Lexer::STRING);
		value = token.GetSymbolSequence();
		spdlog::info("{}{}", key, value);
		return true;
	}
	return false;
}

inline bool ShaderReader::Parser::AcceptProperty()
{
	Lexer::Token token;
	std::string name;
	std::string name2;
	std::string label;

	token = GetLastToken();
	if (Accept(Lexer::IDENTIFIER))
	{
		name = token.GetSymbolSequence();

		if (Expect(Lexer::LPAR))
		{
			token = GetLastToken();
			if (Expect(Lexer::STRING))
			{
				label = token.GetSymbolSequence();
				if (Expect(Lexer::COMA))
				{
					token = GetLastToken();
					if (AcceptIdentifier("Color"))
					{
						name2 = token.GetSymbolSequence();
						if (Expect(Lexer::RPAR) && Expect(Lexer::EQ))
						{
							float x0, x1, x2, x3;
							Expect(Lexer::LPAR);
							AcceptNumber(x0);
							Expect(Lexer::COMA);
							AcceptNumber(x1);
							Expect(Lexer::COMA);
							AcceptNumber(x2);
							Expect(Lexer::COMA);
							AcceptNumber(x3);
							Expect(Lexer::RPAR);
							m_material.properties.emplace_back(name, glm::vec4(x0, x1, x2, x3), MaterialSource::Property::Color);
							return true;
						}
					}
					else if (AcceptIdentifier("Vector"))
					{
						name2 = token.GetSymbolSequence();
						if (Expect(Lexer::RPAR) && Expect(Lexer::EQ))
						{
							float x0, x1, x2, x3;
							Expect(Lexer::LPAR);
							AcceptNumber(x0);
							Expect(Lexer::COMA);
							AcceptNumber(x1);
							Expect(Lexer::COMA);
							AcceptNumber(x2);
							Expect(Lexer::COMA);
							AcceptNumber(x3);
							Expect(Lexer::RPAR);
							m_material.properties.emplace_back(name, glm::vec4(x0, x1, x2, x3), MaterialSource::Property::Vector);
							return true;
						}
					}
					else if (AcceptIdentifier("Range"))
					{
						name2 = token.GetSymbolSequence();
						Expect(Lexer::LPAR);
						float low, high;
						AcceptNumber(low);
						Expect(Lexer::COMA);
						AcceptNumber(high);
						Expect(Lexer::RPAR);
						if (Expect(Lexer::RPAR) && Expect(Lexer::EQ))
						{
							float x;
							AcceptNumber(x);
							m_material.properties.emplace_back(name, x, low, high);
							return true;
						}
					}
					else if (AcceptIdentifier("Float"))
					{
						name2 = token.GetSymbolSequence();
						if (Expect(Lexer::RPAR) && Expect(Lexer::EQ))
						{
							float x;
							AcceptNumber(x);
							m_material.properties.emplace_back(name, x);
							return true;
						}
					}
					else if (AcceptIdentifier("Int"))
					{
						name2 = token.GetSymbolSequence();
						if (Expect(Lexer::RPAR) && Expect(Lexer::EQ))
						{
							Expect(Lexer::INT);
							int i = atoi(std::string(GetLastToken().GetSymbolSequence()).c_str());
							m_material.properties.emplace_back(name, i);
							return true;
						}
					}
					else if (AcceptIdentifier("_2D"))
					{
						name2 = token.GetSymbolSequence();
						if (Expect(Lexer::RPAR) && Expect(Lexer::EQ))
						{
							Expect(Lexer::STRING);
							Expect(Lexer::LBRACES);
							Expect(Lexer::RBRACES);
							m_material.properties.emplace_back(name, MaterialSource::Property::Sampler2D);
							return true;
						}
					}
					else if (AcceptIdentifier("Cube"))
					{
						name2 = token.GetSymbolSequence();
						if (Expect(Lexer::RPAR) && Expect(Lexer::EQ))
						{
							Expect(Lexer::STRING);
							Expect(Lexer::LBRACES);
							Expect(Lexer::RBRACES);
							m_material.properties.emplace_back(name, MaterialSource::Property::SamplerCube);
							return true;
						}
					}
					else if (AcceptIdentifier("3D"))
					{
						name2 = token.GetSymbolSequence();
						if (Expect(Lexer::RPAR) && Expect(Lexer::EQ))
						{
							Expect(Lexer::STRING);
							Expect(Lexer::LBRACES);
							Expect(Lexer::RBRACES);
							m_material.properties.emplace_back(name, MaterialSource::Property::Sampler3D);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}
