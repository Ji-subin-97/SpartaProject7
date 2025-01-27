// Copyright Epic Games, Inc. All Rights Reserved.
#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr), MoveAction(nullptr), LookAction(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 현재 플레이어의 로컬데이터 가져오기
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// 로컬플레이어의 향상된 입력시스템 가져오기
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				// 향상된 입력시스템에 IMC(입력 맵핑) 넣기
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
