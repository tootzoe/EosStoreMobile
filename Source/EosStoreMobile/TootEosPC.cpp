// Fill out your copyright notice in the Description page of Project Settings.


#include "TootEosPC.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineStoreInterfaceV2.h"
#include "Interfaces/OnlinePurchaseInterface.h"
#include "Interfaces/OnlineEntitlementsInterface.h"


#include "TootLog.h"
#include "StoreOffer.h"





void ATootEosPC::QueryOffers()
{
    UE_LOG(LogTemp, Warning, TEXT("msg....%hs") , __func__);

    IOnlineSubsystem* tmpSubSys = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr idPtr = tmpSubSys ? tmpSubSys->GetIdentityInterface() : nullptr;
    IOnlineStoreV2Ptr storePtr = tmpSubSys ? tmpSubSys->GetStoreV2Interface() : nullptr;

    if(idPtr.IsValid() && storePtr.IsValid()){
        const FUniqueNetIdPtr localNetId = idPtr->GetUniquePlayerId(0);

        if(localNetId.IsValid() && idPtr->GetLoginStatus(0) ==  ELoginStatus::Type::LoggedIn){
            FOnlineStoreFilter emptyStroeFilter  = {};
            storePtr->QueryOffersByFilter(*localNetId , emptyStroeFilter ,
                                          FOnQueryOnlineStoreOffersComplete::CreateWeakLambda(this,
                                       [this](bool isSuccess, const TArray<FUniqueOfferId>& offerIdLs , const FString& errStr){
                if(isSuccess){
                    UE_LOG(LogEosStoreMobile, Warning, TEXT("Query Offers successful....%hs") , __func__);
                    CachedOffers.Reset();
                    CachedOffers.Reserve(offerIdLs.Num());
                    IOnlineSubsystem* subSys = Online::GetSubsystem(GetWorld());
                    IOnlineStoreV2Ptr store = subSys ? subSys->GetStoreV2Interface() : nullptr;
                    if(store.IsValid()){
                        for(const FUniqueOfferId& id : offerIdLs){
                            UE_LOG(LogEosStoreMobile, Warning, TEXT("Offer Id: %s....%hs") , *id, __func__);

                            TSharedPtr<FOnlineStoreOffer> offer = store->GetOffer(id);
                            if(offer.IsValid()){
                                UStoreOffer* it = NewObject<UStoreOffer>(this );
                                it->OfferId = id;
                                it->Title= offer->Title;
                                it->Price = offer->PriceText;

                                CachedOffers.Add(it);
                            }else{
                                UE_LOG(LogEosStoreMobile, Warning, TEXT("Get Offer return empty offers.......%hs") , __func__);
                            }
                        }
                    }
                }else{
                     UE_LOG(LogEosStoreMobile, Error, TEXT("Query offers failed with error: %s.......%hs") , *errStr ,__func__);
                }

              //  onquer

            }));
        }
    }





}

void ATootEosPC::BeginPlay()
{
    Super::BeginPlay();

    AutoLogin();



}

void ATootEosPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    IOnlineSubsystem* tmpSubSys = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr tmpId = tmpSubSys ? tmpSubSys->GetIdentityInterface() : nullptr;
    IOnlineEntitlementsPtr tmpEntitlements = tmpSubSys ? tmpSubSys->GetEntitlementsInterface() : nullptr;

    if(tmpId.IsValid()){
        tmpId->ClearOnLoginCompleteDelegate_Handle(0 ,AutoLoginDelegateHandle);
        AutoLoginDelegateHandle.Reset();
    }

}



void ATootEosPC::AutoLogin()
{

    IOnlineSubsystem* tmpSubSys = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr tmpId = tmpSubSys ? tmpSubSys->GetIdentityInterface() : nullptr;
  //  IOnlineEntitlementsPtr tmpEntitlements = tmpSubSys ? tmpSubSys->GetEntitlementsInterface() : nullptr;

    if(tmpId.IsValid()){

        UE_LOG(LogTemp, Warning, TEXT("msg..22222222..%hs") , __func__);

        FUniqueNetIdPtr localNetId = tmpId->GetUniquePlayerId(0);
        if(localNetId.IsValid() && tmpId->GetLoginStatus(0) == ELoginStatus::Type::LoggedIn){
            // do query here...........
            QueryOffers();

            OnAutoLoginComplete.Broadcast(true, TEXT("Auto Login Successfully , query offers now........."));
            return;
        }

        AutoLoginDelegateHandle = tmpId->AddOnLoginCompleteDelegate_Handle(0,
                 FOnLoginCompleteDelegate:: CreateUObject(this, &ThisClass::HandleAutoLoginCompleted));

        if(tmpId->AutoLogin(0)){
            UE_LOG(LogEosStoreMobile, Warning, TEXT("Identity Auto Login called successfully....%hs") , __func__);

            OnAutoLoginComplete.Broadcast(true, TEXT("Identity Auto Login called successfully.........."));
        }else{
            UE_LOG(LogEosStoreMobile, Error, TEXT("Identity Auto Login called Failed....%hs") , __func__);
            tmpId->ClearOnLoginCompleteDelegate_Handle(0,  AutoLoginDelegateHandle);
            AutoLoginDelegateHandle.Reset();
            OnAutoLoginComplete.Broadcast(false, TEXT("Auto Login Failed.........."));
        }


    }else{
        OnAutoLoginComplete.Broadcast(false, TEXT("IdentityInterface is null........."));
    }


}

void ATootEosPC::HandleAutoLoginCompleted(int32 localPlayerNum, bool isSuccessful, const FUniqueNetId & userId, const FString & errorStr)
{
    if(isSuccessful){
        UE_LOG(LogEosStoreMobile, Warning, TEXT("Auto Login Success , player: %s....%hs") ,* userId.ToString() , __func__);
        // query info here.........
    }else{
        UE_LOG(LogEosStoreMobile, Error, TEXT("Auto Login Failed , player: %s....%hs") ,* userId.ToString() , __func__);
    }

    IOnlineSubsystem* tmpSubSys = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr tmpId = tmpSubSys ? tmpSubSys->GetIdentityInterface() : nullptr;


    if(tmpId.IsValid()){
        tmpId->ClearOnLoginCompleteDelegate_Handle(0 , AutoLoginDelegateHandle);
        AutoLoginDelegateHandle.Reset();
    }

    OnAutoLoginComplete.Broadcast(isSuccessful , errorStr);
}
