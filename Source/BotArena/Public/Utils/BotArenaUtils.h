// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LogBotArena.h"

/**
 * Utility functions for the BotArena project
 */
class BOTARENA_API FBotArenaUtils
{
public:
    /**
     * Safely gets a component of the specified type from an actor
     * @param Actor - The actor to get the component from
     * @param ComponentName - Optional name for logging purposes
     * @return The component if found, nullptr otherwise
     */
    template<class T>
    static T* GetComponentSafe(AActor* Actor, const FString& ComponentName = TEXT("Component"))
    {
        if (!IsValid(Actor))
        {
            UE_LOG(LogBotArena, Warning, TEXT("GetComponentSafe: Invalid actor for %s"), *ComponentName);
            return nullptr;
        }
        
        T* Component = Actor->FindComponentByClass<T>();
        if (!Component)
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s: Missing %s"), *Actor->GetName(), *ComponentName);
        }
        
        return Component;
    }
    
    /**
     * Validates that a value is within a specified range
     * @param Value - The value to check
     * @param MinValue - The minimum allowed value
     * @param MaxValue - The maximum allowed value
     * @param ValueName - Name of the value for logging purposes
     * @return True if the value is within range, false otherwise
     */
    template<typename T>
    static bool ValidateRange(T Value, T MinValue, T MaxValue, const FString& ValueName)
    {
        if (Value < MinValue || Value > MaxValue)
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s value %f is outside valid range [%f, %f]"),
                *ValueName, (float)Value, (float)MinValue, (float)MaxValue);
            return false;
        }
        return true;
    }
    
    /**
     * Clamps a value to a specified range and logs a warning if outside range
     * @param Value - The value to clamp
     * @param MinValue - The minimum allowed value
     * @param MaxValue - The maximum allowed value
     * @param ValueName - Name of the value for logging purposes
     * @return The clamped value
     */
    template<typename T>
    static T ClampWithWarning(T Value, T MinValue, T MaxValue, const FString& ValueName)
    {
        if (Value < MinValue)
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s value %f is below minimum %f, clamping"),
                *ValueName, (float)Value, (float)MinValue);
            return MinValue;
        }
        else if (Value > MaxValue)
        {
            UE_LOG(LogBotArena, Warning, TEXT("%s value %f is above maximum %f, clamping"),
                *ValueName, (float)Value, (float)MaxValue);
            return MaxValue;
        }
        return Value;
    }
};