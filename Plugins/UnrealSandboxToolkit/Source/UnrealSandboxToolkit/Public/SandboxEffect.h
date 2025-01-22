#pragma once // Директива препроцессора, предотвращающая множественное включение этого заголовочного файла

#include "Engine.h" // Подключение заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для базового класса AActor
#include "SandboxEffect.generated.h" // Подключение сгенерированного заголовочного файла для этого класса

UCLASS(BlueprintType, Blueprintable) // Макрос для объявления класса, который может быть использован в Blueprint
class UNREALSANDBOXTOOLKIT_API ASandboxEffect : public AActor { // Объявление класса ASandboxEffect, наследующегося от AActor
	GENERATED_BODY() // Макрос для генерации тела класса

public:
	ASandboxEffect(); // Конструктор класса

protected:
	virtual void BeginPlay() override; // Переопределение метода BeginPlay, который вызывается при начале игры

public:
	// Здесь могут быть добавлены дополнительные публичные методы и переменные
};
