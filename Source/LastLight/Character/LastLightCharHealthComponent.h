// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LastLight/Character/LastLightHealthComponent.h"
#include "LastLightCharHealthComponent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChange, float, Shield, float, Damage);

UCLASS()
class LASTLIGHT_API ULastLightCharHealthComponent : public ULastLightHealthComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnShieldChange OnShieldChange;

	FTimerHandle TimerHandle_CoolDownShieldTimer;
	FTimerHandle TimerHandle_ShieldRecoveryRateTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float CoolDownShieldRecoveryTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoveryValue = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoveryRate = 0.1f;

protected:

	float Shield = 100.0f;

public:

	
	void ChangeHealthValue_OnServer(float ChangeValue) override;
	float GetCurrentShield();
	void ChangeShieldValue(float ChangeValue);
	void CoolDownShieldEnd();
	void RecoveryShield();

	/*UFUNCTION(BlueprintCallable)
		float GetShieldValue();*/
	
	UFUNCTION(NetMulticast, Reliable)
		void ShieldChangeEvent_Multicast(float newShield, float Damage);
	
};
