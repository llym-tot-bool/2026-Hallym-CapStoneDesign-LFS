


#include "SoulslikeDummyCharacter.h"


// Sets default values
ASoulslikeDummyCharacter::ASoulslikeDummyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	asc = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	attribSet = CreateDefaultSubobject<USLCharacterAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void ASoulslikeDummyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

UAbilitySystemComponent* ASoulslikeDummyCharacter::GetAbilitySystemComponent() const
{
	return asc;
}

// Called every frame
void ASoulslikeDummyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASoulslikeDummyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


