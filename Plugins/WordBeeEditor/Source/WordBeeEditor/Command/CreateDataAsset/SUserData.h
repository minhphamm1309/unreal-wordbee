#pragma once
#include "UserData.h"

class SUserData
{
public:
	static UUserData* Get();
	

private:
	static UUserData* Instance;
};
