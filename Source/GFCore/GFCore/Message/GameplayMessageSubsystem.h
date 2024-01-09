﻿// Copyright (C) 2024 owoDra

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "Message/GameplayMessageTypes.h"

#include "GameplayTagContainer.h"

#include "GameplayMessageSubsystem.generated.h"


/**
 * An opaque handle that can be used to remove a previously registered message listener
 * 
 * Tips:
 *	@see UGameplayMessageSubsystem::RegisterListener and UGameplayMessageSubsystem::UnregisterListener
 */
USTRUCT(BlueprintType)
struct GFCORE_API FGameplayMessageListenerHandle
{
	GENERATED_BODY()

	friend class UGameplayMessageSubsystem;

public:
	FGameplayMessageListenerHandle() {}

	FGameplayMessageListenerHandle(UGameplayMessageSubsystem* InSubsystem, FGameplayTag InChannel, int32 InID) 
		: Subsystem(InSubsystem)
		, Channel(InChannel)
		, ID(InID) 
	{}

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<UGameplayMessageSubsystem> Subsystem;

	UPROPERTY(Transient)
	FGameplayTag Channel;

	UPROPERTY(Transient)
	int32 ID{ 0 };

	FDelegateHandle StateClearedHandle;

public:
	void Unregister();

	bool IsValid() const { return ID != 0; }

};


/** 
 * Entry information for a single registered listener
 */
USTRUCT()
struct FGameplayMessageListenerData
{
	GENERATED_BODY()
public:
	FGameplayMessageListenerData() {}

public:
	//
	// Callback for when a message has been received
	//
	TFunction<void(FGameplayTag, const UScriptStruct*, const void*)> ReceivedCallback;

	//
	// Adding some logging and extra variables around some potential problems with this
	//
	TWeakObjectPtr<const UScriptStruct> ListenerStructType{ nullptr };

	bool bHadValidType{ false };

	int32 HandleID;
	EGameplayMessageMatch MatchType;

};


/**
 * This system allows event raisers and listeners to register for messages without
 * having to know about each other directly, though they must agree on the format
 * of the message (as a USTRUCT() type).
 *
 *
 * You can get to the message router from the game instance:
 *    UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(GameInstance)
 * or directly from anything that has a route to a world:
 *    UGameplayMessageSubsystem::Get(WorldContextObject)
 *
 * Note that call order when there are multiple listeners for the same channel is
 * not guaranteed and can change over time!
 */
UCLASS()
class GFCORE_API UGameplayMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class UAsyncAction_ListenForGameplayMessage;

public:
	UGameplayMessageSubsystem() {}

public:
	virtual void Deinitialize() override;


protected:
	/**
	 * List of all entries for a given channel
	 */
	struct FChannelListenerList
	{
		TArray<FGameplayMessageListenerData> Listeners;
		int32 HandleID = 0;
	};

protected:
	//
	// Listen data map for messages related to GameplayTag
	//
	TMap<FGameplayTag, FChannelListenerList> ListenerMap;

