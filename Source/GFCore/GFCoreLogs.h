// Copyright (C) 2024 owoDra

#pragma once

#include "Logging/LogMacros.h"

GFCORE_API DECLARE_LOG_CATEGORY_EXTERN(LogGFC, Log, All);

#if !UE_BUILD_SHIPPING

#define GFCLOG(FormattedText, ...) UE_LOG(LogGFC, Log, FormattedText, __VA_ARGS__)

#define GFCENSURE(InExpression) ensure(InExpression)
#define GFCENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GFCENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GFCCHECK(InExpression) check(InExpression)
#define GFCCHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GFCLOG(FormattedText, ...)

#define GFCENSURE(InExpression) InExpression
#define GFCENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GFCENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GFCCHECK(InExpression) InExpression
#define GFCCHECK_MSG(InExpression, InFormat, ...) InExpression

#endif