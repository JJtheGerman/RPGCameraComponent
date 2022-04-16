#include "AVM_CameraComponent.h"

#include "AVM/AVMGameMode.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

UAVM_CameraComponent::UAVM_CameraComponent()
{
	// This component does not need to replicate 
	SetIsReplicated(false);

	// Enable component ticking
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Initialize variables to default values
	bCameraRotateModeActive = false;
	MaxSpringArmLength = 1000.f;
	MinSpringArmLength = 200.f;
	RequestedZoomLevel = MaxSpringArmLength;
	CameraZoomInterpSpeed = 5.f;
	DefaultCameraAngle = 0.75f;
}

void UAVM_CameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// We create the components dynamically on BeginPlay. This allows complete encapsulation.
	CreateComponents();
	
	// This component should only be attached to Characters
	OwningCharacter = CastChecked<ACharacter>(GetOwner());
	
	// Client Only
	if(OwningCharacter->IsLocallyControlled())
	{
		// The player controller currently responsible for this component
		OwningPlayerController = CastChecked<APlayerController>(OwningCharacter->GetController());

		// Bind player inputs
		BindInputs();
	}
}

void UAVM_CameraComponent::CreateComponents()
{
	// Create SpringArm component
	SpringArmComponent = NewObject<USpringArmComponent>(this, USpringArmComponent::StaticClass(), FName("SpringArm"), RF_NoFlags);
	SpringArmComponent->TargetArmLength = MaxSpringArmLength;
	SpringArmComponent->bInheritYaw = false; // The characters rotation and the spring arms rotation are not related
	SpringArmComponent->bEnableCameraRotationLag = true; // The camera only rotates on the yaw axis. Which makes turning the camera smooth
	SpringArmComponent->bEnableCameraLag = true; // Enable positional camera lag
	SpringArmComponent->bDoCollisionTest = false; // Turn off collision testing so the camera doesnt zoom in when you are standing behind a blocking object
	SpringArmComponent->SetWorldRotation(FRotator(FMath::Lerp(0, -90, DefaultCameraAngle), 0, 0));
	SpringArmComponent->SetupAttachment(GetOwner()->GetRootComponent());
	SpringArmComponent->RegisterComponent();

	// Create Camera component
	CameraComponent = NewObject<UCameraComponent>(this, UCameraComponent::StaticClass(), FName("Camera"), RF_NoFlags);
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->RegisterComponent();
}

void UAVM_CameraComponent::BindInputs()
{
	// If the InputComponent is null we set a timer to call this function again in 0.25 seconds until it is no longer null so we can bind input
	// Some dedicated server order of execution stuff makes this necessary 
	if(!OwningCharacter->InputComponent)
	{
		FTimerHandle RetryBindInput;
		GetWorld()->GetTimerManager().SetTimer(RetryBindInput, this, &UAVM_CameraComponent::BindInputs, 0.25f, false);
		return;
	}
	
	// Bind zooming the camera in and out
	OwningCharacter->InputComponent->BindAxis("MouseScroll", this, &UAVM_CameraComponent::AddCameraZoom);
	
	// Bind entering the camera rotation mode
	OwningCharacter->InputComponent->BindAction("RotateCamera", IE_Pressed, this, &UAVM_CameraComponent::EnterRotateCameraMode);
	OwningCharacter->InputComponent->BindAction("RotateCamera", IE_Released, this, &UAVM_CameraComponent::ExitRotateCameraMode);
}

void UAVM_CameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Handle rotating the camera
	if(InCameraRotateMode())
	{
		RotateCamera();
	}

	// Handle Camera zoom on tick to interp values smoothly
	ZoomCamera(DeltaTime);
}

void UAVM_CameraComponent::AddCameraZoom(const float InZoom)
{
	// Return if the zoom input is too low
	if(FMath::IsNearlyZero(InZoom)) { return; }

	// Set new target zoom level that ZoomCamera() will interpolate towards
	RequestedZoomLevel += InZoom;

	// Clamp zoom level
	RequestedZoomLevel = FMath::Clamp(RequestedZoomLevel, MinSpringArmLength, MaxSpringArmLength);
}

void UAVM_CameraComponent::EnterRotateCameraMode()
{
	bCameraRotateModeActive = true;
}

void UAVM_CameraComponent::ExitRotateCameraMode()
{
	bCameraRotateModeActive = false;
}

void UAVM_CameraComponent::RotateCamera() const
{
	// Get Mouse Delta
	FVector2d MouseDelta;
	OwningPlayerController->GetInputMouseDelta(MouseDelta.X, MouseDelta.Y);

	// Multiply to rotate faster
	constexpr float RotateSpeedMultiplier = 3.f;
	
	// Rotate the SpringArm using the X Delta (left to right mouse movement)
	SpringArmComponent->AddWorldRotation(FRotator(0, MouseDelta.X * RotateSpeedMultiplier, 0));
}

void UAVM_CameraComponent::ZoomCamera(const float DeltaTime) const
{
	// Interp to requested zoom level
	SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, RequestedZoomLevel, DeltaTime, CameraZoomInterpSpeed);
}

FVector UAVM_CameraComponent::GetCameraXYForwardVector() const
{
	FVector ForwardVector = CameraComponent->GetForwardVector();

	// Because of the intended purpose of this camera system. We zero the Z axis so the forward vector is just on the X,Y plane
	ForwardVector.Z = 0.f;

	return ForwardVector.GetSafeNormal();
}
