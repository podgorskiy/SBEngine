#include "TTS.h"
#include "TTS_data.h"
#include "unicode_lower.h"
#include <string.h>
#include <fsal.h>
#include <utf8.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

constexpr uint16_t toint(const char* s)
{
	uint16_t r = *s;
	while (*s != '\0') r = (r << 8u) | (uint16_t)*s++;
	return r;
}

void TTS::LoadDict()
{
	fsal::FileSystem fs;
	std::string data = fs.Open("cmudict.txt");

	const char* it = data.c_str();

	auto accept_nl = [&it]() {	if (*it == '\n') { ++it; return true; } return false; };
	auto accept_comment = [&it]() {	if (*it == ';') { ++it; return true; } return false; };
	auto accept_space = [&it]() {	if (*it == ' ') { ++it; return true; } return false; };
	auto accept_double_space = [&it]() {	if (*it == ' ' && *(it +1) == ' ') { ++it; return true; } return false; };
	auto accept_alpha =  [&it](char& c) {	if (*it >= 'A' && *it <= 'Z') { c = *it; ++it; return true; } return false; };
	auto accept_alpha_num =  [&it](char& c) {	if (*it > ' ' && *it <= '~') { c = *it;  ++it; return true; } return false; };
	auto accept_num =  [&it](int& c) {	if (*it >= '0' && *it <= '9') { c = *it - '0'; ++it; return true; } return false; };

	char key[128];
	char phoneme[16];
	phonem_string phonemes;
	while(true)
	{
		if (*it == '\0') break;
		if (accept_comment())
		{
			while (!accept_nl() && *it != '\0') ++it;
			continue;
		}
		char c;
		if (accept_alpha_num(c))
		{
			char* k = key;
			phonemes.clear();
			*k++ = to_lower(c);
			while(accept_alpha_num(c) && *it != '\0')
			{
				*k++ = to_lower(c);
			}
			*k = '\0';
			if (!accept_double_space())
			{
				// If error happened, go to the next line
				while (!accept_nl() && *it != '\0') ++it;
				continue;
			}
			while (accept_space());

			while (!accept_nl())
			{
				char* ph = phoneme;
				while(!accept_space() && *it != '\0')
				{
					int n;
					if (accept_alpha(c))
					{
						accept_num(n);
						*ph++ = c;
					}
					else
					{
						break;
					}
				}
				*ph = '\0';
#define chekph(X) case toint(#X): phonemes.push_back(X); break;

				switch (toint(phoneme))
				{
					chekph(AA)
					chekph(AE)
					chekph(AH)
					chekph(AO)
					chekph(AW)
					chekph(AY)
					chekph(B)
					chekph(CH)
					chekph(D)
					chekph(DH)
					chekph(EH)
					chekph(ER)
					chekph(EY)
					chekph(F)
					chekph(G)
					chekph(HH)
					chekph(IH)
					chekph(IY)
					chekph(JH)
					chekph(K)
					chekph(L)
					chekph(M)
					chekph(N)
					chekph(NG)
					chekph(OW)
					chekph(OY)
					chekph(P)
					chekph(R)
					chekph(S)
					chekph(SH)
					chekph(T)
					chekph(TH)
					chekph(UH)
					chekph(UW)
					chekph(V)
					chekph(W)
					chekph(Y)
					chekph(Z)
					chekph(ZH)
				}
#undef chekph
			}
			m_dict[key] = phonemes;
		}
	}
	printf("Dict size %d\n", (int)m_dict.size());
}


void TTS::SetText(const char* text)
{
    m_text.clear();

	auto length = strlen(text);

	// check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
    auto end_it = utf8::find_invalid(text, text + length);
    if (end_it != text + length)
    {
        printf("Invalid UTF-8 encoding detected at line %s\n", text);
        return;
    }

	utf8::utf8to32(text, text + length, std::back_inserter(m_text));
	m_text.push_back(0);

    for (auto& cp: m_text)
    {
	    cp = to_lower(cp);
    }

//    std::string str;
//	utf8::utf32to8(m_text.begin(), m_text.end(), std::back_inserter(str));
//	printf(str.c_str());
}


