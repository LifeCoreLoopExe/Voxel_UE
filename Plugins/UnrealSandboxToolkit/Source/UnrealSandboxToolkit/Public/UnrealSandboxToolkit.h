// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once // Защита от множественного включения этого заголовочного файла

#include "CoreMinimal.h" // Подключение минимальных заголовочных файлов ядра Unreal Engine
#include "Modules/ModuleManager.h" // Подключение заголовочного файла для управления модулями

// Определение класса модуля Unreal Sandbox Toolkit
class FUnrealSandboxToolkitModule : public IModuleInterface {
public:

	/** Реализация интерфейса IModuleInterface */
	virtual void StartupModule() override; // Метод для инициализации модуля при запуске
	virtual void ShutdownModule() override; // Метод для завершения работы модуля при отключении
};
