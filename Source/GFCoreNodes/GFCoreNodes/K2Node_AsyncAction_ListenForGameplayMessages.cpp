// Copyright (C) 2023 owoDra

#include "K2Node_AsyncAction_ListenForGameplayMessages.h"

#include "Action/AsyncAction_ListenForGameplayMessage.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_AsyncAction.h"
#include "K2Node_CallFunction.h"
#include "K2Node_TemporaryVariable.h"
#include "KismetCompiler.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_AsyncAction_ListenForGameplayMessages)


namespace ListenForGameplayMessagesHelper
{
	static FName ActualChannelPinName	= "ActualChannel";
	static FName PayloadPinName			= "Payload";
	static FName PayloadTypePinName		= "PayloadType";
	static FName DelegateProxyPinName	= "ProxyObject";
};


#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_AsyncAction_ListenForGameplayMessages::UK2Node_AsyncAction_ListenForGameplayMessages(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UK2Node_AsyncAction_ListenForGameplayMessages::PostReconstructNode()
{
	Super::PostReconstructNode();

	RefreshOutputPayloadType();
}

void UK2Node_AsyncAction_ListenForGameplayMessages::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	if (ChangedPin == GetPayloadTypePin())
	{
		if (ChangedPin->LinkedTo.Num() == 0)
		{
			RefreshOutputPayloadType();
		}
	}
}

void UK2Node_AsyncAction_ListenForGameplayMessages::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if (Pin.PinName == ListenForGameplayMessagesHelper::PayloadPinName)
	{
		HoverTextOut = HoverTextOut + LOCTEXT("PayloadOutTooltip", "\n\nThe message structure that we received").ToString();
	}
}


void UK2Node_AsyncAction_ListenForGameplayMessages::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static void SetNodeFunc(UEdGraphNode* NewNode, bool /*bIsTemplateNode*/, TWeakObjectPtr<UFunction> FunctionPtr)
		{
			auto* AsyncTaskNode{ CastChecked<UK2Node_AsyncAction_ListenForGameplayMessages>(NewNode) };

			if (FunctionPtr.IsValid())
			{
				auto* Func{ FunctionPtr.Get() };
				auto* ReturnProp{ CastFieldChecked<FObjectProperty>(Func->GetReturnProperty()) };
						
				AsyncTaskNode->ProxyFactoryFunctionName = Func->GetFName();
				AsyncTaskNode->ProxyFactoryClass        = Func->GetOuterUClass();
				AsyncTaskNode->ProxyClass               = ReturnProp->PropertyClass;
			}
		}
	};

	auto* NodeClass{ GetClass() };

	ActionRegistrar.RegisterClassFactoryActions<UAsyncAction_ListenForGameplayMessage>(
		FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateLambda(
			[NodeClass](const UFunction* FactoryFunc)->UBlueprintNodeSpawner*
			{
				auto* NodeSpawner{ UBlueprintFunctionNodeSpawner::Create(FactoryFunc) };
				check(NodeSpawner != nullptr);
				NodeSpawner->NodeClass = NodeClass;

				auto FunctionPtr{ MakeWeakObjectPtr(const_cast<UFunction*>(FactoryFunc)) };
				NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(GetMenuActions_Utils::SetNodeFunc, FunctionPtr);

				return NodeSpawner;
			}
		)
	);
}

void UK2Node_AsyncAction_ListenForGameplayMessages::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// The output of the UAsyncAction_ListenForGameplayMessage delegates is a proxy object which is used in the follow up call of GetPayload when triggered
	// This is only needed in the internals of this node so hide the pin from the editor.

	auto* DelegateProxyPin{ FindPin(ListenForGameplayMessagesHelper::DelegateProxyPinName) };

	if (ensure(DelegateProxyPin))
	{
		DelegateProxyPin->bHidden = true;
	}

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, ListenForGameplayMessagesHelper::PayloadPinName);
}


bool UK2Node_AsyncAction_ListenForGameplayMessages::HandleDelegates(const TArray<FBaseAsyncTaskHelper::FOutputPinAndLocalVariable>& VariableOutputs, UEdGraphPin* ProxyObjectPin, UEdGraphPin*& InOutLastThenPin, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext)
{
	auto bIsErrorFree{ true };

	if (VariableOutputs.Num() != 3)
	{
		ensureMsgf(false, TEXT("UK2Node_AsyncAction_ListenForGameplayMessages::HandleDelegates - Variable output array not valid. Output delegates must only have the single proxy object output and than must have pin for payload."));
		return false;
	}

	for (auto PropertyIt{ TFieldIterator<FMulticastDelegateProperty>(ProxyClass) }; PropertyIt && bIsErrorFree; ++PropertyIt)
	{
		UEdGraphPin* LastActivatedThenPin{ nullptr };

		bIsErrorFree &= FBaseAsyncTaskHelper::HandleDelegateImplementation(*PropertyIt, VariableOutputs, ProxyObjectPin, InOutLastThenPin, LastActivatedThenPin, this, SourceGraph, CompilerContext);
		bIsErrorFree &= HandlePayloadImplementation(*PropertyIt, VariableOutputs[0], VariableOutputs[2], VariableOutputs[1], LastActivatedThenPin, SourceGraph, CompilerContext);
	}

	return bIsErrorFree;
}

