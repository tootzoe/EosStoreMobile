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
#include "StoreOwnedItem.h"
#include "StoreEntitlement.h"





void ATootEosPC::QueryOffers()
{
   OnQueryOffersComplete.Broadcast(false, TEXT("QueryOffers enter.... "));

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

              OnQueryOffersComplete.Broadcast(isSuccess, errStr);

            }));
        }else{
            UE_LOG(LogEosStoreMobile, Error, TEXT("Player logged out....%hs") , __func__);
            OnQueryOffersComplete.Broadcast(false, TEXT("Player logged out.... "));
        }
    }else{
        OnQueryOffersComplete.Broadcast(false, TEXT("Identity and/or store interface null...."));
    }

}

void ATootEosPC::QueryEntitlements()
{
    OnQueryEntitlementComplete.Broadcast(false, TEXT("Query Entitlements enter....") );

    IOnlineSubsystem* tmpOLSys  = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr idPtr = tmpOLSys ? tmpOLSys->GetIdentityInterface() : nullptr;
    IOnlineEntitlementsPtr entilPtr = tmpOLSys ? tmpOLSys->GetEntitlementsInterface() : nullptr;

    if(!idPtr.IsValid() || !entilPtr.IsValid()){
        UE_LOG(LogEosStoreMobile, Error, TEXT("Indentity and/or entitlement interface invalid....%hs") , __func__);
        return;
    }

    const FUniqueNetIdPtr localNetId = idPtr->GetUniquePlayerId(0);

    if(!localNetId.IsValid() || idPtr->GetLoginStatus(0) != ELoginStatus::Type::LoggedIn){
        UE_LOG(LogEosStoreMobile, Error, TEXT("Player logged out......%hs") , __func__);
        return;
    }

    QueryEntitlementDelegateHandle = entilPtr->AddOnQueryEntitlementsCompleteDelegate_Handle(
                FOnQueryEntitlementsCompleteDelegate::  CreateUObject(this, &ThisClass::HandleQueryEntitlementCompleted)  );

    if(entilPtr->QueryEntitlements(*localNetId , TEXT(""))){
        UE_LOG(LogEosStoreMobile, Warning, TEXT("Entitlement handler , query entitlements called successfully....%hs") , __func__);
    }else{
        UE_LOG(LogEosStoreMobile, Error, TEXT("Entitlement handler , query entitlements Failed  ....%hs") , __func__);
        entilPtr->ClearOnQueryEntitlementsCompleteDelegate_Handle(QueryEntitlementDelegateHandle);
        QueryEntitlementDelegateHandle.Reset();
        OnQueryEntitlementComplete.Broadcast(false, TEXT("Query Entitlements failed....") );
    }


}

void ATootEosPC::QueryReceipts(bool bRestoreReceipts)
{
    OnQueryReceiptsComplete.Broadcast(false , TEXT("Query Receipts(query owning relationship) enter...."));


    IOnlineSubsystem* tmpOLSys  = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr idPtr = tmpOLSys ?tmpOLSys->GetIdentityInterface() : nullptr;
    IOnlinePurchasePtr purchasePtr = tmpOLSys ?tmpOLSys->GetPurchaseInterface() : nullptr;


    if(idPtr.IsValid() && purchasePtr.IsValid()){
        const FUniqueNetIdPtr localNetId = idPtr->GetUniquePlayerId(0);

        if(localNetId.IsValid() && idPtr->GetLoginStatus(0) == ELoginStatus::Type::LoggedIn){
            purchasePtr->QueryReceipts(*localNetId , bRestoreReceipts ,
                                       FOnQueryReceiptsComplete::    CreateWeakLambda(this,
                             [this , localNetId](const FOnlineError& err){
                if(err.bSucceeded){
                    UE_LOG(LogEosStoreMobile, Warning, TEXT("Query receipts success....%hs") , __func__);

                    if(IsValid(this)){
                        IOnlineSubsystem* tmpOLSys  = Online::GetSubsystem(GetWorld());
                        IOnlinePurchasePtr tmpPurchase = tmpOLSys ?tmpOLSys->GetPurchaseInterface() : nullptr;

                        if(tmpPurchase.IsValid()){
                            TArray<FPurchaseReceipt> newReceipts;
                            tmpPurchase->GetReceipts(*localNetId, newReceipts);
                            CachedOwnedItems.Reset();

                            for (const auto& it : newReceipts) {
                                for (const auto& offer : it.ReceiptOffers) {
                                if(offer.LineItems.IsEmpty()) continue;

                                  UStoreOwnedItem *tmpIt = NewObject<UStoreOwnedItem>(this   );
                                  tmpIt->ItemId = offer.LineItems[0].UniqueId;
                                  CachedOwnedItems.Add(tmpIt);
                                  UE_LOG(LogEosStoreMobile, Warning, TEXT("Add new receipts: transactionID : %s , offerID : %s  ....%hs") , *it.TransactionId  , *offer.OfferId, __func__);

                                }

                            }

                        }
                    }
                }else{

                    UE_LOG(LogEosStoreMobile, Error, TEXT("Query receipts Failed, err: %s ....%hs") , *err.ToLogString() ,  __func__);

                }

                OnQueryReceiptsComplete.Broadcast(err.bSucceeded, err.ToLogString());
            } ));
        }else{
             UE_LOG(LogEosStoreMobile, Error, TEXT("Failed , Player logged out ....%hs") ,  __func__);
        }
    }else{
         OnQueryReceiptsComplete.Broadcast(false,  TEXT("Identity and/or purchase interface invalid...."));
    }


}



