// Копирайт Epic Games, все права защищены

#pragma once  // Это защита от повторного включения файла - файл будет подключен только один раз

// Подключаем базовые штуки движка
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// Это главный класс модуля UnrealSandboxToolkit
// Он наследуется от IModuleInterface, который позволяет движку работать с нашим модулем
class FUnrealSandboxToolkitModule : public IModuleInterface
{
public:
	// Тут объявляем два главных метода, которые должен иметь каждый модуль:

	/** Этот метод вызывается когда модуль запускается - тут мы инициализируем всё что нужно */
	virtual void StartupModule() override;

	/** Этот метод вызывается когда модуль выключается - тут мы очищаем за собой */
	virtual void ShutdownModule() override;
};
