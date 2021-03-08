#include "base/color.h"
#include "engine/textrender.h"
#include "game/generated/protocol.h"
#include "scoreboard.h"
#include <game/client/animstate.h>
#include <game/client/components/countryflags.h>
#include <game/client/gameclient.h>

int CScoreboard::GetPlayerCountOnScoreboard(int PlayerCount)
{
	int PlayerCountOnScoreboard = 8;
	if(PlayerCount > 48)
	{
		PlayerCountOnScoreboard = 64;
	}
	else if(PlayerCount > 32)
	{
		PlayerCountOnScoreboard = 48;
	}
	else if(PlayerCount > 24)
	{
		PlayerCountOnScoreboard = 32;
	}
	else if(PlayerCount > 16)
	{
		PlayerCountOnScoreboard = 24;
	}
	else if(PlayerCount > 12)
	{
		PlayerCountOnScoreboard = 16;
	}
	else if(PlayerCount > 8)
	{
		PlayerCountOnScoreboard = 12;
	}
	return PlayerCountOnScoreboard;
}

float CScoreboard::GetScoreboardEntitiesHeight(int &PlayerCountOnScoreboard, int PlayerCount, int Team)
{
	PlayerCountOnScoreboard = GetPlayerCountOnScoreboard(PlayerCount);

	float ScoreboardEntitiesFullHeight = ms_TeamScoreboardSingleEntityMaxHeight * 8.0;
	ScoreboardEntitiesFullHeight += ms_TeamScoreboardEntitySpacing * 7.0;

	float ScoreboardHugeEntitiesFullHeight = ms_TeamScoreboardSingleEntityMaxHeight * 10.0;
	ScoreboardHugeEntitiesFullHeight += ms_TeamScoreboardEntitySpacing * 9.0;

	return (PlayerCountOnScoreboard <= 8) ? ScoreboardEntitiesFullHeight : ScoreboardHugeEntitiesFullHeight;
}

float CScoreboard::GetScoreboardHeightUntilEntries(int Team)
{
	float TitleHeightMax = ms_TeamScoreboardTitleHeightMax;
	if(Team == TEAM_SPECTATORS)
	{
		TitleHeightMax = ms_TeamScoreboardTitleSpectatorHeightMax;
	}
	float ScoreboardHeight = TitleHeightMax;
	ScoreboardHeight += ms_TeamScoreboardGroupOffset * 2.0f;
	ScoreboardHeight += ms_TeamScoreboardHeadlineHeightMax;

	return ScoreboardHeight;
}

float CScoreboard::GetScoreboardHeightAfterEntries(int Team)
{
	float ScoreboardHeightAfter = 0;
	if(Team != TEAM_SPECTATORS)
	{
		ScoreboardHeightAfter += ms_TeamScoreboardGroupOffset * 2.0f;
		ScoreboardHeightAfter += ms_TeamScoreboardFootlineHeightMax;
	}

	return ScoreboardHeightAfter;
}

float CScoreboard::GetScoreboardHeight(int &PlayerCountOnScoreboard, int PlayerCount, int Team, float &ScoreboardEntitiesHeight)
{
	ScoreboardEntitiesHeight = GetScoreboardEntitiesHeight(PlayerCountOnScoreboard, PlayerCount, Team);

	float ScoreboardHeight = GetScoreboardHeightUntilEntries(Team);
	ScoreboardHeight += ScoreboardEntitiesHeight;
	ScoreboardHeight += GetScoreboardHeightAfterEntries(Team);
	return ScoreboardHeight;
}

float CScoreboard::GetCurScoreboardHeight()
{
	float h = 760.0f;
	if(m_pClient->m_Snap.m_pGameInfoObj)
	{
		if((m_pClient->m_Snap.m_pGameInfoObj->m_GameFlags & GAMEFLAG_TEAMS) != 0)
		{
			if(GameClient()->m_GameInfo.m_EntitiesFNG)
			{
				int PlayerSCount = 0;
				int NumPlayersTeam = maximum(m_pClient->m_Snap.m_aTeamSize[TEAM_RED], m_pClient->m_Snap.m_aTeamSize[TEAM_BLUE]);
				float SBEntriesHeightDummy = 0;
				h = GetScoreboardHeight(PlayerSCount, NumPlayersTeam, TEAM_RED, SBEntriesHeightDummy);
			}
		}
	}

	return h;
}

