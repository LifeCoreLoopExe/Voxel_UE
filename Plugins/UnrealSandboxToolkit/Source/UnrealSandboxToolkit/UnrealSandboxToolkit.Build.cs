// Copyright Epic Games, Inc. All Rights Reserved. // Авторские права Epic Games, Инк. Все права защищены.

using UnrealBuildTool; // Подключаем инструмент сборки Unreal.

public class UnrealSandboxToolkit : ModuleRules // Определяем новый класс модуля UnrealSandboxToolkit, который наследует правила модуля.
{
	public UnrealSandboxToolkit(ReadOnlyTargetRules Target) : base(Target) // Конструктор класса, принимающий целевые правила и передающий их базовому классу.
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs; // Устанавливаем режим использования предварительно скомпилированных заголовков.

		PublicIncludePaths.AddRange( // Добавляем пути к публичным заголовочным файлам.
			new string[] {
				// ... добавьте здесь необходимые публичные пути включения ...
			}
			);
				
		
		PrivateIncludePaths.AddRange( // Добавляем пути к приватным заголовочным файлам.
			new string[] {
				// ... добавьте здесь необходимые другие приватные пути включения ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange( // Добавляем публичные зависимости модуля.
			new string[]
			{
				"Core", // Основной модуль Unreal Engine.
				// ... добавьте здесь другие публичные зависимости, с которыми вы связываете статически ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange( // Добавляем приватные зависимости модуля.
			new string[]
			{
				"CoreUObject", // Модуль для работы с объектами Unreal.
				"Engine", // Основной модуль игрового движка.
				"Slate", // Модуль для создания пользовательских интерфейсов.
				"SlateCore", // Основной модуль для работы с Slate.
				"Json", // Модуль для работы с JSON.
				"JsonUtilities", // Утилиты для работы с JSON.
				"AIModule", // Модуль для искусственного интеллекта.
				"UMG" // Модуль для работы с пользовательскими интерфейсами на основе UMG.
				// ... добавьте здесь приватные зависимости, с которыми вы связываете статически ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange( // Добавляем динамически загружаемые модули.
			new string[]
			{
				// ... добавьте здесь любые модули, которые ваш модуль загружает динамически ...
			}
			);
	}
}
