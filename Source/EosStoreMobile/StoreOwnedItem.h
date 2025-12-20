// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StoreOwnedItem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class EOSSTOREMOBILE_API UStoreOwnedItem : public UObject
{
	GENERATED_BODY()

    public:

    UStoreOwnedItem() : ItemId{}{}
     UStoreOwnedItem(const FString& InItemId) : ItemId{InItemId}{}


    UPROPERTY(BlueprintReadOnly, Category="TOOT") FString ItemId;


};
