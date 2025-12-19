// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TootEosPC.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOSSASyncRslSignature, bool, bWasSuccessful, const FString&, Error);


UCLASS()
class EOSSTOREMOBILE_API ATootEosPC : public APlayerController
{
	GENERATED_BODY()
	
    public:

    UPROPERTY(BlueprintAssignable, Category="TOOT")
    FOnOSSASyncRslSignature OnAutoLoginComplete;
	
    //
    UFUNCTION(BlueprintCallable, Category = "TOOT")
    void QueryOffers();

    UFUNCTION(BlueprintPure, Category = "TOOT")
    void FetchCachedOffers(TArray< class UStoreOffer * >&OutArr) const { OutArr = CachedOffers ; }

// AActor interface
protected:
        virtual void BeginPlay() override;


        virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

        UFUNCTION(BlueprintCallable, Category = "TOOT")
        void AutoLogin();

        void HandleAutoLoginCompleted(int32, bool , const FUniqueNetId& , const FString&);

        FDelegateHandle AutoLoginDelegateHandle;

private:

        UPROPERTY( )
        TArray< class UStoreOffer * > CachedOffers;
 };
