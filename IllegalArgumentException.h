#pragma once
#include <exception>
class IllegalArgumentException :
	public std::exception
{
public:
	IllegalArgumentException(void);
	~IllegalArgumentException(void);
};

