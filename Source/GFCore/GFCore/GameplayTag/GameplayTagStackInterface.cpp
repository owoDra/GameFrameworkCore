// Copyright (C) 2023 owoDra

#include "GameplayTagStackInterface.h"

#include "GFCoreLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStackInterface)


int32 IGameplayTagStackInterface::SetStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (auto* Container{ GetStatTags() })
	{
		return Container->SetStack(Tag, StackCount);
	}
	else
	{
		UE_LOG(LogGFC, Error, TEXT("IGameplayTagStackInterface::SetStatTagStack: StatTag is invalid, GetStatTags() not overridden or Container is invalid."));
	}

	return 0;
}

int32 IGameplayTagStackInterface::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (auto* Container{ GetStatTags() })
	{
		return Container->AddStack(Tag, StackCount);
	}
	else
	{
		UE_LOG(LogGFC, Error, TEXT("IGameplayTagStackInterface::AddStatTagStack: StatTag is invalid, GetStatTags() not overridden or Container is invalid."));
	}

	return 0;
}

int32 IGameplayTagStackInterface::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (auto* Container{ GetStatTags() })
	{
		return Container->RemoveStack(Tag, StackCount);
	}
	else
	{
		UE_LOG(LogGFC, Error, TEXT("IGameplayTagStackInterface::RemoveStatTagStack: StatTag is invalid, GetStatTags() not overridden or Container is invalid."));
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
		UE_LOG(LogGFC, Error, TEXT("IGameplayTagStackInterface::GetStatTagStackCount: StatTag is invalid, GetStatTagsConst() not overridden or Container is invalid."));
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
		UE_LOG(LogGFC, Error, TEXT("IGameplayTagStackInterface::HasStatTag: StatTag is invalid, GetStatTagsConst() not overridden or Container is invalid."));
	}

	return false;
}