float CScoreboard::GetCurScoreboardYOffset()
{
	return ms_ScoreboardYOffset;
}

static const ColorRGBA gs_ColorHeaderRed(0.7f, 0.25f, 0.25f, 0.9f);
static const ColorRGBA gs_ColorHeaderBlue(0.25f, 0.35f, 0.7f, 0.9f);
static const ColorRGBA gs_ColorHeaderPurple(0.12f, 0.01f, 0.17f, 0.5f);
/*
static const ColorRGBA gs_ColorEntryRed(0.65f, 0.2f, 0.2f, 0.85f);
static const ColorRGBA gs_ColorEntryBlue(0.2f, 0.3f, 0.65f, 0.85f);
static const ColorRGBA gs_ColorEntryPurple(0.08f, 0.0f, 0.1f, 0.85f);*/
static const ColorRGBA gs_ColorEntryRed(0.3f, 0.3f, 0.3f, 0.75f);
static const ColorRGBA gs_ColorEntryBlue(gs_ColorEntryRed);
static const ColorRGBA gs_ColorEntryPurple(0.12f, 0.01f, 0.17f, 0.5f);

static const ColorRGBA gs_ColorHeaderFont(0.5f, 0.5f, 0.5f, 1.0f);
static const ColorRGBA gs_ColorHeaderFontOutline(1.0f, 1.0f, 1.0f, 0.0f);

static const ColorRGBA gs_ColorFeetFont(gs_ColorHeaderFont);
static const ColorRGBA gs_ColorFeetFontOutline(gs_ColorHeaderFontOutline);

static const ColorRGBA gs_ColorHeader(0.15f, 0.15f, 0.15f, 0.85f);
static const ColorRGBA gs_ColorFeet(gs_ColorHeader);

static const ColorRGBA gs_ColorScoreFont(1.0f, 1.0f, 1.0f, 1.0f);
static const ColorRGBA gs_ColorScoreFontOutline(0.0f, 0.0f, 0.0f, 0.0f);

static const ColorRGBA gs_BoardColorBG(0.0f, 0.0f, 0.0f, 0.65f);