void ATootEosPC::CheckoutOffers(const TArray<UStoreOffer *> &OffersToCheckout)
{

     OnCheckoutComplete.Broadcast(false , TEXT("Checkout Offers enter.... "));

    FPurchaseCheckoutRequest tmpCheckoutReq = {};

    for (const auto& it : OffersToCheckout) {

        tmpCheckoutReq.AddPurchaseOffer("", it->OfferId , it->Quantity, it->bConsumable );
    }

    if(tmpCheckoutReq.PurchaseOffers.IsEmpty()){
        UE_LOG(LogEosStoreMobile, Warning, TEXT("Nothing to checkout....%hs") , __func__);
        OnCheckoutComplete.Broadcast(false , TEXT("Nothing to checkout.... "));
        return;
    }

    IOnlineSubsystem* tmpOLSys  = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr idPtr = tmpOLSys ? tmpOLSys->GetIdentityInterface() : nullptr;
    IOnlinePurchasePtr purchasePtr = tmpOLSys ? tmpOLSys->GetPurchaseInterface() : nullptr;


    if(!idPtr.IsValid() || !purchasePtr.IsValid()){
        OnCheckoutComplete.Broadcast(false , TEXT("Indentity and/or purchase interface invalid.... "));
        return;
    }


    const FUniqueNetIdPtr localId = idPtr->GetUniquePlayerId(0);

    if(!localId.IsValid() || idPtr->GetLoginStatus(0) != ELoginStatus::Type::LoggedIn){
        OnCheckoutComplete.Broadcast(false , TEXT("Player logged out... "));
        return;
    }

    purchasePtr->Checkout(*localId , tmpCheckoutReq , FOnPurchaseCheckoutComplete:: CreateWeakLambda(this,
              [this  ](const FOnlineError& err, const TSharedRef <FPurchaseReceipt> & receipt ){
        if(err.bSucceeded){

            if(receipt->ReceiptOffers.Num() == 0){
                UE_LOG(LogEosStoreMobile, Warning, TEXT("Checkout successed , but empty receips....%hs") , __func__);
            }else{
                UE_LOG(LogEosStoreMobile, Warning, TEXT("Checkout successed , loop all receipts ....%hs") , __func__);
                for (const auto& it : receipt->ReceiptOffers) {
                    UE_LOG(LogEosStoreMobile, Warning, TEXT("checkout done: transcationId: %s, offerId: %s.... %hs") , *receipt->TransactionId, *it.OfferId , __func__);

                }
            }

            QueryEntitlements();
        }else{
            UE_LOG(LogEosStoreMobile, Error, TEXT("Checkout failed , err: %s....%hs") , *err.ToLogString() ,__func__);
        }

        OnCheckoutComplete.Broadcast(err.bSucceeded, err.ToLogString());

    } ));

}

void ATootEosPC::BeginPlay()
{
    Super::BeginPlay();

   // AutoLogin();

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

    if(tmpEntitlements.IsValid()){
        tmpEntitlements->ClearOnQueryEntitlementsCompleteDelegate_Handle(QueryEntitlementDelegateHandle);
        QueryEntitlementDelegateHandle.Reset();
    }

}



