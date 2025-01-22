// Авторские права принадлежат Epic Games, Inc. Все права защищены.

#pragma once
// Директива препроцессора, предотвращающая множественное включение этого файла заголовка.

#include "CoreMinimal.h"
// Включение минимального набора основных заголовочных файлов Unreal Engine.

#include "Modules/ModuleManager.h"
// Включение заголовочного файла для управления модулями Unreal Engine.

class FUnrealSandboxToolkitModule : public IModuleInterface
// Объявление класса FUnrealSandboxToolkitModule, который наследуется от интерфейса IModuleInterface.
{
public:

	/** IModuleInterface implementation */
	// Реализация интерфейса IModuleInterface.

	virtual void StartupModule() override;
	// Виртуальная функция, вызываемая при запуске модуля. Переопределяет базовую реализацию.

	virtual void ShutdownModule() override;
	// Виртуальная функция, вызываемая при завершении работы модуля. Переопределяет базовую реализацию.
};
