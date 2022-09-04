// Fill out your copyright notice in the Description page of Project Settings.


#include "LastLightInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "LastLight/Game/LastLightGameInstance.h"


// Sets default values for this component's properties
ULastLightInventoryComponent::ULastLightInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void ULastLightInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULastLightInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool ULastLightInventoryComponent::SwitchWeaponToIndexByNextPreviosIndex(int32 ChangeToIndex, int32 OldIndex,
	FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	bool bIsSuccess = false;
	int8 CurrentIndex = ChangeToIndex;
	if(ChangeToIndex > WeaponSlots.Num() - 1)
	{
		CurrentIndex = 0;
	}
	else
	{
		if(ChangeToIndex < 0)
		{
			CurrentIndex = WeaponSlots.Num() -1;
		}
	}

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;
	int32 NewCurrentIndex = 0;

	if(WeaponSlots.IsValidIndex(CurrentIndex))
	{
		if (!WeaponSlots[CurrentIndex].NameItem.IsNone())
		{
			if(WeaponSlots[CurrentIndex].AdditionalInfo.Round > 0)
			{
				bIsSuccess = true;
			}
			else
			{
				ULastLightGameInstance* myGI = Cast<ULastLightGameInstance>(GetWorld()->GetGameInstance());
				if(myGI)
				{
					FWeaponInfo myInfo;
					myGI->GetWeaponInfoByName(WeaponSlots[CurrentIndex].NameItem, myInfo);

					bool bIsFind = false;
					int8 j = 0;
					while (j < AmmoSlots.Num() && !bIsFind)
					{
						if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
						{
							bIsSuccess = true;
							bIsFind = true;
						}
						j++;
					}
				}
			}
			if(bIsSuccess)
			{
				NewCurrentIndex = CurrentIndex;
				NewIdWeapon = WeaponSlots[CurrentIndex].NameItem;
				NewAdditionalInfo = WeaponSlots[CurrentIndex].AdditionalInfo;
			}
		}
	}
	if(!bIsSuccess)
	{
		int8 iteration = 0;
		int8 SecondIteration = 0;
		int8 tmpIndex = 0;

		while (iteration < WeaponSlots.Num() && !bIsSuccess)
		{
			iteration++;

			if(bIsForward)
			{
				tmpIndex = ChangeToIndex + iteration;
			}
			else
			{
				SecondIteration = WeaponSlots.Num() - 1;
				tmpIndex = ChangeToIndex - iteration;
			}

			if(WeaponSlots.IsValidIndex(tmpIndex))
			{
				if(!WeaponSlots[tmpIndex].NameItem.IsNone())
				{
					if(WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
					{
						bIsSuccess = true;
						NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
						NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
						NewCurrentIndex = tmpIndex;
					}
					else
					{
						FWeaponInfo myInfo;
						ULastLightGameInstance* myGI = Cast<ULastLightGameInstance>(GetWorld()->GetGameInstance());
						myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

						bool bIsFind = false;
						int8 j = 0;
						while (j < AmmoSlots.Num() && !bIsFind)
						{
							if(AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
							{
								bIsSuccess = true;
								NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
								NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
								NewCurrentIndex = tmpIndex;
								bIsFind = true;
							}
							j++;
						}
					}
				}
			}
			else
			{
				if(OldIndex != SecondIteration)
				{
					if (WeaponSlots.IsValidIndex(SecondIteration))
					{
						if(!WeaponSlots[SecondIteration].NameItem.IsNone())
						{
							if(WeaponSlots[SecondIteration].AdditionalInfo.Round > 0)
							{
								bIsSuccess = true;
								NewIdWeapon = WeaponSlots[SecondIteration].NameItem;
								NewAdditionalInfo = WeaponSlots[SecondIteration].AdditionalInfo;
								NewCurrentIndex = SecondIteration;
							}
							else
							{
								FWeaponInfo myInfo;
								ULastLightGameInstance* myGI = Cast<ULastLightGameInstance>(GetWorld()->GetGameInstance());
								myGI->GetWeaponInfoByName(WeaponSlots[SecondIteration].NameItem, myInfo);

								bool bIsFind = false;
								int8 j = 0;
								while (j < AmmoSlots.Num() && !bIsFind)
								{
									if(AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
									{
										bIsSuccess = true;
										NewIdWeapon = WeaponSlots[SecondIteration].NameItem;
										NewAdditionalInfo = WeaponSlots[SecondIteration].AdditionalInfo;
										NewCurrentIndex = SecondIteration;
										bIsFind = true;
									}
									j++;
								}
							}
						}
					}
				}
				else
				{
					if(WeaponSlots.IsValidIndex(SecondIteration))
					{
						if (!WeaponSlots[SecondIteration].NameItem.IsNone())
						{
							if(WeaponSlots[SecondIteration].AdditionalInfo.Round > 0)
							{
								
							}
							else
							{
								FWeaponInfo myInfo;
								ULastLightGameInstance* myGI = Cast<ULastLightGameInstance>(GetWorld()->GetGameInstance());
								myGI->GetWeaponInfoByName(WeaponSlots[SecondIteration].NameItem, myInfo);

								bool bIsFind = false;
								int8 j = 0;
								while (j < AmmoSlots.Num() && !bIsFind)
								{
									if(AmmoSlots[j].WeaponType == myInfo.WeaponType)
									{
										if(AmmoSlots[j].Cout > 0)
										{
											
										}
										else
										{
											UE_LOG(LogTemp, Error, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
										}
									}
									j++;
								}
							}
						}
					}
				}
				if(bIsForward)
				{
					SecondIteration++;
				}
				else
				{
					SecondIteration--;
				}
			}
		}
	}
	if(bIsSuccess)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		SwitchWeaponEvent_OnServer(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
	}

	return bIsSuccess;
}

bool ULastLightInventoryComponent::SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviosIndex,
	FAdditionalWeaponInfo PreviosWeaponInfo)
{
	bool bIsSuccess = false;
	FName ToSwitchIdWeapon;
	FAdditionalWeaponInfo ToSwitchAdditionalInfo;

	ToSwitchIdWeapon = GetWeaponNameBySlotIndex(IndexWeaponToChange);
	ToSwitchAdditionalInfo = GetAdditionalInfoWeapon(IndexWeaponToChange);

	if (!ToSwitchIdWeapon.IsNone())
	{
		SetAdditionalInfoWeapon(PreviosIndex, PreviosWeaponInfo);
		SwitchWeaponEvent_OnServer(ToSwitchIdWeapon, ToSwitchAdditionalInfo, IndexWeaponToChange);

		EWeaponType ToSwitchWeaponType;
		if (GetWeaponTypeByNameWeapon(ToSwitchIdWeapon, ToSwitchWeaponType))
		{
			int8 AviableAmmoForWeapon = -1;
			if(CheckAmmoForWeapon(ToSwitchWeaponType, AviableAmmoForWeapon))
			{
				
			}
		}
		bIsSuccess = true;
	}

	return bIsSuccess;
}

void ULastLightInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if(WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				WeaponSlots[i].AdditionalInfo = NewInfo;
				bIsFind = true;

				WeaponAdditionalInfoChangeEvent_Multicast(IndexWeapon, NewInfo);
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("ULastLightInventoryComponent::SetAdditionalInfoWeapon - Not Found Weapon with index - %d"), IndexWeapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ULastLightInventoryComponent::SetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);
	}
}

FAdditionalWeaponInfo ULastLightInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	FAdditionalWeaponInfo result;
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				result = WeaponSlots[i].AdditionalInfo;
				bIsFind = true;
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);
	}
	
	return result;
}