void ATootEosPC::AutoLogin()
{

    IOnlineSubsystem* tmpSubSys = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr tmpId = tmpSubSys ? tmpSubSys->GetIdentityInterface() : nullptr;
  //  IOnlineEntitlementsPtr tmpEntitlements = tmpSubSys ? tmpSubSys->GetEntitlementsInterface() : nullptr;

    if(tmpId.IsValid()){

        FUniqueNetIdPtr localNetId = tmpId->GetUniquePlayerId(0);
        if(localNetId.IsValid() && tmpId->GetLoginStatus(0) == ELoginStatus::Type::LoggedIn){
            // do query here...........
            // QueryOffers();
            // QueryReceipts();
            // QueryEntitlements();

            OnAutoLoginComplete.Broadcast(true, TEXT("Auto Login Successfully , query offers Done........."));
            return;
        }

        AutoLoginDelegateHandle = tmpId->AddOnLoginCompleteDelegate_Handle(0,
                 FOnLoginCompleteDelegate:: CreateUObject(this, &ThisClass::HandleAutoLoginCompleted));


        if(tmpId->AutoLogin(0)){
           // UE_LOG(LogEosStoreMobile, Warning, TEXT("Identity Auto Login called successfully....%hs") , __func__);
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
    OnAutoLoginComplete.Broadcast(false , FString::Printf( TEXT("%hs enter....") ,  __func__) );

    if(isSuccessful){
        UE_LOG(LogEosStoreMobile, Warning, TEXT("Auto Login Success , player: %s....%hs") ,* userId.ToString() , __func__);
        // query info here.........
        // QueryOffers();
        // QueryReceipts();
        // QueryEntitlements();
    }else{
        UE_LOG(LogEosStoreMobile, Error, TEXT("Auto Login Failed , player: %s....   %hs") ,* userId.ToString() , __func__);
    }

    IOnlineSubsystem* tmpSubSys = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr tmpId = tmpSubSys ? tmpSubSys->GetIdentityInterface() : nullptr;


    if(tmpId.IsValid()){
        tmpId->ClearOnLoginCompleteDelegate_Handle(0 , AutoLoginDelegateHandle);
        AutoLoginDelegateHandle.Reset();
    }

    OnAutoLoginComplete.Broadcast(isSuccessful , errorStr);
}

void ATootEosPC::RedeemEntitlement(FString EntitlementId)
{
    UE_LOG(LogTemp, Warning, TEXT("msg....%hs") , __func__);

    if(!IsRunningDedicatedServer()){
        UE_LOG(LogEosStoreMobile, Error, TEXT("Should NOT called on Client....%hs") , __func__);
        return;
    }

    IOnlineSubsystem* tmpOLSys  = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr idPtr = tmpOLSys ?tmpOLSys->GetIdentityInterface() : nullptr;
    IOnlinePurchasePtr purchasePtr = tmpOLSys ?tmpOLSys->GetPurchaseInterface() : nullptr;

    if( !idPtr.IsValid() || !purchasePtr.IsValid()){
        UE_LOG(LogEosStoreMobile, Error, TEXT("Identity and/or purchase interface invalid ....%hs") , __func__);
        return;
    }

    const FUniqueNetIdPtr localNetId = idPtr->GetUniquePlayerId(0);
    if(!localNetId.IsValid() || idPtr->GetLoginStatus(0) != ELoginStatus::Type::LoggedIn)
    {
        UE_LOG(LogEosStoreMobile, Error, TEXT("Failed , player logged out....%hs") , __func__);
        return;
    }

    purchasePtr->FinalizeReceiptValidationInfo(*localNetId , EntitlementId, FOnFinalizeReceiptValidationInfoComplete::   CreateWeakLambda(this,
               [this , localNetId](const FOnlineError& err , const FString& validationInfo){
          if(err.bSucceeded){
              UE_LOG(LogEosStoreMobile, Warning, TEXT("Redeem Entitlement success....%hs") , __func__);
          }else{
              UE_LOG(LogEosStoreMobile, Warning, TEXT("Redeem Entitlement failed , err: %s....%hs") , *err.ToLogString(),  __func__);
          }

    }  ));


}

void ATootEosPC::HandleQueryEntitlementCompleted(bool bWasSuccessful, const FUniqueNetId &UserId, const FString &Namespace, const FString &Error)
{
      OnQueryEntitlementComplete.Broadcast(false , FString::Printf( TEXT("%hs enter....") ,  __func__) );

    (void)Namespace;

    IOnlineSubsystem* onlineSys = Online::GetSubsystem(GetWorld());
    IOnlineEntitlementsPtr entitlPtr = onlineSys ? onlineSys->GetEntitlementsInterface() : nullptr;

    if(!entitlPtr.IsValid()){
        UE_LOG(LogEosStoreMobile, Error, TEXT("Entitlement interface invaled....%hs") , __func__);
        OnQueryEntitlementComplete.Broadcast(false,  Error);
        return;
    }

    entitlPtr->ClearOnQueryEntitlementsCompleteDelegate_Handle(QueryEntitlementDelegateHandle);
    QueryEntitlementDelegateHandle.Reset();

    if(!bWasSuccessful){
        UE_LOG(LogEosStoreMobile, Error, TEXT("Query Entitlements failed , err : %s....%hs") , *Error ,   __func__);
        OnQueryEntitlementComplete.Broadcast(bWasSuccessful,  Error);
        return;
    }

    UE_LOG(LogEosStoreMobile, Warning, TEXT("Query Entitlements Successful....  ...%hs") ,  __func__);

    TArray< TSharedRef<FOnlineEntitlement> > newEntitements;
    entitlPtr->GetAllEntitlements(UserId, TEXT(""), newEntitements);
    CachedEntitlements.Reset();
    CachedEntitlements.Reserve(newEntitements.Num());

    for (const auto& e : newEntitements) {

        UE_LOG(LogEosStoreMobile, Warning, TEXT("new entitlement, id: %s....%hs") , *e->Id , __func__);

        UStoreEntitlement* tmpEnt = NewObject<UStoreEntitlement>(this);
        tmpEnt->Id = e->Id;
        tmpEnt->Name = e->Name;
        tmpEnt->ItemId = e->ItemId;
        tmpEnt->bIsConsumable = e->bIsConsumable;
        tmpEnt->ConsumedCount = e->ConsumedCount;
        tmpEnt->EndDate = e->EndDate;

        CachedEntitlements.Add(tmpEnt);

    }


     OnQueryEntitlementComplete.Broadcast(bWasSuccessful,  Error);


}
