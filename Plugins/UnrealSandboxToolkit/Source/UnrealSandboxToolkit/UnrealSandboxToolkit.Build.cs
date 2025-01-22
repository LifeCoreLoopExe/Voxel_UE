// Авторские права Epic Games, Inc. Все права защищены.

using UnrealBuildTool;

// Объявление класса UnrealSandboxToolkit, который наследуется от ModuleRules.
public class UnrealSandboxToolkit : ModuleRules
{
    // Конструктор класса UnrealSandboxToolkit, принимающий ReadOnlyTargetRules в качестве параметра.
    public UnrealSandboxToolkit(ReadOnlyTargetRules Target) : base(Target)
    {
        // Установка режима использования предкомпилированных заголовков (PCH).
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Добавление путей для публичных включений.
        PublicIncludePaths.AddRange(
            new string[] {
                // ... добавьте здесь необходимые публичные пути включений ...
            }
        );

        // Добавление путей для приватных включений.
        PrivateIncludePaths.AddRange(
            new string[] {
                // ... добавьте здесь другие необходимые приватные пути включений ...
            }
        );

        // Добавление имен модулей, от которых зависит данный модуль (публичные зависимости).
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                // ... добавьте здесь другие публичные зависимости, которые статически связываются ...
            }
        );

        // Добавление имен модулей, от которых зависит данный модуль (приватные зависимости).
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Json",
                "JsonUtilities",
                "AIModule",
                "UMG"
                // ... добавьте здесь приватные зависимости, которые статически связываются ...
            }
        );

        // Добавление имен модулей, которые динамически загружаются данным модулем.
        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... добавьте здесь любые модули, которые ваш модуль загружает динамически ...
            }
        );
    }
}
