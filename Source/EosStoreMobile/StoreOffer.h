// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StoreOffer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class EOSSTOREMOBILE_API UStoreOffer : public UObject
{
	GENERATED_BODY()
public:
    UStoreOffer();
    UStoreOffer(const FString& InOfferId , const FText& InTitle, const FText& InPrice , bool bInConsumable, int32 InQuantity );

    UPROPERTY(BlueprintReadOnly, Category="TOOT")
     FString OfferId;
    UPROPERTY(BlueprintReadOnly, Category="TOOT")
    FText Title;
    UPROPERTY(BlueprintReadOnly, Category="TOOT")
    FText Price;
    UPROPERTY(BlueprintReadOnly, Category="TOOT")
    bool bConsumable = true;
    UPROPERTY(BlueprintReadWrite, Category="TOOT")
    int32 Quantity = 0;



	
};
