

#include "SandboxEffect.h" // Включает заголовочный файл для SandboxEffect

ASandboxEffect::ASandboxEffect() {
    PrimaryActorTick.bCanEverTick = true; // Разрешает тикирование актера
    bReplicates = true; // Включает репликацию актера
}

void ASandboxEffect::BeginPlay() {
    Super::BeginPlay(); // Вызывает метод BeginPlay родительского класса
}
