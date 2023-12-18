// Copyright (C) 2023 owoDra

#include "GameplayTagStack.h"

#include "GFCoreLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStack)


//////////////////////////////////////////////////////////////////////
// FGameplayTagStack

#pragma region FGameplayTagStack

FString FGameplayTagStack::GetDebugString() const
{
	return FString::Printf(TEXT("%sx%d"), *Tag.ToString(), StackCount);
}

#pragma endregion


//////////////////////////////////////////////////////////////////////
// FGameplayTagStackContainer

#pragma region FGameplayTagStackContainer

void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const auto& Index : RemovedIndices)
	{
		const auto Tag{ Stacks[Index].Tag };

		TagToCountMap.Remove(Tag);
	}
}

void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		const auto& Stack{ Stacks[Index] };

		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const auto& Index : ChangedIndices)
	{
		const auto& Stack{ Stacks[Index] };

		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}


void FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to AddStack"));

		return;
	}

	if (StackCount > 0)
	{
		for (auto& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const auto NewCount{ Stack.StackCount + StackCount };
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				MarkItemDirty(Stack);
				return;
			}
		}

		auto& NewStack{ Stacks.Emplace_GetRef(Tag, StackCount) };
		MarkItemDirty(NewStack);
		TagToCountMap.Add(Tag, StackCount);
	}
}

void FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to RemoveStack"));

		return;
	}

	if (StackCount > 0)
	{
		for (auto It{ Stacks.CreateIterator() }; It; ++It)
		{
			auto& Stack{ *It };
			if (Stack.Tag == Tag)
			{
				if (Stack.StackCount <= StackCount)
				{
					It.RemoveCurrent();
					TagToCountMap.Remove(Tag);
					MarkArrayDirty();
				}
				else
				{
					const auto NewCount{ Stack.StackCount - StackCount };
					Stack.StackCount = NewCount;
					TagToCountMap[Tag] = NewCount;
					MarkItemDirty(Stack);
				}
				return;
			}
		}
	}
}

#pragma endregion
