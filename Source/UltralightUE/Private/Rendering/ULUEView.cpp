/*
 * UObject wrapper around Ultralight view for Blueprint/C++ usage.
 */

#include "Rendering/ULUEView.h"
#include "Rendering/ULUERenderer.h"
#include "Engine/TextureRenderTarget2D.h"
#include "InputCoreTypes.h"
#include "Ultralight/KeyEvent.h"

void UUltralightView::InitializeNative(TSharedRef<FULUERenderer> InRenderer, const TSharedPtr<FULUEView>& InView)
{
	Renderer = InRenderer;
	NativeView = InView;

	if (NativeView.IsValid())
	{
		RenderTarget = NativeView->GetRenderTarget();
		RenderTargetWrapper = NativeView->GetRenderTargetWrapper();
	}
}

void UUltralightView::BeginDestroy()
{
	ReleaseNative();
	Super::BeginDestroy();
}

void UUltralightView::LoadURL(const FString& URL)
{
	if (NativeView.IsValid())
	{
		NativeView->LoadURL(URL);
	}
}

void UUltralightView::LoadHTML(const FString& HTML, const FString& VirtualURL)
{
	if (NativeView.IsValid())
	{
		NativeView->LoadHTML(HTML, VirtualURL);
	}
}

void UUltralightView::Resize(int32 Width, int32 Height)
{
	if (NativeView.IsValid())
	{
		NativeView->Resize(FIntPoint(Width, Height));
	}
}

void UUltralightView::SetFocused(bool bFocused)
{
	if (NativeView.IsValid())
	{
		NativeView->SetFocused(bFocused);
	}
}

void UUltralightView::InjectMouseMove(const FVector2D& Position)
{
	if (NativeView.IsValid())
	{
		NativeView->InjectMouseEvent(ultralight::MouseEvent::kType_MouseMoved, Position, ultralight::MouseEvent::kButton_None, 0);
	}
}

void UUltralightView::InjectMouseButton(const FVector2D& Position, EULUEMouseButton Button, bool bPressed, bool bShift, bool bCtrl, bool bAlt, bool bMeta)
{
	if (!NativeView.IsValid())
	{
		return;
	}

	ultralight::MouseEvent::Type Type = bPressed ? ultralight::MouseEvent::kType_MouseDown : ultralight::MouseEvent::kType_MouseUp;
	const uint32 Modifiers = BuildModifierFlags(bShift, bCtrl, bAlt, bMeta);
	NativeView->InjectMouseEvent(Type, Position, ToUltralightButton(Button), Modifiers);
}

void UUltralightView::InjectScroll(const FVector2D& ScrollDelta, bool bByPage, bool bShift, bool bCtrl, bool bAlt, bool bMeta)
{
	if (NativeView.IsValid())
	{
		NativeView->InjectScroll(ScrollDelta, bByPage, BuildModifierFlags(bShift, bCtrl, bAlt, bMeta));
	}
}

void UUltralightView::InjectKeyDown(FKey Key, bool bIsRepeat, bool bShift, bool bCtrl, bool bAlt, bool bMeta)
{
	if (!NativeView.IsValid())
	{
		return;
	}

	const int32 VirtualKey = GetVirtualKeyCode(Key);
	if (VirtualKey == 0)
	{
		return;
	}

	const uint32 Modifiers = BuildModifierFlags(bShift, bCtrl, bAlt, bMeta);
	ultralight::KeyEvent Event = BuildKeyEvent(ultralight::KeyEvent::kType_RawKeyDown, VirtualKey, Modifiers, bIsRepeat);
	NativeView->InjectKeyEvent(Event);
}

void UUltralightView::InjectKeyUp(FKey Key, bool bShift, bool bCtrl, bool bAlt, bool bMeta)
{
	if (!NativeView.IsValid())
	{
		return;
	}

	const int32 VirtualKey = GetVirtualKeyCode(Key);
	if (VirtualKey == 0)
	{
		return;
	}

	const uint32 Modifiers = BuildModifierFlags(bShift, bCtrl, bAlt, bMeta);
	ultralight::KeyEvent Event = BuildKeyEvent(ultralight::KeyEvent::kType_KeyUp, VirtualKey, Modifiers, false);
	NativeView->InjectKeyEvent(Event);
}

