// Copyright (C) 2024 owoDra

#include "GameplayTagStack.h"

#include "Message/GameplayMessageSubsystem.h"
#include "GameplayTag/GameplayTagStackMessageTypes.h"
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

		BroadcastTagStackChangeMessage(Tag, 0);
	}
}

void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		const auto& Stack{ Stacks[Index] };

		TagToCountMap.Add(Stack.Tag, Stack.StackCount);

		BroadcastTagStackChangeMessage(Stack.Tag, Stack.StackCount);
	}
}

void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const auto& Index : ChangedIndices)
	{
		const auto& Stack{ Stacks[Index] };

		TagToCountMap[Stack.Tag] = Stack.StackCount;

		BroadcastTagStackChangeMessage(Stack.Tag, Stack.StackCount);
	}
}


int32 FGameplayTagStackContainer::SetStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to SetStack"));

		return 0;
	}

	// Find tags

	for (auto It{ Stacks.CreateIterator() }; It; ++It)
	{
		auto& Stack{ *It };

		if (Stack.Tag == Tag)
		{
			// Set Tag count

			if (StackCount > 0)
			{
				Stack.StackCount = StackCount;
				TagToCountMap[Tag] = StackCount;

				BroadcastTagStackChangeMessage(Stack.Tag, StackCount);

				MarkItemDirty(Stack);
			}

			// Remove Tags

			else
			{
				It.RemoveCurrent();
				TagToCountMap.Remove(Tag);

				BroadcastTagStackChangeMessage(Stack.Tag, 0);

				MarkArrayDirty();
			}

			return StackCount;
		}
	}

	// Add Tags

	if (StackCount > 0)
	{
		auto& NewStack{ Stacks.Emplace_GetRef(Tag, StackCount) };

		BroadcastTagStackChangeMessage(Tag, StackCount);

		MarkItemDirty(NewStack);
		TagToCountMap.Add(Tag, StackCount);
	}

	return StackCount;
}

int32 FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to AddStack"));

		return 0;
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

				BroadcastTagStackChangeMessage(Stack.Tag, NewCount);

				MarkItemDirty(Stack);

				return NewCount;
			}
		}

		auto& NewStack{ Stacks.Emplace_GetRef(Tag, StackCount) };

		BroadcastTagStackChangeMessage(Tag, StackCount);

		MarkItemDirty(NewStack);
		TagToCountMap.Add(Tag, StackCount);

		return StackCount;
	}

	return GetStackCount(Tag);
}

int32 FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to RemoveStack"));

		return 0;
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

					BroadcastTagStackChangeMessage(Stack.Tag, 0);

					MarkArrayDirty();

					return 0;
				}
				else
				{
					const auto NewCount{ Stack.StackCount - StackCount };
					Stack.StackCount = NewCount;
					TagToCountMap[Tag] = NewCount;

					BroadcastTagStackChangeMessage(Stack.Tag, NewCount);

					MarkItemDirty(Stack);

					return NewCount;
				}
			}
		}
	}

	return GetStackCount(Tag);
}


void FGameplayTagStackContainer::BroadcastTagStackChangeMessage(FGameplayTag Tag, int32 CurrentStack)
{
	FGameplayTagStackCountChangeMessage Message;
	Message.OwningObject = OwnerObject;
	Message.Tag = Tag;
	Message.Count = CurrentStack;

	auto& MessageSystem{ UGameplayMessageSubsystem::Get(OwnerObject->GetWorld()) };
	MessageSystem.BroadcastMessage(TAG_Message_TagStackCountChange, Message);
}

#pragma endregion
