
#include "UnrealSandboxToolkit.h" // Включает заголовочный файл для UnrealSandboxToolkit

#define LOCTEXT_NAMESPACE "FUnrealSandboxToolkitModule" // Определяет пространство имен для локализации текста

void FUnrealSandboxToolkitModule::StartupModule() {
    // Этот код будет выполнен после загрузки модуля в память; точное время выполнения указывается в .uplugin файле для каждого модуля
}

void FUnrealSandboxToolkitModule::ShutdownModule() {
    // Эта функция может быть вызвана во время завершения работы для очистки модуля. Для модулей, поддерживающих динамическую перезагрузку,
    // мы вызываем эту функцию перед выгрузкой модуля.
}

#undef LOCTEXT_NAMESPACE // Удаляет определение пространства имен для локализации текста

IMPLEMENT_MODULE(FUnrealSandboxToolkitModule, UnrealSandboxToolkit) // Реализует модуль UnrealSandboxToolkit с использованием класса FUnrealSandboxToolkitModule
