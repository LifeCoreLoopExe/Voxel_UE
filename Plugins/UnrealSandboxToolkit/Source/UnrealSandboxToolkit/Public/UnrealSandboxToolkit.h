// Copyright Epic Games, Inc. All Rights Reserved. // Авторские права Epic Games, Inc. Все права защищены.

#pragma once // Указание компилятору включить этот файл только один раз

#include "CoreMinimal.h" // Подключение минимального заголовочного файла ядра Unreal Engine
#include "Modules/ModuleManager.h" // Подключение заголовочного файла для управления модулями

class FUnrealSandboxToolkitModule : public IModuleInterface // Объявление класса модуля FUnrealSandboxToolkitModule, наследующего от интерфейса IModuleInterface
{
public:

	/** IModuleInterface implementation */ // Реализация интерфейса IModuleInterface
	virtual void StartupModule() override; // Переопределение метода StartupModule для инициализации модуля
	virtual void ShutdownModule() override; // Переопределение метода ShutdownModule для завершения работы модуля
};
