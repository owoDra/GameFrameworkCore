// Copyright (C) 2024 owoDra

#include "GameplayMessageSubsystem.h"

#include "GFCoreLogs.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayMessageSubsystem)


//////////////////////////////////////////////////////////////////////
// FGameplayMessageListenerHandle

#pragma region FGameplayMessageListenerHandle

void FGameplayMessageListenerHandle::Unregister()
{
	if (auto* StrongSubsystem{ Subsystem.Get() })
	{
		StrongSubsystem->UnregisterListener(*this);
		Subsystem.Reset();
		Channel = FGameplayTag();
		ID = 0;
	}
}

#pragma endregion


//////////////////////////////////////////////////////////////////////
// UGameplayMessageSubsystem

#pragma region UGameplayMessageSubsystem

void UGameplayMessageSubsystem::Deinitialize()
{
	ListenerMap.Reset();

	Super::Deinitialize();
}


void UGameplayMessageSubsystem::UnregisterListener(FGameplayMessageListenerHandle Handle)
{
	if (Handle.IsValid())
	{
		check(Handle.Subsystem == this);

		UnregisterListenerInternal(Handle.Channel, Handle.ID);
	}
	else
	{
		UE_LOG(LogGameCore_Framework, Warning, TEXT("Trying to unregister an invalid Handle."));
	}
}

void UGameplayMessageSubsystem::UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID)
{
	if (auto* List{ ListenerMap.Find(Channel) })
	{
		auto MatchIndex
		{
			List->Listeners.IndexOfByPredicate(
				[ID = HandleID](const FGameplayMessageListenerData& Other)
				{
					return Other.HandleID == ID;
				}
			)
		};

		if (MatchIndex != INDEX_NONE)
		{
			List->Listeners.RemoveAtSwap(MatchIndex);
		}

		if (List->Listeners.Num() == 0)
		{
			ListenerMap.Remove(Channel);
		}
	}
}

FGameplayMessageListenerHandle UGameplayMessageSubsystem::RegisterListenerInternal(FGameplayTag Channel, TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback, const UScriptStruct* StructType, EGameplayMessageMatch MatchType)
{
	auto& List{ ListenerMap.FindOrAdd(Channel) };

	auto& Entry{ List.Listeners.AddDefaulted_GetRef() };
	Entry.ReceivedCallback = MoveTemp(Callback);
	Entry.ListenerStructType = StructType;
	Entry.bHadValidType = StructType != nullptr;
	Entry.HandleID = ++List.HandleID;
	Entry.MatchType = MatchType;

	return FGameplayMessageListenerHandle(this, Channel, Entry.HandleID);
}


void UGameplayMessageSubsystem::K2_BroadcastMessage(FGameplayTag Channel, const int32& Message)
{
	checkNoEntry();
}

DEFINE_FUNCTION(UGameplayMessageSubsystem::execK2_BroadcastMessage)
{
	P_GET_STRUCT(FGameplayTag, Channel);

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);

	auto* MessagePtr{ Stack.MostRecentPropertyAddress };
	auto* StructProp{ CastField<FStructProperty>(Stack.MostRecentProperty) };

	P_FINISH;

	if (ensure((StructProp != nullptr) && (StructProp->Struct != nullptr) && (MessagePtr != nullptr)))
	{
		P_THIS->BroadcastMessageInternal(Channel, StructProp->Struct, MessagePtr);
	}
}

void UGameplayMessageSubsystem::BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes)
{
	// Broadcast the message

	auto bOnInitialTag{ true };

	for (auto Tag{ Channel }; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const auto* List{ ListenerMap.Find(Tag) })
		{
			// Copy in case there are removals while handling callbacks

			auto ListenerArray{ TArray<FGameplayMessageListenerData>(List->Listeners) };

			for (const auto& Listener : ListenerArray)
			{
				if (bOnInitialTag || (Listener.MatchType == EGameplayMessageMatch::PartialMatch))
				{
					if (Listener.bHadValidType && !Listener.ListenerStructType.IsValid())
					{
						UE_LOG(LogGameCore_Framework, Warning, TEXT("Listener struct type has gone invalid on Channel %s. Removing listener from list"), *Channel.ToString());
						
						UnregisterListenerInternal(Channel, Listener.HandleID);

						continue;
					}

					// The receiving type must be either a parent of the sending type or completely ambiguous (for internal use)

					if (!Listener.bHadValidType || StructType->IsChildOf(Listener.ListenerStructType.Get()))
					{
						Listener.ReceivedCallback(Channel, StructType, MessageBytes);
					}
					else
					{
						UE_LOG(LogGameCore_Framework, Error, TEXT("Struct type mismatch on channel %s (broadcast type %s, listener at %s was expecting type %s)"),
							*Channel.ToString(),
							*StructType->GetPathName(),
							*Tag.ToString(),
							*Listener.ListenerStructType->GetPathName());
					}
				}
			}
		}

		bOnInitialTag = false;
	}
}


UGameplayMessageSubsystem& UGameplayMessageSubsystem::Get(const UObject* WorldContextObject)
{
	auto* World{ GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert) };
	check(World);

	auto* Router{ UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(World->GetGameInstance()) };
	check(Router);

	return *Router;
}

bool UGameplayMessageSubsystem::HasInstance(const UObject* WorldContextObject)
{
	auto* World{ GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert) };
	auto* Router{ (World != nullptr) ? UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(World->GetGameInstance()) : nullptr };

	return (Router != nullptr);
}

#pragma endregion
