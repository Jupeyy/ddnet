/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_SCOREBOARD_H
#define GAME_CLIENT_COMPONENTS_SCOREBOARD_H
#include <game/client/component.h>

class CScoreboard : public CComponent
{
	void RenderGoals(float x, float y, float w);
	void RenderSpectators(float x, float y, float w);
	void RenderScoreboard(float x, float y, float w, int Team, const char *pTitle, int NumPlayers = -1);
	void RenderScoreboardFNG(float x, float y, float w, int Team, const char *pTitle);
	void RenderRecordingNotification(float x);

	static void ConKeyScoreboard(IConsole::IResult *pResult, void *pUserData);

	const char *GetClanName(int Team);

	bool m_Active;

	static constexpr float ms_TeamScoreboardSingleEntityMaxHeight = 50.0f;
	static constexpr float ms_TeamScoreboardEntitySpacing = 5.0f;
	static constexpr float ms_TeamScoreboardTitleHeightMax = 60.0f;
	static constexpr float ms_TeamScoreboardTitleSpectatorHeightMax = 30.0f;
	static constexpr float ms_TeamScoreboardHeadlineHeightMax = 20.0f;
	static constexpr float ms_TeamScoreboardFootlineHeightMax = ms_TeamScoreboardHeadlineHeightMax;
	static constexpr float ms_TeamScoreboardGroupOffset = 10.0f;

	static constexpr int ms_ScoreboardUIRects = 60;
	static constexpr int ms_ScoreboardUIRectsPerPlayer = 10;

	int GetPlayerCountOnScoreboard(int PlayerCount);
	float GetScoreboardEntitiesHeight(int &PlayerCountOnScoreboard, int PlayerCount, int Team);
	float GetScoreboardHeight(int &PlayerCountOnScoreboard, int PlayerCount, int Team, float &ScoreboardEntitiesHeight);
	float GetScoreboardHeightUntilEntries(int Team);
	float GetScoreboardHeightAfterEntries(int Team);

	IGraphics::CTextureHandle m_ScoreboardImageFNGBG;
	IGraphics::CTextureHandle m_ScoreboardImageFNG;

	CUIElement m_ScoreboardGeneral;
	CUIElement m_ScoreboardPlayers[MAX_CLIENTS];

public:
	CScoreboard();
	void OnInit() override;
	void OnReset() override;
	void OnConsoleInit() override;
	void OnRender() override;
	void OnRelease() override;

	bool Active();

	// DDRace

	void OnMessage(int MsgType, void *pRawMsg) override;

	float GetCurScoreboardHeight();
	float GetCurScoreboardYOffset();
	static constexpr float ms_ScreenHeight = 400 * 3.0f;
	static constexpr float ms_ScoreboardYOffset = 150.0f;

private:
	float m_ServerRecord;
};

#endif
