// Copyright (C) 2024 owoDra

#include "GenericMessageTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GenericMessageTypes)


//////////////////////////////////////////////////////////////////////
// FGenericVerbMessage

FString FGenericVerbMessage::ToString() const
{
	FString HumanReadableMessage;
	FGenericVerbMessage::StaticStruct()->ExportText(/*out*/ HumanReadableMessage, this, /*Defaults=*/ nullptr, /*OwnerObject=*/ nullptr, PPF_None, /*ExportRootScope=*/ nullptr);
	
	return HumanReadableMessage;
}
