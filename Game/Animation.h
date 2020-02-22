#pragma once
#include <string>
#include <vector>

class Animation
{
public:
	Animation(const std::string& name, const std::vector<int> frames): m_name(name), m_frames(frames), m_frame(0)
	{
	}

	const int& Run()
	{
	    return m_frames[++m_frame % m_frames.size()];
	}

	std::vector<int> m_frames;
private:
	std::string m_name;
	int m_frame;
};
