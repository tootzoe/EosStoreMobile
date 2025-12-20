// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TootEosPC.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOSSASyncRslSignature, bool, bWasSuccessful, const FString&, Error);


class UStoreOffer;
class UStoreOwnedItem;
class UStoreEntitlement;

UCLASS()
class EOSSTOREMOBILE_API ATootEosPC : public APlayerController
{
	GENERATED_BODY()
	
    public:

    UPROPERTY(BlueprintAssignable, Category="TOOT")
    FOnOSSASyncRslSignature OnAutoLoginComplete;
    UPROPERTY(BlueprintAssignable, Category="TOOT")
    FOnOSSASyncRslSignature OnQueryOffersComplete;
    UPROPERTY(BlueprintAssignable, Category="TOOT")
    FOnOSSASyncRslSignature OnCheckoutComplete;
    UPROPERTY(BlueprintAssignable, Category="TOOT")
    FOnOSSASyncRslSignature OnQueryEntitlementComplete;
    UPROPERTY(BlueprintAssignable, Category="TOOT")
    FOnOSSASyncRslSignature OnQueryReceiptsComplete;
	
    //
    UFUNCTION(BlueprintCallable, Category = "TOOT")
    void QueryOffers();

    UFUNCTION(BlueprintCallable, Category = "TOOT")
    void QueryEntitlements();
    //
    UFUNCTION(BlueprintCallable, Category = "TOOT")
    void QueryReceipts(bool bRestoreReceipts = false);

    UFUNCTION(BlueprintPure, Category = "TOOT")
    void FetchCachedOffers(TArray<   UStoreOffer * >&OutArr) const { OutArr = CachedOffers ; }

    UFUNCTION(BlueprintPure, Category = "TOOT")
    void FetchCachedOwnedItems(TArray<   UStoreOwnedItem * >&OutArr) const { OutArr = CachedOwnedItems ; }

    // Checkout is performed in the EOS SDK Overlay and isn't supported in-editor -  Test in a standalone game
    UFUNCTION(BlueprintCallable, Category = "TOOT" , meta = (AutoCreateRefTerm = "OffersToCheckout"))
    void CheckoutOffers(const TArray<UStoreOffer * >& OffersToCheckout);



// AActor interface
protected:
        virtual void BeginPlay() override;


        virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

        UFUNCTION(BlueprintCallable, Category = "TOOT")
        void AutoLogin();

        void HandleAutoLoginCompleted(int32, bool , const FUniqueNetId& , const FString&);
        void RedeemEntitlement(FString EntitlementId);

        FDelegateHandle AutoLoginDelegateHandle;
        FDelegateHandle QueryEntitlementDelegateHandle;


        void HandleQueryEntitlementCompleted(bool bWasSuccessful , const FUniqueNetId& UserId ,
                                             const FString& Namespace, const FString& Error);

private:

        UPROPERTY( )
        TArray<   UStoreOffer * > CachedOffers;
        UPROPERTY( )
        TArray<   UStoreOwnedItem * > CachedOwnedItems;
        UPROPERTY( )
        TArray<   UStoreEntitlement * > CachedEntitlements;
 };
