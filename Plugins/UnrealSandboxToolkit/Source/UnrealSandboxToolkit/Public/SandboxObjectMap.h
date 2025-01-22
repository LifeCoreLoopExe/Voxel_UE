// Этот файл является заголовочным файлом для карты объектов в песочнице Unreal Engine

#pragma once  // Защита от повторного включения файла

// Подключаем необходимые заголовочные файлы
#include "Engine.h"
#include "SandboxObject.h"
#include "SandboxEffect.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "SandboxObjectMap.generated.h"

// Определяем структуру для хранения статических данных объекта
// BlueprintType позволяет использовать эту структуру в Blueprint
USTRUCT(BlueprintType)
struct FSandboxStaticData : public FTableRowBase {
	GENERATED_BODY()  // Макрос для генерации кода Unreal Engine

	// Свойство, которое можно редактировать в редакторе и читать из Blueprint
	// Хранит имя объекта как строку
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ObjectName;
};

// Определяем класс карты объектов
// BlueprintType позволяет создавать Blueprint на основе этого класса
// Blueprintable позволяет наследоваться от этого класса в Blueprint
UCLASS(BlueprintType, Blueprintable)
class UNREALSANDBOXTOOLKIT_API USandboxObjectMap : public UDataAsset {
	GENERATED_BODY()  // Макрос для генерации кода Unreal Engine
	
public:	
	// Массив классов объектов песочницы
	// Можно редактировать в редакторе в категории "UnrealSandbox Toolkit"
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	TArray<TSubclassOf<ASandboxObject>> ObjectList;

	// Закомментированное свойство - карта статических данных
	//UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	//TMap<uint64, FSandboxStaticData> StaticData;

	// Карта эффектов, где ключ - целое число, а значение - класс эффекта
	// Можно редактировать в редакторе в категории "UnrealSandbox Toolkit"
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	TMap<int32, TSubclassOf<ASandboxEffect>> Effects;
};