bool UK2Node_AsyncAction_ListenForGameplayMessages::HandlePayloadImplementation(FMulticastDelegateProperty* CurrentProperty, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& ProxyObjectVar, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& PayloadVar, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& ActualChannelVar, UEdGraphPin*& InOutLastActivatedThenPin, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext)
{
	auto bIsErrorFree{ true };
	const auto* PayloadPin{ GetPayloadPin() };
	const auto* Schema{ CompilerContext.GetSchema() };

	check(CurrentProperty && SourceGraph && Schema);

	const auto& PinType{ PayloadPin->PinType };

	if (PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
	{
		// If no payload type is specified and we're not trying to connect the output to anything ignore the rest of this step

		if (PayloadPin->LinkedTo.Num() == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	auto* TempVarOutput{ CompilerContext.SpawnInternalVariable(this, PinType.PinCategory, PinType.PinSubCategory, PinType.PinSubCategoryObject.Get(), PinType.ContainerType, PinType.PinValueType) };

	auto* const CallGetPayloadNode{ CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph) };
	CallGetPayloadNode->FunctionReference.SetExternalMember(TEXT("GetPayload"), CurrentProperty->GetOwnerClass());
	CallGetPayloadNode->AllocateDefaultPins();

	// Hook up the self connection

	auto* GetPayloadCallSelfPin{ Schema->FindSelfPin(*CallGetPayloadNode, EGPD_Input) };
	if (GetPayloadCallSelfPin)
	{
		bIsErrorFree &= Schema->TryCreateConnection(GetPayloadCallSelfPin, ProxyObjectVar.TempVar->GetVariablePin());

		// Hook the activate node up in the exec chain
		auto* GetPayloadExecPin{ CallGetPayloadNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute) };
		auto* GetPayloadThenPin{ CallGetPayloadNode->FindPinChecked(UEdGraphSchema_K2::PN_Then) };

		auto LastThenPin{ (UEdGraphPin*)nullptr };
		auto* GetPayloadPin{ CallGetPayloadNode->FindPinChecked(TEXT("OutPayload")) };
		bIsErrorFree &= Schema->TryCreateConnection(TempVarOutput->GetVariablePin(), GetPayloadPin);


		auto* AssignNode{ CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph) };
		AssignNode->AllocateDefaultPins();
		bIsErrorFree &= Schema->TryCreateConnection(GetPayloadThenPin, AssignNode->GetExecPin());
		bIsErrorFree &= Schema->TryCreateConnection(PayloadVar.TempVar->GetVariablePin(), AssignNode->GetVariablePin());
		AssignNode->NotifyPinConnectionListChanged(AssignNode->GetVariablePin());
		bIsErrorFree &= Schema->TryCreateConnection(AssignNode->GetValuePin(), TempVarOutput->GetVariablePin());
		AssignNode->NotifyPinConnectionListChanged(AssignNode->GetValuePin());


		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*InOutLastActivatedThenPin, *AssignNode->GetThenPin()).CanSafeConnect();
		bIsErrorFree &= Schema->TryCreateConnection(InOutLastActivatedThenPin, GetPayloadExecPin);

		// Hook up the actual channel connection

		auto* OutActualChannelPin{ GetOutputChannelPin() };
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*OutActualChannelPin, *ActualChannelVar.TempVar->GetVariablePin()).CanSafeConnect();
	}

	return bIsErrorFree;
}


void UK2Node_AsyncAction_ListenForGameplayMessages::RefreshOutputPayloadType()
{
	auto* PayloadPin{ GetPayloadPin() };
	auto* PayloadTypePin{ GetPayloadTypePin() };

	if (PayloadTypePin->DefaultObject != PayloadPin->PinType.PinSubCategoryObject)
	{
		if (PayloadPin->SubPins.Num() > 0)
		{
			GetSchema()->RecombinePin(PayloadPin);
		}

		PayloadPin->PinType.PinSubCategoryObject = PayloadTypePin->DefaultObject;
		PayloadPin->PinType.PinCategory = (PayloadTypePin->DefaultObject == nullptr) ? UEdGraphSchema_K2::PC_Wildcard : UEdGraphSchema_K2::PC_Struct;
	}
}


UEdGraphPin* UK2Node_AsyncAction_ListenForGameplayMessages::GetPayloadPin() const
{
	auto* Pin{ FindPinChecked(ListenForGameplayMessagesHelper::PayloadPinName) };
	check(Pin->Direction == EGPD_Output);

	return Pin;
}

UEdGraphPin* UK2Node_AsyncAction_ListenForGameplayMessages::GetPayloadTypePin() const
{
	auto* Pin{ FindPinChecked(ListenForGameplayMessagesHelper::PayloadTypePinName) };
	check(Pin->Direction == EGPD_Input);

	return Pin;
}

UEdGraphPin* UK2Node_AsyncAction_ListenForGameplayMessages::GetOutputChannelPin() const
{
	auto* Pin{ FindPinChecked(ListenForGameplayMessagesHelper::ActualChannelPinName) };
	check(Pin->Direction == EGPD_Output);

	return Pin;
}

#undef LOCTEXT_NAMESPACE

