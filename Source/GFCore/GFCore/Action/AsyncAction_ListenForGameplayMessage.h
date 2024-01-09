// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/CancellableAsyncAction.h"

#include "Message/GameplayMessageSubsystem.h"
#include "Message/GameplayMessageTypes.h"

#include "AsyncAction_ListenForGameplayMessage.generated.h"

class UScriptStruct;


/**
 * Proxy object pin will be hidden in K2Node_GameplayMessageAsyncAction. Is used to get a reference to the object triggering the delegate for the follow up call of 'GetPayload'.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncGameplayMessageDelegate, UAsyncAction_ListenForGameplayMessage*, ProxyObject, FGameplayTag, ActualChannel);


/**
 * Async action for listen for gameplay message.
 */
UCLASS(BlueprintType, meta=(HasDedicatedAsyncNode))
class GFCORE_API UAsyncAction_ListenForGameplayMessage : public UCancellableAsyncAction
{
	GENERATED_BODY()
public:
	UAsyncAction_ListenForGameplayMessage() {}

public:
	//
	// Called when a message is broadcast on the specified channel. Use GetPayload() to request the message payload.
	//
	UPROPERTY(BlueprintAssignable)
	FAsyncGameplayMessageDelegate OnMessageReceived;

protected:
	const void* ReceivedMessagePayloadPtr{ nullptr };

	TWeakObjectPtr<UWorld> WorldPtr;
	FGameplayTag ChannelToRegister;
	TWeakObjectPtr<UScriptStruct> MessageStructType{ nullptr };
	EGameplayMessageMatch MessageMatchType{ EGameplayMessageMatch::ExactMatch };

	FGameplayMessageListenerHandle ListenerHandle;

public:
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	/**
	 * Asynchronously waits for a gameplay message to be broadcast on the specified channel.
	 *
	 * @param Channel			The message channel to listen for
	 * @param PayloadType		The kind of message structure to use (this must match the same type that the sender is broadcasting)
	 * @param MatchType			The rule used for matching the channel with broadcasted messages
	 */
	UFUNCTION(BlueprintCallable, Category = Messaging, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true", GameplayTagFilter = "Message"))
	static UAsyncAction_ListenForGameplayMessage* ListenForGameplayMessages(UObject* WorldContextObject, FGameplayTag Channel, UScriptStruct* PayloadType, EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch);

	/**
	 * Attempt to copy the payload received from the broadcasted gameplay message into the specified wildcard.
	 * The wildcard's type must match the type from the received message.
	 *
	 * @param OutPayload	The wildcard reference the payload should be copied into
	 * @return				If the copy was a success
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Messaging", meta = (CustomStructureParam = "OutPayload"))
	bool GetPayload(UPARAM(ref) int32& OutPayload);

	DECLARE_FUNCTION(execGetPayload);

private:
	void HandleMessageReceived(FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload);

};
