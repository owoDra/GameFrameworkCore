// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "GameplayTagStackMessageTypes.generated.h"


GFCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Message_TagStackCountChange);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Stat);


/**
 * Data of the message that informs the user that the TagStack count has changed
 */
USTRUCT(BlueprintType)
struct GFCORE_API FGameplayTagStackCountChangeMessage
{
	GENERATED_BODY()
public:
	FGameplayTagStackCountChangeMessage() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UObject> OwningObject{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Count{ 0 };

};
