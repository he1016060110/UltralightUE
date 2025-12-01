/*
 * Blueprint function library implementation.
 */

#include "UltralightBlueprintLibrary.h"
#include "ULUESubsystem.h"
#include "Rendering/ULUEView.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

UUltralightView* UUltralightBlueprintLibrary::CreateTestView(UObject* WorldContextObject, int32 Width, int32 Height)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogUltralightUE, Error, TEXT("CreateTestView: Invalid WorldContextObject"));
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogUltralightUE, Error, TEXT("CreateTestView: Failed to get World"));
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogUltralightUE, Error, TEXT("CreateTestView: Failed to get GameInstance"));
		return nullptr;
	}

	UUltralightSubsystem* Subsystem = GameInstance->GetSubsystem<UUltralightSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogUltralightUE, Error, TEXT("CreateTestView: Failed to get UltralightSubsystem"));
		return nullptr;
	}

	// Create view
	UUltralightView* View = Subsystem->CreateView(Width, Height, true, TEXT(""));
	if (!View)
	{
		UE_LOG(LogUltralightUE, Error, TEXT("CreateTestView: Failed to create view"));
		return nullptr;
	}

	// Load test content
	LoadTestHTML(View, TEXT("Hello Ultralight!"), TEXT("#667eea"));

	UE_LOG(LogUltralightUE, Log, TEXT("CreateTestView: Successfully created %dx%d test view"), Width, Height);
	return View;
}

void UUltralightBlueprintLibrary::LoadTestHTML(UUltralightView* View, const FString& Title, const FString& BackgroundColor)
{
	if (!View)
	{
		UE_LOG(LogUltralightUE, Error, TEXT("LoadTestHTML: Invalid View"));
		return;
	}

	FString HTML = FString::Printf(TEXT(R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            background: linear-gradient(135deg, %s 0%%, #764ba2 100%%);
            color: white;
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            height: 100vh;
            overflow: hidden;
        }
        h1 {
            font-size: 72px;
            font-weight: 700;
            text-shadow: 2px 2px 8px rgba(0,0,0,0.3);
            margin-bottom: 20px;
            animation: fadeIn 1s ease-in;
        }
        p {
            font-size: 24px;
            opacity: 0.9;
            animation: fadeIn 1.5s ease-in;
        }
        .status {
            margin-top: 40px;
            padding: 15px 30px;
            background: rgba(255,255,255,0.2);
            border-radius: 10px;
            font-size: 18px;
            animation: pulse 2s infinite;
        }
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(-20px); }
            to { opacity: 1; transform: translateY(0); }
        }
        @keyframes pulse {
            0%%, 100%% { transform: scale(1); }
            50%% { transform: scale(1.05); }
        }
    </style>
</head>
<body>
    <h1>%s</h1>
    <p>UltralightUE Plugin is working! 🎉</p>
    <div class="status">Rendering with Ultralight 1.4.0</div>
</body>
</html>
)"), *BackgroundColor, *Title);

	View->LoadHTML(HTML, TEXT("about:blank"));

	UE_LOG(LogUltralightUE, Log, TEXT("LoadTestHTML: Loaded HTML with title '%s'"), *Title);
}