void TTS::TextToPhonems()
{
	m_phonems.clear();

	auto is_whitespace = [](char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; };
	auto is_punctiation = [](char c) { return c == '.' || c == ',' || c == '!' || c == '.' || c == '?' || c == '~' || c == '/' || c == '-' || c == ':'; };


	std::string word;
    for (auto& cp: m_text)
    {
	    if (cp < 0x80)
	    {
			if (is_whitespace(cp) || is_punctiation(cp) || cp == 0)
			{
				if (word.size() > 0)
				{
					auto it = m_dict.find(word);
					if (it != m_dict.end())
					{
						auto result = it->second;
						for (auto r: result)
							PushPhonem(r);
					}
					else
					{
						printf("Could not find %s", word.c_str());
					}
				}

				if (is_punctiation(cp))
				{
					PushPause(cp);
				}
				else if (is_whitespace(cp))
				{
					PushPause(' ');
				}
				word.clear();
			}
			else
			{
				word.push_back(cp);
			}
	    }
    }

	PushPause('.');
}

void TTS::TestPhonems()
{
	PushPhonem(P1, P1);
	PushPhonem(AA, D, P1);
	PushPhonem(AE, T, P1);
	PushPhonem(HH, AH, T, P1);
	PushPhonem(AO, T, P1);
	PushPhonem(K, AW, P1);
	PushPhonem(HH, AY, D, P1);
	PushPhonem(B, IY, P1);
	PushPhonem(CH, IY, Z, P1);
	PushPhonem(D, IY, P1);
	PushPhonem(DH, IY, P1);
	PushPhonem(EH, D, P1);
	PushPhonem(HH, ER, T, P1);
	PushPhonem(EY, T, P1);
	PushPhonem(F, IY, P1);
	PushPhonem(G, R, IY, N, P1);
	PushPhonem(HH, IY, P1);
	PushPhonem(IH, T, P1);
	PushPhonem(IY, T, P1);
	PushPhonem(JH, IY, P1);
	PushPhonem(K, IY, P1);
	PushPhonem(L, IY, P1);
	PushPhonem(M, IY, P1);
	PushPhonem(N, IY, P1);
	PushPhonem(P, IH, NG, P1);
	PushPhonem(OW, T, P1);
	PushPhonem(T, OY, P1);
	PushPhonem(P, IY, P1);
	PushPhonem(R, IY, D, P1);
	PushPhonem(S, IY, P1);
	PushPhonem(SH, IY, P1);
	PushPhonem(T, IY, P1);
	PushPhonem(TH, EY, T, AH, P1);
	PushPhonem(HH, UH, D, P1);
	PushPhonem(T, UW, P1);
	PushPhonem(V, IY, P1);
	PushPhonem(W, IY, P1);
	PushPhonem(Y, IY, L, D, P1);
	PushPhonem(Z, IY, P1);
	PushPhonem(S, IY, ZH, ER, P1);
	PushPhonem(P1, P1);
}


void TTS::PushPause(uint8_t c)
{
	switch (c)
	{
		case '.':
		case '!':
		case '?':
			PushPhonem(P1);
			break;
		case ',':
			PushPhonem(P2);
			break;
		case ' ':
			PushPhonem(P4);
			break;
		case '~':
			PushPhonem(P8);
			break;
		default:
			PushPhonem(P4);
			break;
	}
}

void TTS::PushPhonem(uint8_t ph)
{
	m_phonems.push_back(ph);
}

void TTS::PhonemsToPCM()
{
	m_pcm.clear();
	for (auto ph: m_phonems)
	{
		if (ph >= P1)
		{
			switch(ph)
			{
				case P1: for (int i = 0; i < 0x48; ++i) PushSample(nullptr, 0); break;
				case P2: for (int i = 0; i < 0x24; ++i) PushSample(nullptr, 0); break;
				case P4: for (int i = 0; i < 0x12; ++i) PushSample(nullptr, 0); break;
				case P8: for (int i = 0; i < 0x05; ++i) PushSample(nullptr, 0); break;
			}
		}
		else
		{
			for (uint16_t* data = get_phoneme(ph);; ++data)
			{
				uint16_t fw = *data;
				auto snd = get_sound(fw >> 8u);
				for (uint8_t Rep = fw & 0x1F; Rep; --Rep)
					PushSample(snd.data, snd.size);
				if (fw & 0x40u)
					break;
			}
		}
	}
}