void UUltralightView::InjectChar(const FString& Characters, bool bShift, bool bCtrl, bool bAlt, bool bMeta)
{
	if (!NativeView.IsValid() || Characters.IsEmpty())
	{
		return;
	}

	ultralight::KeyEvent Event;
	Event.type = ultralight::KeyEvent::kType_Char;
	Event.modifiers = BuildModifierFlags(bShift, bCtrl, bAlt, bMeta);
	Event.text = ultralight::String(TCHAR_TO_UTF8(*Characters));
	Event.unmodified_text = Event.text;
	Event.virtual_key_code = Characters[0];
	Event.native_key_code = Event.virtual_key_code;
	Event.is_keypad = false;
	Event.is_auto_repeat = false;
	Event.is_system_key = false;

	NativeView->InjectKeyEvent(Event);
}

void UUltralightView::ReleaseNative()
{
	if (Renderer.IsValid() && NativeView.IsValid())
	{
		Renderer.Pin()->DestroyView(NativeView);
	}

	NativeView.Reset();
	Renderer.Reset();
	RenderTarget = nullptr;
	RenderTargetWrapper = nullptr;
}

/* ------------------------------- Helpers ---------------------------------- */

ultralight::MouseEvent::Button UUltralightView::ToUltralightButton(EULUEMouseButton Button)
{
	switch (Button)
	{
	case EULUEMouseButton::Left:
		return ultralight::MouseEvent::kButton_Left;
	case EULUEMouseButton::Middle:
		return ultralight::MouseEvent::kButton_Middle;
	case EULUEMouseButton::Right:
		return ultralight::MouseEvent::kButton_Right;
	default:
		return ultralight::MouseEvent::kButton_None;
	}
}

int32 UUltralightView::GetVirtualKeyCode(const FKey& Key)
{
	const uint32* KeyCode = nullptr;
	const uint32* CharCode = nullptr;
	FInputKeyManager::Get().GetCodesFromKey(Key, KeyCode, CharCode);
	const uint32 ResolvedKey = (KeyCode && *KeyCode != 0) ? *KeyCode : ((CharCode && *CharCode != 0) ? *CharCode : 0);
	return ResolvedKey != 0 ? static_cast<int32>(ResolvedKey) : 0;
}

uint32 UUltralightView::BuildModifierFlags(bool bShift, bool bCtrl, bool bAlt, bool bMeta)
{
	uint32 Flags = 0;
	if (bShift)
	{
		Flags |= ultralight::KeyEvent::kMod_ShiftKey;
	}
	if (bCtrl)
	{
		Flags |= ultralight::KeyEvent::kMod_CtrlKey;
	}
	if (bAlt)
	{
		Flags |= ultralight::KeyEvent::kMod_AltKey;
	}
	if (bMeta)
	{
		Flags |= ultralight::KeyEvent::kMod_MetaKey;
	}
	return Flags;
}

ultralight::KeyEvent UUltralightView::BuildKeyEvent(ultralight::KeyEvent::Type Type, int32 VirtualKey, uint32 Modifiers, bool bIsRepeat)
{
	ultralight::KeyEvent Event;
	Event.type = Type;
	Event.modifiers = Modifiers;
	Event.virtual_key_code = VirtualKey;
	Event.native_key_code = VirtualKey;
	Event.is_keypad = false;
	Event.is_auto_repeat = bIsRepeat;
	Event.is_system_key = false;

	ultralight::GetKeyIdentifierFromVirtualKeyCode(Event.virtual_key_code, Event.key_identifier);
	ultralight::GetKeyFromVirtualKeyCode(Event.virtual_key_code, (Modifiers & ultralight::KeyEvent::kMod_ShiftKey) != 0, Event.unmodified_text);
	return Event;
}
