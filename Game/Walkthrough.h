#pragma once
#include <stdio.h>
#include <memory>

class Walkthrough;
typedef std::shared_ptr<Walkthrough>  WalkthroughPtr;


class Walkthrough
{
public:
	void Start();
	void Release();
};
