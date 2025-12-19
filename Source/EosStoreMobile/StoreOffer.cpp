// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreOffer.h"





UStoreOffer::UStoreOffer() :
    OfferId{} ,Title{} , Price{} ,
    bConsumable(true) , Quantity(0)
{
}

UStoreOffer::UStoreOffer(const FString &InOfferId, const FText &InTitle, const FText &InPrice, bool bInConsumable, int32 InQuantity)
 :     OfferId{InOfferId} ,Title{InTitle} , Price{InPrice} ,
   bConsumable(bInConsumable) , Quantity(InQuantity)
{

}