void CScoreboard::RenderScoreboardFNG(float x, float y, float w, int Team, const char *pTitle)
{
	if(Team < -1)
		Team = 0;

	int NumPlayersTeam = maximum(m_pClient->m_Snap.m_aTeamSize[TEAM_RED], m_pClient->m_Snap.m_aTeamSize[TEAM_BLUE]);
	int NumPlayersSpectator = m_pClient->m_Snap.m_NumPlayers - (m_pClient->m_Snap.m_aTeamSize[TEAM_RED] + m_pClient->m_Snap.m_aTeamSize[TEAM_BLUE]);

	if(Team == TEAM_SPECTATORS && NumPlayersSpectator <= 0)
		return;

	int NextPlayerCount = 8;
	float ScoreboardRealEntitiesFullHeight = 0;

	float TitleHeightMax = ms_TeamScoreboardTitleHeightMax;
	if(Team == TEAM_SPECTATORS)
	{
		TitleHeightMax = ms_TeamScoreboardTitleSpectatorHeightMax;
	}
	const float TitleFontsize = TitleHeightMax * 2.0f / 3.0f;
	const float TitleTextOffset = 20.0f;

	float h = GetScoreboardHeight(NextPlayerCount, NumPlayersTeam, Team, ScoreboardRealEntitiesFullHeight);

	const float Spacing = ms_TeamScoreboardEntitySpacing;
	float LineHeightFull = ms_TeamScoreboardSingleEntityMaxHeight;
	LineHeightFull = (ScoreboardRealEntitiesFullHeight - ((NextPlayerCount - 1) * Spacing)) / NextPlayerCount;

	if(Team == TEAM_SPECTATORS)
	{
		int SpectatorScoreboardPlayerCount = GetPlayerCountOnScoreboard(NumPlayersSpectator);
		SpectatorScoreboardPlayerCount = clamp(SpectatorScoreboardPlayerCount, 1, 24);
		float HeightUntilEntries = GetScoreboardHeightUntilEntries(Team);
		float FullPossibleEntriesHeight = (ms_ScreenHeight - y) - HeightUntilEntries;
		float LineHeightFullSpec = (FullPossibleEntriesHeight - ((SpectatorScoreboardPlayerCount - 1) * Spacing)) / SpectatorScoreboardPlayerCount;
		// only use smallest lineheight
		LineHeightFull = minimum(LineHeightFull, LineHeightFullSpec);
		h = HeightUntilEntries + LineHeightFull * NumPlayersSpectator + Spacing * (NumPlayersSpectator - 1);
	}

	int UIRectIndex = 0;
	int Corners = CUI::CORNER_BL;
	int CornersTitle = CUI::CORNER_TL;
	if(Team == TEAM_BLUE)
	{
		Corners = CUI::CORNER_BR;
		CornersTitle = CUI::CORNER_TR;
		UIRectIndex += ms_ScoreboardUIRects / 3;
	}
	else if(Team == TEAM_SPECTATORS)
	{
		Corners = 0;
		CornersTitle = 0;
		UIRectIndex += 2 * (ms_ScoreboardUIRects / 3);
	}

	// background
	RenderTools()->DrawUIElRect(*m_ScoreboardGeneral.Get(UIRectIndex++), x, y + TitleHeightMax, w, h - TitleHeightMax, gs_BoardColorBG, Corners, ms_TeamScoreboardGroupOffset);

	char aBuf[128] = {0};

	// render title
	const float TitleDiffSize = TitleHeightMax - TitleFontsize;
	const float TitleOffset = TitleDiffSize / 2.0f;
	const float ScoreFontsize = 60.0f;
	const float ScoreDiffSize = TitleHeightMax - ScoreFontsize;
	const float ScoreOffset = ScoreDiffSize / 2.0f;
	if(!pTitle)
	{
		if(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags & GAMESTATEFLAG_GAMEOVER)
			pTitle = Localize("Game over");
		else
		{
			str_utf8_truncate(aBuf, sizeof(aBuf), Client()->GetCurrentMap(), 16);
			if(str_comp(aBuf, Client()->GetCurrentMap()))
				str_append(aBuf, "…", sizeof(aBuf));
			pTitle = aBuf;
		}
	}

	ColorRGBA ColorTitle(1.0f, 1.0f, 1.0f, 1.0f);
	if(Team == TEAM_RED)
		ColorTitle = gs_ColorHeaderRed;
	else if(Team == TEAM_BLUE)
		ColorTitle = gs_ColorHeaderBlue;
	else if(Team == TEAM_SPECTATORS)
		ColorTitle = gs_ColorHeaderPurple;
	Graphics()->TextureSet(m_ScoreboardImageFNGBG);
	Graphics()->WrapNormal();
	Graphics()->QuadsSetSubset(0, 0, (w / TitleHeightMax), 1);
	RenderTools()->DrawUIElRect(*m_ScoreboardGeneral.Get(UIRectIndex++), x, y, w, TitleHeightMax, ColorTitle, CornersTitle, 10.0f, ERenderToolsQuadTextureMode::RENDER_TOOLS_QUAD_TEXTURE_MODE_RESPECT_REPEATED);
	Graphics()->QuadsSetSubset(0, 0, 1, 1);

	float tw = TextRender()->TextWidth(0, TitleFontsize, pTitle, -1, -1);

	Graphics()->TextureSet(m_ScoreboardImageFNG);
	Graphics()->WrapClamp();
	ColorRGBA TextureColor(1.0f, 1.0f, 1.0f, 0.5f);
	if(Team == TEAM_RED)
	{
		Graphics()->QuadsSetSubset(0, 0, 0.5f, 1.0f);
		RenderTools()->DrawUIElRectQuad(*m_ScoreboardGeneral.Get(UIRectIndex++), x + TitleTextOffset + tw - 25.0f, y, TitleHeightMax, TitleHeightMax, TextureColor, false);
	}
	else if(Team == TEAM_BLUE)
	{
		Graphics()->QuadsSetSubset(0.5f, 0, 1.0f, 1.0f);
		RenderTools()->DrawUIElRectQuad(*m_ScoreboardGeneral.Get(UIRectIndex++), x + w - TitleTextOffset - tw - 25.0f, y, TitleHeightMax, TitleHeightMax, TextureColor, false);
	}
	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	Graphics()->WrapNormal();

	if(Team == TEAM_RED)
		UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), x + TitleTextOffset, y + TitleOffset, w, TitleFontsize, pTitle, TitleFontsize, -1);
	else if(Team == TEAM_BLUE)
		UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), x, y + TitleOffset, w - TitleTextOffset, TitleFontsize, pTitle, TitleFontsize, 1);
	else if(Team == TEAM_SPECTATORS)
		UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), x, y + TitleOffset, w, TitleFontsize, pTitle, TitleFontsize, 0);

	int Score = Team == TEAM_RED ? m_pClient->m_Snap.m_pGameDataObj->m_TeamscoreRed : (Team == TEAM_BLUE ? m_pClient->m_Snap.m_pGameDataObj->m_TeamscoreBlue : 0);
	str_format(aBuf, sizeof(aBuf), "%d", Score);

	TextRender()->TextColor(gs_ColorScoreFont);
	TextRender()->TextOutlineColor(gs_ColorScoreFontOutline);

	if(Team == TEAM_RED)
		UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), x, y + ScoreOffset, w - 20.0f, ScoreFontsize, aBuf, ScoreFontsize, 1, -1, 0);
	else if(Team == TEAM_BLUE)
		UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), x + 20.0f, y + ScoreOffset, w, ScoreFontsize, aBuf, ScoreFontsize, -1, -1, 0);

	TextRender()->TextColor(TextRender()->DefaultTextColor());
	TextRender()->TextOutlineColor(TextRender()->DefaultTextOutlineColor());

	// calculate measurements
	const float SmallSpacing = 5.0f;
	float VerySmallSpacing = SmallSpacing / 2.0f;
	float SmallSpacingSize = SmallSpacing * 2.0f;
	float XEntriesLeftSize = 0;
	float XEntries = x + XEntriesLeftSize + SmallSpacing;
	float WidthEntries = w - SmallSpacingSize - XEntriesLeftSize;

	float LineHeight = LineHeightFull;
	const float MaxPossibleFontSize = ms_TeamScoreboardSingleEntityMaxHeight / 2.0f - 4.0f;
	float FontSize = clamp(LineHeight - 2.0f, 2.0f, MaxPossibleFontSize);

	float TeeScoreOffset = XEntries, ScoreLength = TextRender()->TextWidth(0, MaxPossibleFontSize, "9999", -1, -1.0f);
	float TeeSmallSpacing = VerySmallSpacing;
	float PingLength = TextRender()->TextWidth(0, MaxPossibleFontSize, "999", -1, -1.0f);
	if(Team == TEAM_SPECTATORS)
	{
		ScoreLength = 0;
		TeeSmallSpacing = 0;
		PingLength = 0;
	}
	float PingOffset = XEntries + WidthEntries - PingLength - SmallSpacing;
	float TeeOffset = TeeScoreOffset + ScoreLength + TeeSmallSpacing, TeeLength = LineHeight + TeeSmallSpacing * 2.0f;
	float CountryLength = LineHeight + SmallSpacingSize, CountryOffset = PingOffset - CountryLength;
	float NameOffset = TeeOffset + TeeLength;
	float LenNameAndClan = CountryOffset - NameOffset;
	float ClanLength = LenNameAndClan / 2.0f, ClanOffset = CountryOffset - ClanLength;
	float NameLength = LenNameAndClan / 2.0f;

	// render headlines
	const float ScoreboardHeadlineOffset = ms_TeamScoreboardGroupOffset;
	const float ScoreboardHeadlineMaxSize = ms_TeamScoreboardHeadlineHeightMax;
	const float ScoreboardHeadlineFontsize = 15.0f;
	const float ScoreboardHeadlineTextOffset = (ScoreboardHeadlineMaxSize - ScoreboardHeadlineFontsize) / 2.0f;
	y += TitleHeightMax + ScoreboardHeadlineOffset;

	float OverSize3D = 10;
	float Radius3D = 5;

	RenderTools()->DrawUIElRect(*m_ScoreboardGeneral.Get(UIRectIndex++), x, y, w, ScoreboardHeadlineMaxSize, gs_ColorHeader, 0, 0.0f);
	if(Team == TEAM_RED)
		RenderTools()->DrawUIElRect3D(*m_ScoreboardGeneral.Get(UIRectIndex++), x, y, -OverSize3D, ScoreboardHeadlineMaxSize, gs_ColorHeader, -Radius3D);
	else if(Team == TEAM_BLUE)
		RenderTools()->DrawUIElRect3D(*m_ScoreboardGeneral.Get(UIRectIndex++), x + w, y, OverSize3D, ScoreboardHeadlineMaxSize, gs_ColorHeader, Radius3D);

	TextRender()->TextColor(gs_ColorHeaderFont);
	TextRender()->TextOutlineColor(gs_ColorHeaderFontOutline);

	const char *pScore = Localize("Score");
	if(Team != TEAM_SPECTATORS)
		UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), TeeScoreOffset, y + ScoreboardHeadlineTextOffset, ScoreLength, ScoreboardHeadlineFontsize, pScore, ScoreboardHeadlineFontsize, 1);

	UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), NameOffset, y + ScoreboardHeadlineTextOffset, WidthEntries, ScoreboardHeadlineFontsize, Localize("Name"), ScoreboardHeadlineFontsize, -1);
	UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), ClanOffset, y + ScoreboardHeadlineTextOffset, WidthEntries, ScoreboardHeadlineFontsize, Localize("Clan"), ScoreboardHeadlineFontsize, -1);

	if(Team != TEAM_SPECTATORS)
		UI()->DoLabelStreamed(*m_ScoreboardGeneral.Get(UIRectIndex++), PingOffset, y + ScoreboardHeadlineTextOffset, WidthEntries, ScoreboardHeadlineFontsize, Localize("Ping"), ScoreboardHeadlineFontsize, -1);

	TextRender()->TextColor(TextRender()->DefaultTextColor());
	TextRender()->TextOutlineColor(TextRender()->DefaultTextOutlineColor());

	// render player entries
	const float ScoreboardEntriesOffset = ms_TeamScoreboardGroupOffset;
	y += ScoreboardHeadlineMaxSize + ScoreboardEntriesOffset;
	float YBeforeEntities = y;

	int RenderedEntities = 0;

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		// make sure that we render the correct team
		const CNetObj_PlayerInfo *pInfo = m_pClient->m_Snap.m_paInfoByDDTeamScore[i];
		if(!pInfo || pInfo->m_Team != Team)
			continue;

		float YEntries = y;

		int PlayerUIRectIndex = 0;
		ColorRGBA ColorInner(1.0f, 1.0f, 1.0f, 1.0f);
		// background so it's easy to find the local player or the followed one in spectator mode
		if(pInfo->m_Local || (m_pClient->m_Snap.m_SpecInfo.m_Active && pInfo->m_ClientID == m_pClient->m_Snap.m_SpecInfo.m_SpectatorID))
		{
			if(Team == TEAM_RED)
				ColorInner = gs_ColorEntryRed;
			else if(Team == TEAM_BLUE)
				ColorInner = gs_ColorEntryBlue;
			else if(Team == TEAM_SPECTATORS)
				ColorInner = gs_ColorEntryPurple;

			if(Team == TEAM_RED)
				RenderTools()->DrawUIElRect3D(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), XEntries - SmallSpacing, YEntries, -OverSize3D, LineHeight, ColorInner, -Radius3D, 0.35f);
			else if(Team == TEAM_BLUE)
				RenderTools()->DrawUIElRect3D(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), XEntries - SmallSpacing + WidthEntries + SmallSpacingSize, YEntries, OverSize3D, LineHeight, ColorInner, Radius3D, 0.35f);
			RenderTools()->DrawUIElRect(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), XEntries - SmallSpacing, YEntries, WidthEntries + SmallSpacingSize, LineHeight, ColorInner, 0, 0.0f);
		}

		const float LineInfoOffset = (LineHeight - FontSize) / 2.f;

		if(Team != TEAM_SPECTATORS)
		{
			// score
			str_format(aBuf, sizeof(aBuf), "%d", clamp(pInfo->m_Score, -999, 9999));
			UI()->DoLabelStreamed(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), TeeScoreOffset, YEntries + LineInfoOffset, ScoreLength, FontSize, aBuf, FontSize, 1);
		}

		// flag
		if(m_pClient->m_Snap.m_pGameInfoObj->m_GameFlags & GAMEFLAG_FLAGS &&
			m_pClient->m_Snap.m_pGameDataObj && (m_pClient->m_Snap.m_pGameDataObj->m_FlagCarrierRed == pInfo->m_ClientID || m_pClient->m_Snap.m_pGameDataObj->m_FlagCarrierBlue == pInfo->m_ClientID))
		{
			Graphics()->BlendNormal();
			if(m_pClient->m_Snap.m_pGameDataObj->m_FlagCarrierBlue == pInfo->m_ClientID)
				Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteFlagBlue);
			else
				Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteFlagRed);

			Graphics()->QuadsBegin();
			Graphics()->QuadsSetSubset(1, 0, 0, 1);

			float Size = LineHeight;
			IGraphics::CQuadItem QuadItem(TeeOffset + 0.0f, YEntries - 5.0f - SmallSpacing / 2.0f, Size / 2.0f, Size);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}

		// avatar
		CTeeRenderInfo TeeInfo = m_pClient->m_aClients[pInfo->m_ClientID].m_RenderInfo;
		TeeInfo.m_Size = LineHeight;
		vec2 TeeOffsetToMid;
		RenderTools()->GetRenderTeeOffsetToRenderedTee(CAnimState::GetIdle(), &TeeInfo, TeeOffsetToMid);
		RenderTools()->RenderTee(CAnimState::GetIdle(), &TeeInfo, EMOTE_NORMAL, vec2(1.0f, 0.0f), vec2(TeeOffset + TeeLength / 2, YEntries + LineHeight / 2) + TeeOffsetToMid);

		// name
		if(m_pClient->m_aClients[pInfo->m_ClientID].m_AuthLevel)
		{
			ColorRGBA Color = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClAuthedPlayerColor));
			TextRender()->TextColor(Color);
		}

		if(g_Config.m_ClShowIDs)
		{
			char aId[64] = "";
			str_format(aId, sizeof(aId), "%2d: %s", pInfo->m_ClientID, m_pClient->m_aClients[pInfo->m_ClientID].m_aName);
			UI()->DoLabelStreamed(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), NameOffset, YEntries + LineInfoOffset, WidthEntries, FontSize, aId, FontSize, -1, NameLength, 1, true);
		}
		else
		{
			UI()->DoLabelStreamed(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), NameOffset, YEntries + LineInfoOffset, WidthEntries, FontSize, m_pClient->m_aClients[pInfo->m_ClientID].m_aName, FontSize, -1, NameLength, 1, true);
		}

		// clan
		if(str_comp(m_pClient->m_aClients[pInfo->m_ClientID].m_aClan,
			   m_pClient->m_aClients[GameClient()->m_LocalIDs[g_Config.m_ClDummy]].m_aClan) == 0)
		{
			ColorRGBA Color = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClSameClanColor));
			TextRender()->TextColor(Color);
		}
		else
			TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);

		if(m_pClient->m_aClients[pInfo->m_ClientID].m_aClan[0] != '\0')
			UI()->DoLabelStreamed(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), ClanOffset, YEntries + LineInfoOffset, WidthEntries, FontSize, m_pClient->m_aClients[pInfo->m_ClientID].m_aClan, FontSize, -1, ClanLength, 1, true);

		TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);

		// country flag
		ColorRGBA Color(1.0f, 1.0f, 1.0f, 0.5f);
		m_pClient->m_pCountryFlags->Render(m_pClient->m_aClients[pInfo->m_ClientID].m_Country, &Color,
			CountryOffset, YEntries + (LineHeight - CountryLength / 2.0f) / 2.0f, CountryLength, CountryLength / 2.0f);

		if(Team != TEAM_SPECTATORS)
		{
			// ping
			if(g_Config.m_ClEnablePingColor)
			{
				ColorRGBA rgb = color_cast<ColorRGBA>(ColorHSLA((300.0f - clamp(pInfo->m_Latency, 0, 300)) / 1000.0f, 1.0f, 0.5f));
				TextRender()->TextColor(rgb);
			}
			str_format(aBuf, sizeof(aBuf), "%d", clamp(pInfo->m_Latency, 0, 999));
			UI()->DoLabelStreamed(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), PingOffset, YEntries + LineInfoOffset, PingLength, FontSize, aBuf, FontSize, 1, PingLength, 1, true);
		}

		TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);

		++RenderedEntities;
		bool PlayerCountReached = RenderedEntities == NumPlayersTeam;
		if(Team == TEAM_SPECTATORS)
		{
			PlayerCountReached = RenderedEntities == NumPlayersSpectator;
		}

		if(RenderedEntities >= 32 || (Team == TEAM_SPECTATORS && RenderedEntities >= 24) || PlayerCountReached)
			break;
		y += LineHeightFull;
		ColorInner = ColorRGBA(0.25f, 0.25f, 0.25f, 0.15f);
		RenderTools()->DrawUIElRect(*m_ScoreboardPlayers[i].Get(PlayerUIRectIndex++), XEntries - SmallSpacing, y, WidthEntries + SmallSpacingSize, Spacing, ColorInner, 0, 0.0f);
		y += Spacing;
	}

	y = YBeforeEntities + ScoreboardRealEntitiesFullHeight;

	const float ScoreboardFootlineOffset = ms_TeamScoreboardGroupOffset;
	const float ScoreboardFootlineMaxSize = ms_TeamScoreboardFootlineHeightMax;
	const float ScoreboardFootlineFontsize = ScoreboardHeadlineFontsize;
	const float ScoreboardFootlineTextOffset = (ScoreboardFootlineMaxSize - ScoreboardFootlineFontsize) / 2.0f;
	y += ScoreboardFootlineOffset;

	if(Team != TEAM_SPECTATORS)
	{
		RenderTools()->DrawUIElRect(*m_ScoreboardGeneral.Get(UIRectIndex++), x, y, w, ScoreboardHeadlineMaxSize, gs_ColorFeet, 0, 0.0f);
	}

	if(m_pClient->m_Snap.m_pGameInfoObj)
	{
		TextRender()->TextColor(gs_ColorFeetFont);
		TextRender()->TextOutlineColor(gs_ColorFeetFontOutline);
		if(Team == TEAM_RED)
		{
			char aBuf[64];
			if(m_pClient->m_Snap.m_pGameInfoObj->m_ScoreLimit)
			{
				str_format(aBuf, sizeof(aBuf), "%s: %d", Localize("Score limit"), m_pClient->m_Snap.m_pGameInfoObj->m_ScoreLimit);
				UI()->DoTextLabel(XEntries, y + ScoreboardFootlineTextOffset, WidthEntries, ScoreboardFootlineFontsize, aBuf, ScoreboardFootlineFontsize, -1);
			}
			if(m_pClient->m_Snap.m_pGameInfoObj->m_TimeLimit)
			{
				char aBuf[64];
				str_format(aBuf, sizeof(aBuf), Localize("Time limit: %d min"), m_pClient->m_Snap.m_pGameInfoObj->m_TimeLimit);
				UI()->DoTextLabel(XEntries, y + ScoreboardFootlineTextOffset, WidthEntries, ScoreboardFootlineFontsize, aBuf, ScoreboardFootlineFontsize, 1);
			}
		}
		else if(Team == TEAM_BLUE)
		{
			str_format(aBuf, sizeof(aBuf), "%s: %s", Localize("Map"), Client()->GetCurrentMap());
			UI()->DoTextLabel(XEntries, y + ScoreboardFootlineTextOffset, WidthEntries, ScoreboardFootlineFontsize, aBuf, ScoreboardFootlineFontsize, -1);
			if(m_pClient->m_Snap.m_pGameInfoObj->m_RoundNum && m_pClient->m_Snap.m_pGameInfoObj->m_RoundCurrent)
			{
				char aBuf[64];
				str_format(aBuf, sizeof(aBuf), "%s %d/%d", Localize("Round"), m_pClient->m_Snap.m_pGameInfoObj->m_RoundCurrent, m_pClient->m_Snap.m_pGameInfoObj->m_RoundNum);
				UI()->DoTextLabel(XEntries, y + ScoreboardFootlineTextOffset, WidthEntries, ScoreboardFootlineFontsize, aBuf, ScoreboardFootlineFontsize, 1);
			}
		}
		TextRender()->TextColor(TextRender()->DefaultTextColor());
		TextRender()->TextOutlineColor(TextRender()->DefaultTextOutlineColor());
	}
}
