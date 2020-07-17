#pragma once
#include "TTS_data.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <map>


class TTS
{
	typedef std::vector<uint32_t> utf32string;
	typedef std::vector<uint8_t> phonem_string;
	typedef std::vector<int16_t> pcm;
public:
	void LoadDict();

	void SetText(const char* str);

	void TextToPhonems();

	void PhonemsToPCM();

	void SaveToWav(std::string path);

	void TestPhonems();
	void PushPhonem(uint8_t ph);

	template<class ... Types>
	void PushPhonem(uint8_t ph, Types ...tail)
	{
		PushPhonem(ph);
		PushPhonem(tail...);
	}
private:
	void PushSample(uint8_t* data, uint8_t size);
	void PushPause(uint8_t c);
	void PausePCM();
	void BitDelay(int16_t v);

	utf32string m_text;
	phonem_string m_phonems;
	pcm m_pcm;
	uint16_t  m_fonTime = 2300;
	uint8_t  m_bitTime = 16;

	std::map<std::string, phonem_string> m_dict;
};
