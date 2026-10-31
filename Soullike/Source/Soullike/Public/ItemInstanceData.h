#pragma once

#include "CoreMinimal.h"
#include "ItemInstanceData.generated.h"

USTRUCT(BlueprintType)
struct SOULLIKE_API FItemInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FGuid InstanceId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName DefinitionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity = 1;

    bool operator==(const FItemInstanceData& Other) const
	{
		return InstanceId == Other.InstanceId && DefinitionId == Other.DefinitionId;
	}
};