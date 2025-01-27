// Copyright Epic Games, Inc. All Rights Reserved.
#include "CharacterPawn.h"
#include "SpartaPlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"

ACharacterPawn::ACharacterPawn()
{
	// 중력낙하 구현을위해 true
	PrimaryActorTick.bCanEverTick = true;

	// 박스 컴포넌트
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComponent"));
	// 박스 크기 설정
	BoxComponent->SetBoxExtent(FVector(100.0f, 100.0f, 80.0f));
	// 박스 컴포넌트 충돌설정
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// 박스 컴포넌트를 루트 컴포넌트로 지정
	RootComponent = BoxComponent;

	// 화살 컴포넌트
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);

	// 스켈레탈 매쉬 컴포넌트
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	// 스켈레탈 매쉬 불러오기
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(TEXT("/Game/Fab/Free_Disco_Drone___Full_Pack_In_Description/free_disco_drone_full_pack_in_description.free_disco_drone_full_pack_in_description"));
	if (SkeletalMesh.Succeeded())
	{
		SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh.Object);
	}

	// 스프링암 컴포넌트
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	// 카메라 컴포넌트
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	// 캐릭터 설정
	// 이동속도
	GroundSpeed = 600.0f;
	// 상승속도
	UpSpeed = 1500.0f;
	// 하강속도
	DownSpeed = 100.0f;
	// 회전속도
	RotateSpeed = FRotator(100.0f, 100.0f, 100.0f);
	// 낙하속도
	FallingSpeed = 100.0f;
	// 중력상수
	Gravity = -980.0f;
	// 중력가속도
	GravitySpeed = 0.0f;
	// Line Trace 끝값
	LineTraceEndValue = 30.0f;
	// 호버링 여부
	bIsHover = false;
	// 현재속도
	Velocity = FVector::ZeroVector;

	// 플레이어가 바라보는 방향으로 폰이 바라보도록 설정
	// bUseControllerRotationYaw = true;
}

void ACharacterPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 중력 및 낙하 구현 Line Trace 사용
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, LineTraceEndValue);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);

	if (bHit)
	{
		// 땅에 닿으면 회전값, 중력가속도 초기화
		GravitySpeed = 0.0f;
		SetActorRotation(FRotator(0.0f, GetActorRotation().Yaw, 0.0f));
		// UE_LOG(LogTemp, Warning, TEXT("다음 액터에 막혔습니다.: %s"), *HitResult.GetActor()->GetName());
	}
	else if (bIsHover)
	{
		// 호버 모드시 중력가속도 무시
		GravitySpeed = 0.0f;
	}
	else
	{
		// 중력가속도 : 초기속도 + 중력상수 * 시간
		GravitySpeed += Gravity * DeltaTime;
		AddActorWorldOffset(FVector(0.0f, 0.0f, GravitySpeed * DeltaTime), true);
	}

	Velocity = GetVelocity();
}

void ACharacterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 플레이어 IC를 향상된 입력 컴포넌트로 Cast
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 캐릭터의 컨트롤러를 구현한 컨트롤러로 Cast
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			// 향상된 입력에 각 행동들 바인드
			if (PlayerController->MoveAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ACharacterPawn::Move);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ACharacterPawn::Look);
			}

			if (PlayerController->UpAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->UpAction, ETriggerEvent::Triggered, this, &ACharacterPawn::Up);
			}

			if (PlayerController->DownAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->DownAction, ETriggerEvent::Triggered, this, &ACharacterPawn::Down);
			}

			if (PlayerController->RollAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->RollAction, ETriggerEvent::Triggered, this, &ACharacterPawn::Roll);
			}

			if (PlayerController->HoverAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->HoverAction, ETriggerEvent::Triggered, this, &ACharacterPawn::Hover);
			}
		}
	}
}

void ACharacterPawn::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	FVector2D MoveInput = value.Get<FVector2D>();
	
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddActorLocalOffset(FVector(MoveInput.X * GroundSpeed * GetWorld()->GetDeltaSeconds(), 0.0F, 0.0F));
	}
	else if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddActorLocalOffset(FVector(0.0F, MoveInput.Y * GroundSpeed * GetWorld()->GetDeltaSeconds(), 0.0F));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Move!"));
	}
}

void ACharacterPawn::Look(const FInputActionValue& value)
{
	if (!Controller) return;

	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);

	// Look에 회전적용
	FRotator ControllerRotation = Controller->GetControlRotation();
	SetActorRotation(FRotator(ControllerRotation.Pitch, ControllerRotation.Yaw, GetActorRotation().Roll));
}

void ACharacterPawn::Up(const FInputActionValue& value)
{
	if (!Controller) return;

	if (value.Get<bool>())
	{	
		//World기준으로 해야 액터의 위치와 회전에 관계없이 상승 및 하강가능.
		AddActorWorldOffset(FVector(0.0f, 0.0f, UpSpeed * GetWorld()->GetDeltaSeconds()));
	}
}

void ACharacterPawn::Down(const FInputActionValue& value)
{
	if (!Controller) return;

	if (value.Get<bool>())
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, DownSpeed * GetWorld()->GetDeltaSeconds() * -1.0f));
	}
	
}

void ACharacterPawn::Roll(const FInputActionValue& value)
{
	if (!Controller) return;

	float RollInput = value.Get<float>();
	AddActorLocalRotation(FRotator(0.0f, 0.0f, RotateSpeed.Roll * RollInput * GetWorld()->GetDeltaSeconds()));

}

void ACharacterPawn::Hover(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		bIsHover = !bIsHover;

		if (GEngine)
		{
			if (bIsHover)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Drone Hovering ON"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Drone Hovering OFF"));
			}
		}
	}
}