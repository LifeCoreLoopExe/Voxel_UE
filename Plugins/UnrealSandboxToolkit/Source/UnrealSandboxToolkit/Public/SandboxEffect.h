#pragma once // Включает файл только один раз

#include "Engine.h" // Включает основные функции движка
#include "GameFramework/Actor.h" // Включает класс Actor
#include "SandboxEffect.generated.h" // Включает сгенерированный заголовочный файл для SandboxEffect

UCLASS(BlueprintType, Blueprintable) // Объявление класса, доступного для рефлексии UObject и Blueprint
class UNREALSANDBOXTOOLKIT_API ASandboxEffect : public AActor { // Объявление класса ASandboxEffect, наследующегося от AActor
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    ASandboxEffect(); // Конструктор

protected:
    virtual void BeginPlay() override; // Метод, вызываемый при начале игры

public:
    // Дополнительные публичные методы и свойства могут быть добавлены здесь
};
