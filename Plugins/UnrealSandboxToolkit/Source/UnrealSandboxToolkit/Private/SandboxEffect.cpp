

//#include "SandboxEffect.h" // Подключение заголовочного файла SandboxEffect.h

ASandboxEffect::ASandboxEffect() {
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

void ASandboxEffect::BeginPlay() {
	Super::BeginPlay();
}