public:
	/**
	 * Broadcast a message on the specified channel
	 *
	 * @param Channel			The message channel to broadcast on
	 * @param Message			The message to send (must be the same type of UScriptStruct expected by the listeners for this channel, otherwise an error will be logged)
	 */
	template <typename FMessageStructType>
	void BroadcastMessage(FGameplayTag Channel, const FMessageStructType& Message)
	{
		const auto* StructType{ TBaseStructure<FMessageStructType>::Get() };
		BroadcastMessageInternal(Channel, StructType, &Message);
	}

	/**
	 * Register to receive messages on a specified channel
	 *
	 * @param Channel			The message channel to listen to
	 * @param Callback			Function to call with the message when someone broadcasts it (must be the same type of UScriptStruct provided by broadcasters for this channel, otherwise an error will be logged)
	 *
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	template <typename FMessageStructType>
	FGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, TFunction<void(FGameplayTag, const FMessageStructType&)>&& Callback, EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch)
	{
		auto ThunkCallback
		{
			[InnerCallback = MoveTemp(Callback)](FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)
			{
				InnerCallback(ActualTag, *reinterpret_cast<const FMessageStructType*>(SenderPayload));
			}
		};

		const auto* StructType{ TBaseStructure<FMessageStructType>::Get() };

		return RegisterListenerInternal(Channel, ThunkCallback, StructType, MatchType);
	}

	/**
	 * Register to receive messages on a specified channel and handle it with a specified member function
	 * Executes a weak object validity check to ensure the object registering the function still exists before triggering the callback
	 *
	 * @param Channel			The message channel to listen to
	 * @param Object			The object instance to call the function on
	 * @param Function			Member function to call with the message when someone broadcasts it (must be the same type of UScriptStruct provided by broadcasters for this channel, otherwise an error will be logged)
	 *
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	template <typename FMessageStructType, typename TOwner = UObject>
	FGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, TOwner* Object, void(TOwner::* Function)(FGameplayTag, const FMessageStructType&))
	{
		auto WeakObject{ TWeakObjectPtr<TOwner>(Object) };
		auto Handle
		{
			RegisterListener<FMessageStructType>(
				Channel,[WeakObject, Function](FGameplayTag Channel, const FMessageStructType& Payload)
				{
					if (auto* StrongObject{WeakObject.Get()})
					{
						(StrongObject->*Function)(Channel, Payload);
					}
				}
			)
		};

		return Handle;
	}

	/**
	 * Register to receive messages on a specified channel with extra parameters to support advanced behavior
	 * The stateful part of this logic should probably be separated out to a separate system
	 *
	 * @param Channel			The message channel to listen to
	 * @param Params			Structure containing details for advanced behavior
	 *
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	template <typename FMessageStructType>
	FGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, FGameplayMessageListenerParams<FMessageStructType>& Params)
	{
		FGameplayMessageListenerHandle Handle;

		// Register to receive any future messages broadcast on this channel

		if (Params.OnMessageReceivedCallback)
		{
			auto ThunkCallback
			{
				[InnerCallback = Params.OnMessageReceivedCallback](FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)
				{
					InnerCallback(ActualTag, *reinterpret_cast<const FMessageStructType*>(SenderPayload));
				}
			};

			const auto* StructType{ TBaseStructure<FMessageStructType>::Get() };
			Handle = RegisterListenerInternal(Channel, ThunkCallback, StructType, Params.MatchType);
		}

		return Handle;
	}

	/**
	 * Remove a message listener previously registered by RegisterListener
	 *
	 * @param Handle	The handle returned by RegisterListener
	 */
	void UnregisterListener(FGameplayMessageListenerHandle Handle);

private:
	/**
	 * Internal helper for unregistering a message listener
	 */
	void UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID);

	/**
	 * Internal helper for registering a message listener
	 */
	FGameplayMessageListenerHandle RegisterListenerInternal(
		FGameplayTag Channel,
		TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback,
		const UScriptStruct* StructType,
		EGameplayMessageMatch MatchType);

	
protected:
	/**
	 * Broadcast a message on the specified channel
	 *
	 * @param Channel			The message channel to broadcast on
	 * @param Message			The message to send (must be the same type of UScriptStruct expected by the listeners for this channel, otherwise an error will be logged)
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category=Messaging, meta=(CustomStructureParam="Message", AllowAbstract="false", DisplayName="Broadcast Message", GameplayTagFilter = "Message"))
	void K2_BroadcastMessage(FGameplayTag Channel, const int32& Message);

	DECLARE_FUNCTION(execK2_BroadcastMessage);

private:
	/**
	 * Internal helper for broadcasting a message
	 */
	void BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes);

	
public:
	/**
		* @return the message router for the game instance associated with the world of the specified object
		*/
	static UGameplayMessageSubsystem& Get(const UObject* WorldContextObject);

	/**
		* @return true if a valid GameplayMessageRouter subsystem if active in the provided world
		*/
	static bool HasInstance(const UObject* WorldContextObject);

};
