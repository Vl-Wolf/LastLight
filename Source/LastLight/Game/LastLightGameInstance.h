// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "LastLight/FuncLibrary/Types.h"
#include "LastLight/Weapons/WeaponDefault.h"
#include "LastLightGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LASTLIGHT_API ULastLightGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "WeaponSetting")
		UDataTable* WeaponInfoTable = nullptr;
	UFUNCTION(BlueprintCallable)
		bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo);
};
