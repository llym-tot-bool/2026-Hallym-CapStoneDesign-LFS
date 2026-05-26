// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogSoulslike, Log, All);

#if WITH_EDITOR
#define ensureOrQuit(Condition) \
    if (!(Condition)) \
    { \
        ensureAlwaysMsgf(false, TEXT("Critical logic failure in %s"), *FString(__FUNCTION__)); \
        if (GEditor) \
        { \
            GEditor->RequestEndPlayMap(); \
        } \
        return; \
    }
#else
#define ensureOrQuit(Condition) \
    if (!(Condition)) \
    { \
        ensureAlwaysMsgf(false, TEXT("Critical logic failure in %s"), *FString(__FUNCTION__)); \
        return; \
    }
#endif

#define SLDEBUG(Format, ...) UE_LOG(LogTemp, Display, TEXT("[SL debug] " Format), ##__VA_ARGS__)