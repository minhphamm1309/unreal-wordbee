#pragma once
#include "WordBeeEditor/Command/LinkProject/ULinkDocumentCommand.h"

struct FWordbeeFile;

class CreateLocalizesCommand
{
public:
	static void Execute(const FWordbeeFile& WordbeeFile, FOnLinkDocumentComplete callback);
};
