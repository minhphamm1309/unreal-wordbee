#include "CreateLocalizesCommand.h"

#include "WordBeeEditor/Command/LinkProject/ULinkDocumentCommand.h"

void CreateLocalizesCommand::Execute(const FWordbeeFile& WordbeeFile , FOnLinkDocumentComplete callback)
{
}

// void CreateLocalizesCommand::ImportSegmentsToStrTblCollection(UStringTableCollection* TableCollection, const TArray<FSegment>& Segments, const TArray<FString>& IncludedLocales = TArray<FString>())
// {
// 	if (Segments.Num() == 0 || !TableCollection) {
// 		return;
// 	}
//
// 	for (const FSegment& Segment : Segments) {
// 		for (auto TextEntry : Segment.texts) {
// 			FString LocaleCode = TextEntry.Key;
// 			FSegmentText TextObject = TextEntry.Value;
//
// 			// Nếu IncludedLocales không rỗng và không chứa LocaleCode thì bỏ qua
// 			if (IncludedLocales.Num() > 0 && !IncludedLocales.Contains(LocaleCode)) {
// 				continue;
// 			}
//
// 			if ( TextObject.v.IsEmpty()) {
// 				continue;
// 			}
//
// 			// Lấy bảng StringTable tương ứng với LocaleCode
// 			UStringTable* Table = TableCollection->GetTable(LocaleCode);
// 			if (!Table) {
// 				continue;
// 			}
//
// 			// Thêm hoặc cập nhật mục entry
// 			FStringTableEntry* Entry = Table->GetEntry(Segment.key);
// 			if (!Entry) {
// 				Table->AddEntry(Segment.key, TextObject.v);
// 			} else {
// 				Entry->Value = TextObject.v;
// 			}
// 		}
// 	}
// }
