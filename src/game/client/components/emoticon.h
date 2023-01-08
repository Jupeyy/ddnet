/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_EMOTICON_H
#define GAME_CLIENT_COMPONENTS_EMOTICON_H
#include <base/vmath.h>
#include <game/client/component.h>

class CEmoticon : public CComponent
{
	bool m_WasActive;
	bool m_Active;

	vec2 m_SelectorMouse;
	int m_SelectedEmote;
	int m_SelectedEyeEmote;

	static void ConKeyEmoticon(IConsole::IResult *pResult, void *pUserData);
	static void ConEmote(IConsole::IResult *pResult, void *pUserData);

	EComponentMouseMovementBlockMode OnMouseWrongStateImpl();

public:
	CEmoticon();
	virtual int Sizeof() const override { return sizeof(*this); }

	virtual void OnReset() override;
	virtual void OnConsoleInit() override;
	virtual void OnRender() override;
	virtual void OnRelease() override;

	virtual EComponentMouseMovementBlockMode OnMouseInWindowPos(int X, int Y) override;
	virtual EComponentMouseMovementBlockMode OnMouseAbsoluteInWindowPos(int X, int Y) override;
	virtual EComponentMouseMovementBlockMode OnMouseInWindowRelativeMove(int X, int Y) override;
	virtual EComponentMouseMovementBlockMode OnMouseRelativeMove(float x, float y, IInput::ECursorType CursorType) override;

	void Emote(int Emoticon);
	void EyeEmote(int EyeEmote);
};

#endif
