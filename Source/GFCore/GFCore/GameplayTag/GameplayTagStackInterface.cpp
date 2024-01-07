// Copyright (C) 2023 owoDra

#include "GameplayTagStackInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStackInterface)


void IGameplayTagStackInterface::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (auto* Container{ GetStatTags() })
	{
		Container->AddStack(Tag, StackCount);
	}
}

void IGameplayTagStackInterface::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (auto* Container{ GetStatTags() })
	{
		Container->RemoveStack(Tag, StackCount);
	}
}

int32 IGameplayTagStackInterface::GetStatTagStackCount(FGameplayTag Tag) const
{
	if (auto* Container{ GetStatTagsConst() })
	{
		return Container->GetStackCount(Tag);
	}

	return 0;
}

bool IGameplayTagStackInterface::HasStatTag(FGameplayTag Tag) const
{
	if (auto* Container{ GetStatTagsConst() })
	{
		return Container->ContainsTag(Tag);
	}

	return false;
}
