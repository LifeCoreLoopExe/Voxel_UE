
#pragma once // Включает файл только один раз

#include "Engine.h" // Включает основные функции движка
#include "SandboxObject.h" // Включает заголовочный файл для SandboxObject
#include "SandboxEffect.h" // Включает заголовочный файл для SandboxEffect
#include "Runtime/Engine/Classes/Engine/DataAsset.h" // Включает заголовочный файл для DataAsset
#include "SandboxObjectMap.generated.h" // Включает сгенерированный заголовочный файл для SandboxObjectMap

USTRUCT(BlueprintType) // Объявление структуры, доступной для рефлексии UObject и Blueprint
struct FSandboxStaticData : public FTableRowBase {
    GENERATED_BODY() // Макрос для генерации тела структуры

    UPROPERTY(EditAnywhere, BlueprintReadOnly) // Свойство, доступное для редактирования в редакторе и чтения в Blueprint
    FString ObjectName; // Имя объекта
};

UCLASS(BlueprintType, Blueprintable) // Объявление класса, доступного для рефлексии UObject и Blueprint
class UNREALSANDBOXTOOLKIT_API USandboxObjectMap : public UDataAsset { // Объявление класса USandboxObjectMap, наследующегося от UDataAsset
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Свойство, доступное для редактирования в редакторе
    TArray<TSubclassOf<ASandboxObject>> ObjectList; // Список объектов Sandbox

    
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Свойство, доступное для редактирования в редакторе
    TMap<int32, TSubclassOf<ASandboxEffect>> Effects; // Карта эффектов Sandbox
};
