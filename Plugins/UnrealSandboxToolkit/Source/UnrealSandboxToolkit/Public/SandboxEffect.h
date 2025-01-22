#pragma once // Защита от множественного включения этого заголовочного файла

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса Actor
#include "SandboxEffect.generated.h" // Генерация кода для этого заголовочного файла

// Класс для эффектов в песочнице, наследующий от AActor
UCLASS(BlueprintType, Blueprintable) 
class UNREALSANDBOXTOOLKIT_API ASandboxEffect : public AActor { 
	GENERATED_BODY() // Генерация тела класса

public:	
	ASandboxEffect(); // Конструктор класса

protected:
	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

public:
	// Здесь могут быть добавлены публичные методы и свойства класса
};
