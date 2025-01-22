

#include "SandboxEffect.h" // Подключаем заголовочный файл SandboxEffect.h

ASandboxEffect::ASandboxEffect() { // Конструктор класса ASandboxEffect
	PrimaryActorTick.bCanEverTick = true; // Разрешаем вызов функции Tick каждый кадр
	bReplicates = true; // Включаем репликацию для этого актёра, чтобы он мог быть синхронизирован по сети
}

void ASandboxEffect::BeginPlay() { // Функция BeginPlay, вызываемая при начале игры
	Super::BeginPlay(); // Вызываем BeginPlay родительского класса
}