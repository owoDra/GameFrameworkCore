// Copyright (C) 2024 owoDra

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
	friend struct FFastGameplayTagStack;

public:
	FGameplayTagStack(){}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount, int32 InMaxStackCount = -1)
		: Tag(InTag)
		, StackCount(InStackCount)
	{
		ChangeMaxStackValue(InMaxStackCount);
	}

private:
	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount{ 0 };

	UPROPERTY()
	int32 MaxStackCount{ -1 };

private:
	/**
	 * Returns whether the tag is correct or not.
	 */
	bool Match(const FGameplayTag OtherTag) const;

	/**
	 * Returns whether the tag is correct or not.
	 */
	bool WillBeZero(int32 NumToRemove) const;

private:
	/**
	 * Add the specified number to Stack
	 * 
	 * Tips:
	 *	If a negative value is entered, it is subtracted.
	 */
	int32 AddStackValue(int32 NumToAdd);

	/**
	 * Set the stack count
	 * 
	 * Tips:
	 *	If MaxStacks is set, it is limited to that range
	 */
	int32 ChangeStackValue(int32 NewCount);

	/**
	 * MaxStacks sets and limits the current count
	 */
	int32 ChangeMaxStackValue(int32 NewCount);

public:
	FString GetDebugString() const;

};


/**
 * List entry data for quick query of Stat tags
 */
USTRUCT()
struct FFastGameplayTagStack
{
	GENERATED_BODY()
public:
	FFastGameplayTagStack() {}

	FFastGameplayTagStack(int32 InStackCount, int32 InMaxStackCount)
		: StackCount(InStackCount)
		, MaxStackCount(InMaxStackCount)
	{
	}

	FFastGameplayTagStack(const FGameplayTagStack& OriginalStack)
		: StackCount(OriginalStack.StackCount)
		, MaxStackCount(OriginalStack.MaxStackCount)
	{
	}

public:
	UPROPERTY()
	int32 StackCount{ 0 };

	UPROPERTY()
	int32 MaxStackCount{ -1 };

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

	//////////////////////////////////////////////////////////
	// Container parameters
public:
	//
	// Replicated list of gameplay tag stacks
	//
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	//
	// Accelerated list of tag stacks for queries
	//
	UPROPERTY(NotReplicated)
	TMap<FGameplayTag, FFastGameplayTagStack> FastStacks;

	//
	// Owner of this container.
	//
	UPROPERTY(NotReplicated)
	TObjectPtr<UObject> OwnerObject;


	///////////////////////////////////////////////////////////
	// Replication
public:
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}


	///////////////////////////////////////////////////////////
	// Change and Notify
protected:
	/**
	 * What changes are made when the TagStack is modified
	 */
	enum class EStackChangeType
	{
		// New StatTag added or count changed
		AddOrChange,

		// StatTag is completely removed.
		Remove
	};

protected:
	/**
	 * Runs when TagStack is changed, updates FastStacks, and broadcasts messages
	 */
	void RefreshFastStacks(EStackChangeType ChangeType, const FGameplayTag& Tag, FFastGameplayTagStack Stack = FFastGameplayTagStack());

	/**
	 * Broadcast messages that the TagStack has changed through the GameplayMessageSubsystem
	 */
	void BroadcastTagStackChangeMessage(FGameplayTag Tag = FGameplayTag::EmptyTag, int32 CurrentStack = 0, int32 MaxStack = -1);


public:
	/**
	 * Set max number of stacks to the tag
	 */
	void SetMaxStack(FGameplayTag Tag, int32 MaxStackCount, bool bCanAddNewTag = true);

	/**
	 * Set a specified number of stacks to the tag
	 */
	int32 SetStack(FGameplayTag Tag, int32 StackCount, bool bCanAddNewTag = true, bool bRemoveTagAtZero = false);

	/**
	 * Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	 */
	int32 AddStack(FGameplayTag Tag, int32 StackCount, bool bCanAddNewTag = true);

	/**
	 * Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	 */
	int32 RemoveStack(FGameplayTag Tag, int32 StackCount, bool bRemoveTagAtZero = false);


public:
	/**
	 * Returns the stack count of the specified tag (or 0 if the tag is not present)
	 */
	int32 GetStackCount(FGameplayTag Tag) const { return FastStacks.FindRef(Tag).StackCount; }

	/**
	 * Returns the max stack count of the specified tag (or -1 if the tag is not present)
	 */
	int32 GetMaxStackCount(FGameplayTag Tag) const { return FastStacks.FindRef(Tag).MaxStackCount; }

	/**
	 * Returns true if there is at least one stack of the specified tag
	 */
	bool ContainsTag(FGameplayTag Tag) const { return FastStacks.Contains(Tag); }

};

template<>
struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
