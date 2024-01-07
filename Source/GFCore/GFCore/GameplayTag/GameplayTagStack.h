// Copyright (C) 2023 owoDra

#pragma once

#include "Net/Serialization/FastArraySerializer.h"

#include "GameplayTagContainer.h"

#include "GameplayTagStack.generated.h"


/**
 * Represents one stack of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct GFCORE_API FGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend struct FGameplayTagStackContainer;

public:
	FGameplayTagStack(){}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{
	}

private:
	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount{ 0 };

public:
	FString GetDebugString() const;

};


/**
 * Container of gameplay tag stacks 
 */
USTRUCT(BlueprintType)
struct GFCORE_API FGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()
public:
	FGameplayTagStackContainer() 
		: OwnerObject(nullptr)
	{}

	FGameplayTagStackContainer(UObject* InOwnerObject)
		: OwnerObject(InOwnerObject)
	{}

private:
	//
	// Replicated list of gameplay tag stacks
	//
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	//
	// Accelerated list of tag stacks for queries
	//
	UPROPERTY(NotReplicated)
	TMap<FGameplayTag, int32> TagToCountMap;

	//
	// Owner of this container.
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<UObject> OwnerObject;

public:
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

public:
	/**
	 * Set a specified number of stacks to the tag
	 */
	int32 SetStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	 */
	int32 AddStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	 */
	int32 RemoveStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Returns the stack count of the specified tag (or 0 if the tag is not present)
	 */
	int32 GetStackCount(FGameplayTag Tag) const { return TagToCountMap.FindRef(Tag); }

	/**
	 * Returns true if there is at least one stack of the specified tag
	 */
	bool ContainsTag(FGameplayTag Tag) const { return TagToCountMap.Contains(Tag); }


protected:
	void BroadcastTagStackChangeMessage(FGameplayTag Tag = FGameplayTag::EmptyTag, int32 CurrentStack = 0);

};

template<>
struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
