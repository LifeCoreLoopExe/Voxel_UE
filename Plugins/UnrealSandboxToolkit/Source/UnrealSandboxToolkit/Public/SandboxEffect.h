#pragma once // Указание компилятору включить этот файл только один раз

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса AActor
#include "SandboxEffect.generated.h" // Генерация заголовочного файла для класса ASandboxEffect

UCLASS(BlueprintType, Blueprintable) // Определение класса ASandboxEffect, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API ASandboxEffect : public AActor { // Определение класса ASandboxEffect, наследующего от AActor
	GENERATED_BODY() // Генерация тела класса
	
public:	
	ASandboxEffect(); // Конструктор класса ASandboxEffect

protected:

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

public:

	// Здесь могут быть добавлены публичные методы и свойства
};
