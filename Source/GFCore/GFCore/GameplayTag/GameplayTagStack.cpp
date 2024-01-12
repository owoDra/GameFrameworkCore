// Copyright (C) 2024 owoDra

#include "GameplayTagStack.h"

#include "Message/GameplayMessageSubsystem.h"
#include "GameplayTag/GameplayTagStackMessageTypes.h"
#include "GFCoreLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStack)


//////////////////////////////////////////////////////////////////////
// FGameplayTagStack

#pragma region FGameplayTagStack

bool FGameplayTagStack::Match(const FGameplayTag OtherTag) const
{
	return Tag == OtherTag;
}

bool FGameplayTagStack::WillBeZero(int32 NumToRemove) const
{
	return StackCount <= NumToRemove;
}


int32 FGameplayTagStack::AddStackValue(int32 NumToAdd)
{
	return ChangeStackValue(StackCount + NumToAdd);
}

int32 FGameplayTagStack::ChangeStackValue(int32 NewCount)
{
	StackCount = (MaxStackCount != -1) ? FMath::Clamp(NewCount, 0, MaxStackCount) : FMath::Max(NewCount, 0);
	return StackCount;
}

int32 FGameplayTagStack::ChangeMaxStackValue(int32 NewCount)
{
	MaxStackCount = (NewCount <= 0) ? -1 : NewCount;

	return ChangeStackValue(StackCount);
}


FString FGameplayTagStack::GetDebugString() const
{
	return FString::Printf(TEXT("%s(%d/%d)"), *Tag.ToString(), StackCount, MaxStackCount);
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

		RefreshFastStacks(EStackChangeType::Remove, Tag);
	}
}

void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		const auto& Stack{ Stacks[Index] };

		RefreshFastStacks(EStackChangeType::AddOrChange, Stack.Tag, Stack);
	}
}

void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const auto& Index : ChangedIndices)
	{
		const auto& Stack{ Stacks[Index] };

		RefreshFastStacks(EStackChangeType::AddOrChange, Stack.Tag, Stack);
	}
}


void FGameplayTagStackContainer::RefreshFastStacks(EStackChangeType ChangeType, const FGameplayTag& Tag, FFastGameplayTagStack Stack)
{
	switch (ChangeType)
	{
	case FGameplayTagStackContainer::EStackChangeType::AddOrChange:
		FastStacks.Emplace(Tag, Stack);
		break;

	case FGameplayTagStackContainer::EStackChangeType::Remove:
		FastStacks.Remove(Tag);
		break;
	}
	
	BroadcastTagStackChangeMessage(Tag, Stack.StackCount, Stack.MaxStackCount);
}

void FGameplayTagStackContainer::BroadcastTagStackChangeMessage(FGameplayTag Tag, int32 CurrentStack, int32 MaxStack)
{
	FGameplayTagStackCountChangeMessage Message;
	Message.OwningObject = OwnerObject;
	Message.Tag = Tag;
	Message.Count = CurrentStack;
	Message.MaxCount = MaxStack;

	auto& MessageSystem{ UGameplayMessageSubsystem::Get(OwnerObject->GetWorld()) };
	MessageSystem.BroadcastMessage(TAG_Message_TagStackCountChange, Message);
}


void FGameplayTagStackContainer::SetMaxStack(FGameplayTag Tag, int32 MaxStackCount, bool bCanAddNewTag)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to AddMaxStack"));

		return;
	}

	// Find Stack, and set max stack count

	for (auto& Stack : Stacks)
	{
		if (Stack.Match(Tag))
		{
			Stack.ChangeMaxStackValue(MaxStackCount);

			RefreshFastStacks(EStackChangeType::AddOrChange, Tag, Stack);

			MarkItemDirty(Stack);

			return;
		}
	}

	// Add Stack

	if (bCanAddNewTag)
	{
		auto& NewStack{ Stacks.Emplace_GetRef(Tag, 0, MaxStackCount) };

		RefreshFastStacks(EStackChangeType::AddOrChange, Tag, NewStack);

		MarkItemDirty(NewStack);

		return;
	}
}

int32 FGameplayTagStackContainer::SetStack(FGameplayTag Tag, int32 StackCount, bool bCanAddNewTag, bool bRemoveTagAtZero)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to SetStack"));

		return 0;
	}

	// Find stack, and set count

	for (auto It{ Stacks.CreateIterator() }; It; ++It)
	{
		auto& Stack{ *It };

		if (Stack.Match(Tag))
		{
			// Set Tag count

			if (StackCount > 0)
			{
				const auto NewCount{ Stack.ChangeStackValue(StackCount) };

				RefreshFastStacks(EStackChangeType::AddOrChange, Tag, Stack);

				MarkItemDirty(Stack);

				return NewCount;
			}

			// Remove Tag

			else if (bRemoveTagAtZero)
			{
				It.RemoveCurrent();

				RefreshFastStacks(EStackChangeType::Remove, Tag);

				MarkArrayDirty();

				return 0;
			}

			// Remove tag count

			else
			{
				Stack.ChangeStackValue(StackCount);

				RefreshFastStacks(EStackChangeType::AddOrChange, Tag, Stack);

				MarkItemDirty(Stack);

				return 0;
			}
		}
	}

	// Add Tags

	if (bCanAddNewTag && !bRemoveTagAtZero)
	{
		auto& NewStack{ Stacks.Emplace_GetRef(Tag, StackCount, -1) };

		RefreshFastStacks(EStackChangeType::AddOrChange, Tag, NewStack);

		MarkItemDirty(NewStack);

		return StackCount;
	}

	return 0;
}

int32 FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount, bool bCanAddNewTag)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to AddStack"));

		return 0;
	}

	if (StackCount >= 0)
	{
		// Find Stack, and add stack count

		for (auto& Stack : Stacks)
		{
			if (Stack.Match(Tag))
			{
				const auto NewCount{ Stack.AddStackValue(StackCount) };

				RefreshFastStacks(EStackChangeType::AddOrChange, Tag, Stack);

				MarkItemDirty(Stack);

				return NewCount;
			}
		}

		// Add Stack

		if (bCanAddNewTag)
		{
			auto& NewStack{ Stacks.Emplace_GetRef(Tag, StackCount, -1) };

			RefreshFastStacks(EStackChangeType::AddOrChange, Tag, NewStack);

			MarkItemDirty(NewStack);

			return StackCount;
		}
	}

	return 0;
}

int32 FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount, bool bRemoveTagAtZero)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogGFC, Warning, TEXT("An invalid tag was passed to RemoveStack"));

		return 0;
	}

	if (StackCount > 0)
	{
		// Find stack, and remove count

		for (auto It{ Stacks.CreateIterator() }; It; ++It)
		{
			auto& Stack{ *It };

			if (Stack.Match(Tag))
			{
				// If 0 after application

				if (Stack.WillBeZero(StackCount))
				{
					// When tag can be removed

					if (bRemoveTagAtZero)
					{
						It.RemoveCurrent();
						
						RefreshFastStacks(EStackChangeType::Remove, Tag);

						MarkArrayDirty();
					}

					// When tag cannot be removed

					else
					{
						Stack.AddStackValue(-StackCount);

						RefreshFastStacks(EStackChangeType::AddOrChange, Tag, Stack);

						MarkItemDirty(Stack);
					}

					return 0;
				}

				// If it remains after application

				else
				{
					const auto NewCount{ Stack.AddStackValue(-StackCount) };
					
					RefreshFastStacks(EStackChangeType::AddOrChange, Tag, Stack);

					MarkItemDirty(Stack);

					return NewCount;
				}
			}
		}
	}

	return 0;
}

#pragma endregion
