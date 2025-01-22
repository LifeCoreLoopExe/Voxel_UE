// Copyright Epic Games, Inc. All Rights Reserved. // Авторские права Epic Games, Inc. Все права защищены.

#include "UnrealSandboxToolkit.h" // Подключаем заголовочный файл UnrealSandboxToolkit.h

#define LOCTEXT_NAMESPACE "FUnrealSandboxToolkitModule" // Определяем пространство имен для локализованных текстов

void FUnrealSandboxToolkitModule::StartupModule() // Определяем функцию инициализации модуля
{
	// Этот код будет выполняться после загрузки вашего модуля в память; точное время выполнения указано в файле .uplugin для каждого модуля
}

void FUnrealSandboxToolkitModule::ShutdownModule() // Определяем функцию завершения работы модуля
{
	// Эта функция может быть вызвана во время завершения работы для очистки вашего модуля. Для модулей, которые поддерживают динамическую перезагрузку,
	// мы вызываем эту функцию перед выгрузкой модуля.
}

#undef LOCTEXT_NAMESPACE // Отменяем определение пространства имен
	
IMPLEMENT_MODULE(FUnrealSandboxToolkitModule, UnrealSandboxToolkit) // Реализуем модуль FUnrealSandboxToolkitModule с именем UnrealSandboxToolkit
