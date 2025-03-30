#include "WordbeeUtils.h"


TArray<FString> WordbeeUtils::EnumToStringArray(UEnum* EnumClass)
{
	TArray<FString> EnumNames;

	if (!EnumClass)
	{
		return EnumNames;
	}

	int32 NumEnums = EnumClass->NumEnums();

	for (int32 i = 0; i < NumEnums - 1; i++)
	{
		FString EnumName = EnumClass->GetNameStringByIndex(i);
		EnumNames.Add(EnumName);
	}

	return EnumNames;
}
