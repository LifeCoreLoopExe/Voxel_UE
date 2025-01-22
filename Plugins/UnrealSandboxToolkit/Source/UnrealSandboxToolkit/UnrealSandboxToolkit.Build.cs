// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool; // Подключаем инструменты сборки Unreal Engine

public class UnrealSandboxToolkit : ModuleRules // Определяем класс модуля UnrealSandboxToolkit, который наследует от ModuleRules
{
    public UnrealSandboxToolkit(ReadOnlyTargetRules Target) : base(Target) // Конструктор класса, принимающий параметры сборки
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs; // Устанавливаем режим использования предкомпилированных заголовков

        PublicIncludePaths.AddRange( // Добавляем публичные пути для включаемых файлов
            new string[] {
                // добавьте необходимые публичные пути здесь 
            }
        );

        PrivateIncludePaths.AddRange( // Добавляем приватные пути для включаемых файлов
            new string[] {
                // добавьте другие необходимые приватные пути здесь 
            }
        );

        PublicDependencyModuleNames.AddRange( // Добавляем публичные зависимости модуля
            new string[]
            {
                "Core", // Основной модуль Unreal Engine
                // ... добавьте другие публичные зависимости, с которыми вы статически связываетесь здесь ...
            }
        );

        PrivateDependencyModuleNames.AddRange( // Добавляем приватные зависимости модуля
            new string[]
            {
                "CoreUObject", // Модуль для работы с объектами Unreal Engine
                "Engine", // Модуль движка Unreal Engine
                "Slate", // Модуль для работы с пользовательским интерфейсом
                "SlateCore", // Основной модуль Slate для UI
                "Json", // Модуль для работы с JSON
                "JsonUtilities", // Утилиты для работы с JSON
                "AIModule", // Модуль для искусственного интеллекта
                "UMG" // Модуль для пользовательских интерфейсов на основе UMG
                // ... добавьте приватные зависимости, с которыми вы статически связываетесь здесь ...
            }
        );

        DynamicallyLoadedModuleNames.AddRange( // Добавляем динамически загружаемые модули
            new string[]
            {
                // ... добавьте любые модули, которые ваш модуль загружает динамически здесь ...
            }
        );
    }
}
