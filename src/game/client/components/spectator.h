/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_SPECTATOR_H
#define GAME_CLIENT_COMPONENTS_SPECTATOR_H
#include <base/vmath.h>

#include <game/client/component.h>

class CSpectator : public CComponent
{
	enum
	{
		NO_SELECTION = -3,
	};

	bool m_Active;
	bool m_WasActive;

	int m_SelectedSpectatorID;
	vec2 m_SelectorMouse;

	bool CanChangeSpectator();
	void SpectateNext(bool Reverse);

	static void ConKeySpectator(IConsole::IResult *pResult, void *pUserData);
	static void ConSpectate(IConsole::IResult *pResult, void *pUserData);
	static void ConSpectateNext(IConsole::IResult *pResult, void *pUserData);
	static void ConSpectatePrevious(IConsole::IResult *pResult, void *pUserData);
	static void ConSpectateClosest(IConsole::IResult *pResult, void *pUserData);

	EComponentMouseMovementBlockMode OnMouseWrongStateImpl();

public:
	CSpectator();
	virtual int Sizeof() const override { return sizeof(*this); }

	virtual void OnConsoleInit() override;
	virtual void OnRender() override;
	virtual void OnRelease() override;
	virtual void OnReset() override;

	virtual EComponentMouseMovementBlockMode OnMouseInWindowPos(int X, int Y) override;
	virtual EComponentMouseMovementBlockMode OnMouseAbsoluteInWindowPos(int X, int Y) override;
	virtual EComponentMouseMovementBlockMode OnMouseInWindowRelativeMove(int X, int Y) override;
	virtual EComponentMouseMovementBlockMode OnMouseRelativeMove(float x, float y, IInput::ECursorType CursorType) override;

	void Spectate(int SpectatorID);
};

#endif
