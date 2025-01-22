#include "SandboxEffect.h"

// Конструктор для ASandboxEffect
ASandboxEffect::ASandboxEffect() {
    PrimaryActorTick.bCanEverTick = true; // Разрешение тиков для этого актора
    bReplicates = true; // Включение репликации для этого актора
}

// Вызывается, когда игра начинается или когда объект создается
void ASandboxEffect::BeginPlay() {
    Super::BeginPlay(); // Вызов функции BeginPlay родительского класса
}
