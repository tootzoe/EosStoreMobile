// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreEntitlement.h"


UStoreEntitlement::UStoreEntitlement() :
    Id{} , Name{} , ItemId{},
    bIsConsumable(false),
    ConsumedCount(0),
    EndDate{}
{}

UStoreEntitlement::UStoreEntitlement(const FString &InId, const FString &InName, const FString &InItemId, const bool bInConsumable,
                                     const int32 InCousumedCount, const FString &InEndDate) :
    Id{InId} , Name{InName} , ItemId{InItemId},
    bIsConsumable(bInConsumable),
    ConsumedCount(InCousumedCount),
    EndDate{InEndDate}
{}



