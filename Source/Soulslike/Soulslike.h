// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editor.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSoulslike, Log, All);

#define ensureOrQuit(Condition) \
    if (!(Condition)) \
    { \
        ensureMsgf(false, TEXT("Critical logic failure in %s"), *FString(__FUNCTION__)); \
        GEditor->RequestEndPlayMap(); \
        return; \
    }

#define SLDEBUG(Format, ...) UE_LOG(LogTemp, Display, TEXT("[SL debug] " Format), ##__VA_ARGS__)