// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealSandboxToolkit.h"

#define LOCTEXT_NAMESPACE "FUnrealSandboxToolkitModule"

void FUnrealSandboxToolkitModule::StartupModule()
{
    // Этот код будет выполнен после загрузки вашего модуля в память; точное время выполнения указано в файле .uplugin для каждого модуля
}

void FUnrealSandboxToolkitModule::ShutdownModule()
{
    // Эта функция может быть вызвана во время завершения работы для очистки вашего модуля. Для модулей, поддерживающих динамическую перезагрузку,
    // мы вызываем эту функцию перед выгрузкой модуля.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealSandboxToolkitModule, UnrealSandboxToolkit)