void TTS::PausePCM()
{
	for (int i=0; i < (m_fonTime * 22050 * 2 / 1000 + 500) / 1000; ++i)
		m_pcm.push_back(0);
}

void TTS::BitDelay(int16_t v)
{
	for (int i=0; i < (m_bitTime * 22050 * 5 / 1000 + 500) / 1000; ++i)
		m_pcm.push_back(v);
}

void TTS::PushSample(uint8_t* data, uint8_t size)
{
	uint8_t msk=0x80;
	if (size != 0)
	{
		for (; size; size--)
		{
			if (*data & msk)
			{
				BitDelay(127 * 128);
			}
			else
			{
				BitDelay(0);
			}
			if ((msk >>= 1u) == 0)
			{
				msk = 0x80;
				++data;
			}
		}
	}
	PausePCM();
}

template<typename Word>
std::ostream& write_word(std::ostream& outs, Word value, unsigned size = sizeof(Word))
{
	for (; size; --size, value >>= 8u)
	{
		outs.put(static_cast <char> (value & 0xFFu));
	}
	return outs;
}

void TTS::SaveToWav(std::string path)
{
	auto _pcm = m_pcm;
	int alpha = 90;
	for (int i = 1; i < m_pcm.size(); ++i)
	{
		_pcm[i] = alpha * (_pcm[i - 1] + m_pcm[i] - m_pcm[i - 1]) / 100;
	}
	m_pcm = _pcm;
	alpha = 20;
	for (int i = 1; i < m_pcm.size(); ++i)
	{
		_pcm[i] = m_pcm[i] * alpha / 100 + m_pcm[i-1] * (100 - alpha) / 100;
	}
	m_pcm = _pcm;
	for (int i = 1; i < m_pcm.size(); ++i)
	{
		_pcm[i] = m_pcm[i];
	}
	m_pcm = _pcm;

	// Reverb
	int delays[100] = {0};
	int magnitude[100] = {0};
	int low_pass[100] = {0};
	for (int i = 0; i < 100; ++i)
	{
		delays[i] = rand() % 300;
		magnitude[i] = ((rand() % 1000) / 1000.0f / powf(2.0f, delays[i] / 100.0f)) * 1000;
	}

	for (int i = 1; i < m_pcm.size(); ++i)
	{
		int out = 0;
		for (int j = 0; j < 100; ++j)
		{
			int offset = delays[j] * 22050 / 1000;

			if (i > offset)
			{
				int input = (m_pcm[i - offset] * magnitude[j]) / 1000;
				low_pass[j] = low_pass[j] * (100 - alpha) / 100 + input * alpha / 100;
				out += low_pass[j];
			}
		}
		_pcm[i] = out * 30 / 100 + m_pcm[i];
	}
	m_pcm = _pcm;

	std::ofstream f("example.wav", std::ios::binary);

	// Write the file headers
	f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
	write_word(f, 16, 4);  // no extension data
	write_word(f, 1, 2);  // PCM - integer samples
	write_word(f, 1, 2);  // one channels
	write_word(f, 22050, 4);  // samples per second (Hz)
	write_word(f, 44100, 4);  // (Sample Rate * BitsPerSample * Channels) / 8
	write_word(f, 2, 2);  // data block size (size of two integer samples, one for each channel, in bytes)
	write_word(f, 16, 2);  // number of bits per sample (use a multiple of 8)

	// Write the data chunk header
	size_t data_chunk_pos = f.tellp();
	f << "data----";  // (chunk size to be filled in later)

	for (auto v: m_pcm)
	{
		write_word(f, (int) (v), 2);
	}

	// (We'll need the final file size to fix the chunk sizes above)
	size_t file_length = f.tellp();

	// Fix the data chunk header to contain the data size
	f.seekp(data_chunk_pos + 4);
	write_word(f, file_length - data_chunk_pos + 8);

	// Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
	f.seekp(0 + 4);
	write_word(f, file_length - 8, 4);
}
