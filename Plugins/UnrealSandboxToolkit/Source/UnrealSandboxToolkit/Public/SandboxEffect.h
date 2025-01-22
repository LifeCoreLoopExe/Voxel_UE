# pragma once  // Это защита от повторного включения файла - файл будет подключен только один раз

// Подключаем необходимые заголовочные файлы
#include "Engine.h"  // Основной файл движка Unreal Engine
#include "GameFramework/Actor.h"  // Базовый класс Actor, от которого мы будем наследоваться
#include "SandboxEffect.generated.h"  // Автоматически сгенерированный файл для Unreal Engine

// Декораторы класса:
// UCLASS - говорит Unreal что это игровой класс
// BlueprintType - позволяет создавать переменные этого типа в Blueprint
// Blueprintable - позволяет создавать Blueprint на основе этого класса
UCLASS(BlueprintType, Blueprintable)
class UNREALSANDBOXTOOLKIT_API ASandboxEffect : public AActor {  // Наш класс эффектов, наследуется от Actor
	GENERATED_BODY()  // Макрос, который добавляет стандартный функционал Unreal Engine
	
public:	
	ASandboxEffect();  // Конструктор класса - создает новый экземпляр эффекта

protected:
	// Функция, которая вызывается, когда эффект появляется в игре
	virtual void BeginPlay() override;

public:
	// Здесь можно добавить публичные функции и переменные
	
};
