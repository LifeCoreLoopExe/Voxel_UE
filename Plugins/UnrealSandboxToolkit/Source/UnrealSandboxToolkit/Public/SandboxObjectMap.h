// Fill out your copyright notice in the Description page of Project Settings.

#pragma once // Защита от множественного включения этого заголовочного файла

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "SandboxObject.h" // Подключение заголовочного файла для объектов песочницы
#include "SandboxEffect.h" // Подключение заголовочного файла для эффектов песочницы
#include "Runtime/Engine/Classes/Engine/DataAsset.h" // Подключение заголовочного файла для работы с Data Asset
#include "SandboxObjectMap.generated.h" // Генерация кода для этого заголовочного файла

// Структура для хранения статических данных объектов песочницы
USTRUCT(BlueprintType) 
struct FSandboxStaticData : public FTableRowBase { 
	GENERATED_BODY() // Генерация тела структуры

	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	FString ObjectName; // Имя объекта
};

// Класс для карты объектов песочницы, наследующий от UDataAsset
UCLASS(BlueprintType, Blueprintable) 
class UNREALSANDBOXTOOLKIT_API USandboxObjectMap : public UDataAsset { 
	GENERATED_BODY() // Генерация тела класса
	
public:	
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") 
	TArray<TSubclassOf<ASandboxObject>> ObjectList; // Список классов объектов песочницы

	//UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	//TMap<uint64, FSandboxStaticData> StaticData; // Карта статических данных объектов (закомментировано)

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") 
	TMap<int32, TSubclassOf<ASandboxEffect>> Effects; // Карта эффектов с их идентификаторами
};
