
#pragma once // Включает файл только один раз

#include "CoreMinimal.h" // Включает минимальные функции ядра движка
#include "Modules/ModuleManager.h" // Включает менеджер модулей

class FUnrealSandboxToolkitModule : public IModuleInterface { // Объявление класса FUnrealSandboxToolkitModule, наследующегося от IModuleInterface
public:
    virtual void StartupModule() override; // Метод для запуска модуля
    virtual void ShutdownModule() override; // Метод для завершения работы модуля
};
