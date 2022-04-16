#pragma once

#include "CoreMinimal.h"
#include "AVM_CameraComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class UAVM_CameraComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	// Constructor
	UAVM_CameraComponent();

public:

	// Max Distance of default camera zoom
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float MaxSpringArmLength;

	// Min Distance of default camera zoom
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float MinSpringArmLength;	

	// Set the default angle the SpringArmComponent is using ( 1 is directly above the character looking down; 0 is horizontal )
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float DefaultCameraAngle;

	// The Camera zooms interpolation speed
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float CameraZoomInterpSpeed;
	

private:

	// SpringArm Component
	class USpringArmComponent* SpringArmComponent;
	
	// Camera Component
	class UCameraComponent* CameraComponent;

public:

	// Returns the SpringArmComponent
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }

	// Returns the Camera Component
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	
	// Returns the camera forward vector on the XY Plane
	FVector GetCameraXYForwardVector() const;

	// Returns true if we are currently rotating the camera
	FORCEINLINE bool InCameraRotateMode() const { return bCameraRotateModeActive; }
	
public:

	// UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// UActorComponent Interface

	// Create Components
	void CreateComponents();

	// Binds input directly inside the component
	void BindInputs();

private:

	// Increment or decrements the SpringArmComponents length
	void AddCameraZoom(const float InZoom);
	// Enter Camera rotation mode
	void EnterRotateCameraMode();
	// Exit Camera rotation mode
	void ExitRotateCameraMode();
	// Rotate Camera using mouse delta
	void RotateCamera() const;
	// Zoom Camera
	void ZoomCamera(const float DeltaTime) const;
	
private:

	// Owning Character
	class ACharacter* OwningCharacter;

	// Owning PlayerController
	class APlayerController* OwningPlayerController;

private:

	bool bCameraRotateModeActive;

	// Used by ZoomCamera as the target zoom level to interpolate towards
	float RequestedZoomLevel;
};
