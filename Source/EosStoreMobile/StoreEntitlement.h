// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StoreEntitlement.generated.h"

/**
 * 
 */
UCLASS()
class EOSSTOREMOBILE_API UStoreEntitlement : public UObject
{
	GENERATED_BODY()

      public:
      UStoreEntitlement();
    UStoreEntitlement(const FString& InId ,
                      const FString& InName,
                      const FString& InItemId,
                      const bool bInConsumable,
                      const int32 InCousumedCount,
                      const FString& InEndDate
                      );

    UPROPERTY(BlueprintReadOnly, Category="TOOT")
    FString Id;
    UPROPERTY(BlueprintReadOnly, Category="TOOT")
    FString Name;
    UPROPERTY(BlueprintReadOnly, Category="TOOT")
    FString ItemId;
    UPROPERTY(BlueprintReadOnly, Category="TOOT")
    bool bIsConsumable;
    UPROPERTY(BlueprintReadWrite, Category="TOOT")
    int32 ConsumedCount;
    UPROPERTY(BlueprintReadWrite, Category="TOOT")
    FString EndDate;



      protected:


      private:
	
};
