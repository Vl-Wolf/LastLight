// Fill out your copyright notice in the Description page of Project Settings.


#include "LastLightHealthComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
ULastLightHealthComponent::ULastLightHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void ULastLightHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULastLightHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float ULastLightHealthComponent::GetCurrentHealth()
{
	return Health;
}

/*void ULastLightHealthComponent::SetCurrenHealth(float newHealth)
{
	Health = newHealth;
}*/

bool ULastLightHealthComponent::GetIsAlive()
{
	return bIsAlive;
}

void ULastLightHealthComponent::DeadEvent_Multicast_Implementation()
{
	OnDead.Broadcast();
}

void ULastLightHealthComponent::HealthChangeEvent_Multicast_Implementation(float newHealth, float value)
{
	OnHealthChange.Broadcast(newHealth, value);
}

void ULastLightHealthComponent::ChangeHealthValue_OnServer_Implementation(float ChangeValue)
{
	if(bIsAlive)
	{
		ChangeValue = ChangeValue * CoefDamage;
		Health += ChangeValue;

		HealthChangeEvent_Multicast(Health, ChangeValue);

		if(Health > 100.f)
		{
			Health = 100.0f;
		}
		else
		{
			if(Health < 0.0f)
			{
				bIsAlive = false;
				DeadEvent_Multicast();
			}
		}
	}
}

void ULastLightHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULastLightHealthComponent, Health);
	DOREPLIFETIME(ULastLightHealthComponent, bIsAlive);
}