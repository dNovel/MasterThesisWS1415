#pragma once
#include "GameFramework/Actor.h"
#include "UECodeAnalysisProjectile.generated.h"

UCLASS(config=Game)
class AUECodeAnalysisProjectile : public AActor
{

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

public:
	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;
};