int32 ULastLightInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 result = -1;
	int8 i = 0;
	bool bISFind = false;
	while (i < WeaponSlots.Num() && !bISFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bISFind = true;
			result = i;
		}
		i++;
	}
	
	return result;
}

FName ULastLightInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName result;

	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		result = WeaponSlots[IndexSlot].NameItem;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetWeaponNameBySlotIndex - Not Correct index Weapon  - %d"), IndexSlot);
	}

	return result;
}

bool ULastLightInventoryComponent::GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleType;
	ULastLightGameInstance* myGI = Cast<ULastLightGameInstance>(GetWorld()->GetGameInstance());
	if(myGI)
	{
		if(WeaponSlots.IsValidIndex(IndexSlot))
		{
			myGI->GetWeaponInfoByName(WeaponSlots[IndexSlot].NameItem, OutInfo);
			WeaponType = OutInfo.WeaponType;
			bIsFind = true;
		}
	}

	return bIsFind;
}

bool ULastLightInventoryComponent::GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleType;
	ULastLightGameInstance* myGI = Cast<ULastLightGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		myGI->GetWeaponInfoByName(IdWeaponName, OutInfo);
		WeaponType = OutInfo.WeaponType;
		bIsFind = true;
	}

	return bIsFind;
}

void ULastLightInventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if(AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Cout += CoutChangeAmmo;
			if(AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
			{
				AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;
			}

			AmmoChangeEvent_Multicast(AmmoSlots[i].WeaponType, AmmoSlots[i].Cout);

			bIsFind = true;
		}
		i++;
	}
}

