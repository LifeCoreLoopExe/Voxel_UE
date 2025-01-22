// Авторские права Epic Games, Inc. Все права защищены.

#include "UnrealSandboxToolkit.h" // Подключение заголовочного файла модуля UnrealSandboxToolkit

#define LOCTEXT_NAMESPACE "FUnrealSandboxToolkitModule" // Определение пространства имен для локализации текста

void FUnrealSandboxToolkitModule::StartupModule()
{
    // Этот код будет выполнен после загрузки вашего модуля в память; точное время выполнения указывается в .uplugin файле для каждого модуля
}

void FUnrealSandboxToolkitModule::ShutdownModule()
{
    // Эта функция может быть вызвана во время завершения работы для очистки вашего модуля. Для модулей, поддерживающих динамическую перезагрузку,
    // мы вызываем эту функцию перед выгрузкой модуля.
}

#undef LOCTEXT_NAMESPACE // Отмена определения пространства имен для локализации текста

IMPLEMENT_MODULE(FUnrealSandboxToolkitModule, UnrealSandboxToolkit) // Реализация модуля UnrealSandboxToolkit с использованием класса FUnrealSandboxToolkitModule
