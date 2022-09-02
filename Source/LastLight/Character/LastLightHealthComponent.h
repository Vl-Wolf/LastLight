// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LastLightHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LASTLIGHT_API ULastLightHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULastLightHealthComponent();

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category="Health")
	FOnHealthChange OnHealthChange;
	
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category="Health")
	FOnDead OnDead;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
		float Health = 100.0f;
	UPROPERTY(Replicated)
		float bIsAlive = true;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
		float CoefDamage = 1.0f;

	UFUNCTION(BlueprintCallable, Category="Health")
		float GetCurrentHealth();
	/*UFUNCTION(BlueprintCallable, Category="Health")
		void SetCurrenHealth(float newHealth);*/
	UFUNCTION(BlueprintCallable, Category="Health")
		bool GetIsAlive();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Health")
		virtual void ChangeHealthValue_OnServer(float ChangeValue);
	UFUNCTION(NetMulticast,Reliable)
		void HealthChangeEvent_Multicast(float newHealth, float value);
	UFUNCTION(NetMulticast, Reliable)
		void DeadEvent_Multicast();
};
