// Fill out your copyright notice in the Description page of Project Settings. // Заполните уведомление об авторских правах на странице описания в настройках проекта.

#pragma once // Указание компилятору включить этот файл только один раз

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "SandboxObject.h" // Подключение заголовочного файла для объекта песочницы
#include "SandboxEffect.h" // Подключение заголовочного файла для эффектов песочницы
#include "Runtime/Engine/Classes/Engine/DataAsset.h" // Подключение заголовочного файла для работы с DataAsset
#include "SandboxObjectMap.generated.h" // Генерация заголовочного файла для класса USandboxObjectMap

USTRUCT(BlueprintType) // Определение структуры FSandboxStaticData, доступной в Blueprints
struct FSandboxStaticData : public FTableRowBase { // Определение структуры FSandboxStaticData, наследующей от FTableRowBase
	GENERATED_BODY() // Генерация тела структуры

	UPROPERTY(EditAnywhere, BlueprintReadOnly) // Свойство ObjectName, редактируемое в редакторе и доступное для чтения в Blueprints
	FString ObjectName; // Имя объекта
};


UCLASS(BlueprintType, Blueprintable) // Определение класса USandboxObjectMap, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API USandboxObjectMap : public UDataAsset { // Определение класса USandboxObjectMap, наследующего от UDataAsset
	GENERATED_BODY() // Генерация тела класса
	
public:	

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Свойство ObjectList, редактируемое в редакторе в категории "UnrealSandbox Toolkit"
	TArray<TSubclassOf<ASandboxObject>> ObjectList; // Список объектов песочницы

	//UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") 
	//TMap<uint64, FSandboxStaticData> StaticData; // Закомментированное свойство для статических данных объектов

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Свойство Effects, редактируемое в редакторе в категории "UnrealSandbox Toolkit"
	TMap<int32, TSubclassOf<ASandboxEffect>> Effects; // Карта эффектов (идентификатор эффекта -> класс эффекта)

}; // Закрывающая скобка для определения класса USandboxObjectMap
