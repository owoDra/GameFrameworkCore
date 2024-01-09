// Copyright (C) 2024 owoDra

#pragma once

#include "UObject/Interface.h"

#include "GameplayTagStack.h"

#include "GameplayTagStackInterface.generated.h"


UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
 class GFCORE_API UGameplayTagStackInterface : public UInterface
 {
	 GENERATED_BODY()
 };
 
 class GFCORE_API IGameplayTagStackInterface : public IInterface
 {
	 GENERATED_BODY()

 protected:
	 virtual FGameplayTagStackContainer* GetStatTags() { return nullptr; }
	 virtual const FGameplayTagStackContainer* GetStatTagsConst() const { return nullptr; }

 public:
	 /**
	 * Set a Tag to Equipment that can be handled as statistics
	 * 
	 * Note:
	 *	Authority is required
	 */
	 UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Stat", meta = (GameplayTagFilter = "Stat"))
	 virtual int32 SetStatTagStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Add a Tag to Equipment that can be handled as statistics
	 *
	 * Tips:
	 *	if StackCount is less than or equal to 0, do nothing.
	 * 
	 * Note:
	 *	Authority is required
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Stat", meta = (GameplayTagFilter = "Stat"))
	virtual int32 AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Delete Tags that can be handled as statistics in Equipment
	 *
	 * Tips:
	 *	If StackCount is less than or equal to 0, do nothing.
	 * 
	 * Note:
	 *	Authority is required
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Stat", meta = (GameplayTagFilter = "Stat"))
	virtual int32 RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Returns the number of Tags that can be handled as statistics in Equipment.
	 *
	 * Tips:
	 *	Returns 0 if not present.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stat", meta = (GameplayTagFilter = "Stat"))
	virtual int32 GetStatTagStackCount(FGameplayTag Tag) const;

	/**
	 * Returns whether or not a Tag that can be handled as statistics exists in Equipment.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stat", meta = (GameplayTagFilter = "Stat"))
	virtual bool HasStatTag(FGameplayTag Tag) const;

 };

