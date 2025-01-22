// Заполните информацию об авторских правах на странице описания настроек проекта.

#pragma once

#include "Engine.h"
#include "SandboxObject.h"
#include "SandboxEffect.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "SandboxObjectMap.generated.h"

// Определение структуры данных для статических данных песочницы
USTRUCT(BlueprintType)
struct FSandboxStaticData : public FTableRowBase {
	GENERATED_BODY()

	// Свойство для имени объекта, доступное для редактирования и чтения в Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ObjectName;
};

// Определение класса для карты объектов песочницы, наследуемого от UDataAsset
UCLASS(BlueprintType, Blueprintable)
class UNREALSANDBOXTOOLKIT_API USandboxObjectMap : public UDataAsset {
	GENERATED_BODY()

public:

	// Свойство для списка объектов песочницы, доступное для редактирования в редакторе
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	TArray<TSubclassOf<ASandboxObject>> ObjectList;

	// Закомментированное свойство для статических данных песочницы
	// UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	// TMap<uint64, FSandboxStaticData> StaticData;

	// Свойство для эффектов песочницы, доступное для редактирования в редакторе
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	TMap<int32, TSubclassOf<ASandboxEffect>> Effects;

};