bool ULastLightInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AviableAmmoForWeapon)
{
	AviableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			bIsFind = true;
			AviableAmmoForWeapon = AmmoSlots[i].Cout;
			if (AmmoSlots[i].Cout > 0)
			{
				return true;
			}
		}
		i++;
	}

	if(AviableAmmoForWeapon <= 0)
	{
		WeaponAmmoEmptyEvent_Multicast(TypeWeapon);
	}
	else
	{
		WeaponAmmoAviableEvent_Multicast(TypeWeapon);
	}

	return false;
	
}

bool ULastLightInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !result)
	{
		if(AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Cout < AmmoSlots[i].MaxCout)
		{
			result = true;
		}
		i++;
	}

	return result;
}

bool ULastLightInventoryComponent::CheckCanTakeWeapon(int32& FreeSlot)
{
	bool bIsFreeSlot = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFreeSlot)
	{
		if(WeaponSlots[i].NameItem.IsNone())
		{
			bIsFreeSlot = true;
			FreeSlot = i;
		}
		i++;
	}

	return bIsFreeSlot;
}

bool ULastLightInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot,
	int32 CurrentIndexWeaponChar)
{
	bool result = false;

	if(WeaponSlots.IsValidIndex(IndexSlot))
	{
		WeaponSlots[IndexSlot] = NewWeapon;
		SwitchWeaponToIndexByNextPreviosIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);
		UpdateWeaponSlotsEvent_Multicast(IndexSlot, NewWeapon);

		result = true;
	}

	return result;
}

TArray<FWeaponSlot> ULastLightInventoryComponent::GetWeaponSlots()
{
	return WeaponSlots;
}

TArray<FAmmoSlot> ULastLightInventoryComponent::GetAmmoSlots()
{
	return AmmoSlots;
}

void ULastLightInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULastLightInventoryComponent, WeaponSlots);
	DOREPLIFETIME(ULastLightInventoryComponent, AmmoSlots);
}

void ULastLightInventoryComponent::InitInventory_OnServer_Implementation(const TArray<FWeaponSlot>& NewWeaponSlotsInfo,
                                                                         const TArray<FAmmoSlot>& NewAmmoSlotsInfo)
{
	WeaponSlots = NewWeaponSlotsInfo;
	AmmoSlots = NewAmmoSlotsInfo;

	MaxSlotsWeapon = WeaponSlots.Num();
	if(WeaponSlots.IsValidIndex(0))
	{
		if(!WeaponSlots[0].NameItem.IsNone())
		{
			SwitchWeaponEvent_OnServer(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
		}
	}
}

void ULastLightInventoryComponent::SwitchWeaponEvent_OnServer_Implementation(FName WeaponName,
                                                                             FAdditionalWeaponInfo AdditionalInfo, int32 IndexSlot)
{
	OnSwitchWeapon.Broadcast(WeaponName, AdditionalInfo, IndexSlot);
}

void ULastLightInventoryComponent::AmmoChangeEvent_Multicast_Implementation(EWeaponType TypeWeapon, int32 Cout)
{
	OnAmmoChange.Broadcast(TypeWeapon, Cout);
}

void ULastLightInventoryComponent::WeaponAdditionalInfoChangeEvent_Multicast_Implementation(int32 IndexWeapon,
                                                                                            FAdditionalWeaponInfo AdditionalWeaponInfo)
{
	OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, AdditionalWeaponInfo);
}

void ULastLightInventoryComponent::WeaponAmmoEmptyEvent_Multicast_Implementation(EWeaponType TypeWeapon)
{
	OnWeaponAmmoEmpty.Broadcast(TypeWeapon);
}

void ULastLightInventoryComponent::WeaponAmmoAviableEvent_Multicast_Implementation(EWeaponType TypeWeapon)
{
	OnWeaponAmmoAviable.Broadcast(TypeWeapon);
}

void ULastLightInventoryComponent::UpdateWeaponSlotsEvent_Multicast_Implementation(int32 IndexSlotChange,
                                                                                    FWeaponSlot NewInfo)
{
	OnUpdateWeaponSlots.Broadcast(IndexSlotChange, NewInfo);
}

void ULastLightInventoryComponent::WeaponNotHaveRoundEvent_Multicast_Implementation(int32 IndexSlotWeapon)
{
	OnWeaponNotHaveRound.Broadcast(IndexSlotWeapon);
}

void ULastLightInventoryComponent::WeaponHaveRoundEvent_Multicast_Implementation(int32 IndexSlotWeapon)
{
	OnWeaponHaveRound.Broadcast(IndexSlotWeapon);
}
