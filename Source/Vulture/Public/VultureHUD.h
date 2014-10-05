// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "VultureHUD.generated.h"

UCLASS()
class AVultureHUD : public AHUD
{
	GENERATED_UCLASS_BODY()
	/** Put Roboto Here */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JoyHUD)
	UFont* UE4Font;
public:

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

