// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CharacterPawn.generated.h"

class UBoxComponent;
class UArrowComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT7_API ACharacterPawn : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category = "Character")
	UBoxComponent* BoxComponent;
	UPROPERTY()
	UArrowComponent* ArrowComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Character")
	USkeletalMeshComponent* SkeletalMeshComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* CameraComponent;

public:
	ACharacterPawn();

	// 캐릭터 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movements")
	float GroundSpeed;
	// 캐릭터 상승속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movements")
	float UpSpeed;
	// 캐릭터 하강속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movements")
	float DownSpeed;
	// 캐릭터 회전속도 (Pitch, Yaw, Roll)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movements")
	FRotator RotateSpeed;
	// 캐릭터 낙하속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movements")
	float FallingSpeed;
	// 중력 상수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movements")
	float Gravity;
	// 중력 가속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movements")
	float GravitySpeed;
	// Line Trace 끝값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movements")
	float LineTraceEndValue;
	// 호버링 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movements")
	bool bIsHover;
	// 현재 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movements")
	FVector Velocity;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void Up(const FInputActionValue& value);
	UFUNCTION()
	void Down(const FInputActionValue& value);
	UFUNCTION()
	void Roll(const FInputActionValue& value);
	UFUNCTION()
	void Hover(const FInputActionValue& value);
};
