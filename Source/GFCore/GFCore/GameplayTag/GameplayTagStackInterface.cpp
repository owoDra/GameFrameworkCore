// Copyright (C) 2024 owoDra

#include "GameplayTagStackInterface.h"

#include "GFCoreLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStackInterface)


void IGameplayTagStackInterface::SetMaxStatTagStack(FGameplayTag Tag, int32 StackCount, bool bCanAddNewTag)
{
	if (auto* Container{ GetStatTags() })
	{
		Container->SetMaxStack(Tag, StackCount, bCanAddNewTag);
	}
	else
	{
		UE_LOG(LogGameCore_Framework, Error, TEXT("IGameplayTagStackInterface::SetMaxStatTagStack: StatTag is invalid, GetStatTags() not overridden or Container is invalid."));
	}
}

int32 IGameplayTagStackInterface::SetStatTagStack(FGameplayTag Tag, int32 StackCount, bool bCanAddNewTag, bool bRemoveTagAtZero)
{
	if (auto* Container{ GetStatTags() })
	{
		return Container->SetStack(Tag, StackCount, bCanAddNewTag, bRemoveTagAtZero);
	}
	else
	{
		UE_LOG(LogGameCore_Framework, Error, TEXT("IGameplayTagStackInterface::SetStatTagStack: StatTag is invalid, GetStatTags() not overridden or Container is invalid."));
	}

	return 0;
}

int32 IGameplayTagStackInterface::AddStatTagStack(FGameplayTag Tag, int32 StackCount, bool bCanAddNewTag)
{
	if (auto* Container{ GetStatTags() })
	{
		return Container->AddStack(Tag, StackCount, bCanAddNewTag);
	}
	else
	{
		UE_LOG(LogGameCore_Framework, Error, TEXT("IGameplayTagStackInterface::AddStatTagStack: StatTag is invalid, GetStatTags() not overridden or Container is invalid."));
	}

	return 0;
}

int32 IGameplayTagStackInterface::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount, bool bRemoveTagAtZero)
{
	if (auto* Container{ GetStatTags() })
	{
		return Container->RemoveStack(Tag, StackCount, bRemoveTagAtZero);
	}
	else
	{
		UE_LOG(LogGameCore_Framework, Error, TEXT("IGameplayTagStackInterface::RemoveStatTagStack: StatTag is invalid, GetStatTags() not overridden or Container is invalid."));
	}

	return 0;
}


int32 IGameplayTagStackInterface::GetStatTagStackCount(FGameplayTag Tag) const
{
	if (auto* Container{ GetStatTagsConst() })
	{
		return Container->GetStackCount(Tag);
	}
	else
	{
		UE_LOG(LogGameCore_Framework, Error, TEXT("IGameplayTagStackInterface::GetStatTagStackCount: StatTag is invalid, GetStatTagsConst() not overridden or Container is invalid."));
	}

	return 0;
}

int32 IGameplayTagStackInterface::GetMaxStatTagStackCount(FGameplayTag Tag) const
{
	if (auto* Container{ GetStatTagsConst() })
	{
		return Container->GetMaxStackCount(Tag);
	}
	else
	{
		UE_LOG(LogGameCore_Framework, Error, TEXT("IGameplayTagStackInterface::GetMaxStatTagStackCount: StatTag is invalid, GetStatTagsConst() not overridden or Container is invalid."));
	}

	return 0;
}

bool IGameplayTagStackInterface::HasStatTag(FGameplayTag Tag) const
{
	if (auto* Container{ GetStatTagsConst() })
	{
		return Container->ContainsTag(Tag);
	}
	else
	{
		UE_LOG(LogGameCore_Framework, Error, TEXT("IGameplayTagStackInterface::HasStatTag: StatTag is invalid, GetStatTagsConst() not overridden or Container is invalid."));
	}

	return false;
}
