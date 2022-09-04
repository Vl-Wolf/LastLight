// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LastLight/FuncLibrary/Types.h"
#include "LastLightInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon, FName, WeaponId, FAdditionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange, EWeaponType, TypeAmmo, int32, Cout);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChange, int32, IndexSlot, FAdditionalWeaponInfo, AdditionalInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoAviable, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlots, int32, IndexSlotChange, FWeaponSlot, NewInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponNotHaveRound, int32, IndexSlotWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHaveRound, int32, IndexSlotWeapon);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LASTLIGHT_API ULastLightInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULastLightInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnSwitchWeapon OnSwitchWeapon;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnAmmoChange OnAmmoChange;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnWeaponAdditionalInfoChange OnWeaponAdditionalInfoChange;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnWeaponAmmoEmpty OnWeaponAmmoEmpty;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnWeaponAmmoAviable OnWeaponAmmoAviable;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnUpdateWeaponSlots OnUpdateWeaponSlots;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnWeaponNotHaveRound OnWeaponNotHaveRound;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
		FOnWeaponHaveRound OnWeaponHaveRound;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Weapons")
		TArray<FWeaponSlot> WeaponSlots;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Weapons")
		TArray<FAmmoSlot> AmmoSlots;

	int32 MaxSlotsWeapon = 0;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool SwitchWeaponToIndexByNextPreviosIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);
	bool SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviosIndex, FAdditionalWeaponInfo PreviosWeaponInfo);

	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);
	FName GetWeaponNameBySlotIndex(int32 IndexSlot);
	bool GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType);
	bool GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType);

	UFUNCTION(BlueprintCallable)
		void AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo);

	bool CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AviableAmmoForWeapon);

	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool CheckCanTakeAmmo(EWeaponType AmmoType);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool CheckCanTakeWeapon(int32& FreeSlot);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		TArray<FWeaponSlot> GetWeaponSlots();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		TArray<FAmmoSlot> GetAmmoSlots();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory")
		void InitInventory_OnServer(const TArray<FWeaponSlot>& NewWeaponSlotsInfo, const TArray<FAmmoSlot>& NewAmmoSlotsInfo);

	UFUNCTION(Server, Reliable)
		void SwitchWeaponEvent_OnServer(FName WeaponName, FAdditionalWeaponInfo AdditionalInfo, int32 IndexSlot);
	UFUNCTION(NetMulticast, Reliable)
		void AmmoChangeEvent_Multicast(EWeaponType TypeWeapon, int32 Cout);
	UFUNCTION(NetMulticast, Reliable)
		void WeaponAdditionalInfoChangeEvent_Multicast(int32 IndexWeapon, FAdditionalWeaponInfo AdditionalWeaponInfo);
	UFUNCTION(NetMulticast, Reliable)
		void WeaponAmmoEmptyEvent_Multicast(EWeaponType TypeWeapon);
	UFUNCTION(NetMulticast, Reliable)
		void WeaponAmmoAviableEvent_Multicast(EWeaponType TypeWeapon);
	UFUNCTION(NetMulticast, Reliable)
		void UpdateWeaponSlotsEvent_Multicast(int32 IndexSlotChange, FWeaponSlot NewInfo);
	UFUNCTION(NetMulticast, Reliable)
		void WeaponNotHaveRoundEvent_Multicast(int32 IndexSlotWeapon);
	UFUNCTION(NetMulticast, Reliable)
		void WeaponHaveRoundEvent_Multicast(int32 IndexSlotWeapon);

	
	
	
};
