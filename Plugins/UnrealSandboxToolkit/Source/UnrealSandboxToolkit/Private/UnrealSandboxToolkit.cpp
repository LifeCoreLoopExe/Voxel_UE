// Стандартное уведомление об авторских правах Epic Games
// Copyright Epic Games, Inc. All Rights Reserved.

// Подключаем заголовочный файл с объявлениями нашего модуля
#include "UnrealSandboxToolkit.h"

// Определяем пространство имен для локализации текста
// Это нужно для поддержки мультиязычности в модуле
#define LOCTEXT_NAMESPACE "FUnrealSandboxToolkitModule"

// Функция, которая вызывается при загрузке модуля в память
// Здесь можно инициализировать различные компоненты модуля
void FUnrealSandboxToolkitModule::StartupModule()
{
	// Этот код выполнится после загрузки модуля в память
	// Точное время загрузки указано в файле .uplugin для каждого модуля
}

// Функция, которая вызывается при выгрузке модуля
// Здесь нужно освободить все ресурсы и сделать очистку
void FUnrealSandboxToolkitModule::ShutdownModule()
{
	// Эта функция вызывается при выключении модуля для очистки
	// Для модулей, которые поддерживают динамическую перезагрузку,
	// мы вызываем эту функцию перед выгрузкой модуля
}

// Закрываем пространство имен локализации
#undef LOCTEXT_NAMESPACE
	
// Регистрируем модуль в движке Unreal Engine
// Создаёт экземпляр класса FUnrealSandboxToolkitModule и регистрирует его как модуль UnrealSandboxToolkit
IMPLEMENT_MODULE(FUnrealSandboxToolkitModule, UnrealSandboxToolkit)