/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>
#include <game/generated/client_data.h>
#include <game/generated/protocol.h>

#include "engine/shared/protocol.h"
#include "game/client/components/players.h"
#include "killmessages.h"
#include <game/client/animstate.h>
#include <game/client/gameclient.h>

#include <game/client/components/sounds.h>

void CKillMessages::OnWindowResize()
{
	for(auto &Killmsg : m_aKillmsgs)
	{
		if(Killmsg.m_VictimTextContainerIndex != -1)
			TextRender()->DeleteTextContainer(Killmsg.m_VictimTextContainerIndex);
		if(Killmsg.m_KillerTextContainerIndex != -1)
			TextRender()->DeleteTextContainer(Killmsg.m_KillerTextContainerIndex);
		Killmsg.m_VictimTextContainerIndex = Killmsg.m_KillerTextContainerIndex = -1;
	}

	for(auto &KillTracker : m_CurKillTrackers)
	{
		if(KillTracker.m_TextContainer != -1)
		{
			TextRender()->DeleteTextContainer(KillTracker.m_TextContainer);
			KillTracker.m_TextContainer = -1;
			KillTracker.m_LastKills = 0;
		}
	}
}

void CKillMessages::OnReset()
{
	m_KillmsgCurrent = 0;
	for(auto &Killmsg : m_aKillmsgs)
	{
		Killmsg.m_Tick = -100000;

		if(Killmsg.m_VictimTextContainerIndex != -1)
			TextRender()->DeleteTextContainer(Killmsg.m_VictimTextContainerIndex);

		if(Killmsg.m_KillerTextContainerIndex != -1)
			TextRender()->DeleteTextContainer(Killmsg.m_KillerTextContainerIndex);

		Killmsg.m_VictimTextContainerIndex = Killmsg.m_KillerTextContainerIndex = -1;
	}
}

void CKillMessages::OnInit()
{
	Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);
	m_SpriteQuadContainerIndex = Graphics()->CreateQuadContainer();

	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	RenderTools()->QuadContainerAddSprite(m_SpriteQuadContainerIndex, 0.f, 0.f, 28.f, 56.f);
	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	RenderTools()->QuadContainerAddSprite(m_SpriteQuadContainerIndex, 0.f, 0.f, 28.f, 56.f);

	Graphics()->QuadsSetSubset(1, 0, 0, 1);
	RenderTools()->QuadContainerAddSprite(m_SpriteQuadContainerIndex, 0.f, 0.f, 28.f, 56.f);
	Graphics()->QuadsSetSubset(1, 0, 0, 1);
	RenderTools()->QuadContainerAddSprite(m_SpriteQuadContainerIndex, 0.f, 0.f, 28.f, 56.f);

	for(int i = 0; i < NUM_WEAPONS; ++i)
	{
		float ScaleX, ScaleY;
		Graphics()->QuadsSetSubset(0, 0, 1, 1);
		RenderTools()->GetSpriteScale(g_pData->m_Weapons.m_aId[i].m_pSpriteBody, ScaleX, ScaleY);
		RenderTools()->QuadContainerAddSprite(m_SpriteQuadContainerIndex, 96.f * ScaleX, 96.f * ScaleY);
	}
}

void CKillMessages::OnMapLoad()
{
	mem_zero(m_aClients, sizeof(m_aClients));
	m_CurKillTrackers.clear();
}

void CKillMessages::CreateKillmessageNamesIfNotCreated(CKillMsg &Kill)
{
	const float FontSize = 36.0f;
	if(Kill.m_VictimTextContainerIndex == -1 && Kill.m_aVictimName[0] != 0)
	{
		Kill.m_VitctimTextWidth = TextRender()->TextWidth(0, FontSize, Kill.m_aVictimName, -1, -1.0f);

		CTextCursor Cursor;
		TextRender()->SetCursor(&Cursor, 0, 0, FontSize, TEXTFLAG_RENDER);
		Cursor.m_LineWidth = -1;

		Kill.m_VictimTextContainerIndex = TextRender()->CreateTextContainer(&Cursor, Kill.m_aVictimName);
	}

	if(Kill.m_KillerTextContainerIndex == -1 && Kill.m_aKillerName[0] != 0)
	{
		Kill.m_KillerTextWidth = TextRender()->TextWidth(0, FontSize, Kill.m_aKillerName, -1, -1.0f);

		CTextCursor Cursor;
		TextRender()->SetCursor(&Cursor, 0, 0, FontSize, TEXTFLAG_RENDER);
		Cursor.m_LineWidth = -1;

		Kill.m_KillerTextContainerIndex = TextRender()->CreateTextContainer(&Cursor, Kill.m_aKillerName);
	}
}

void CKillMessages::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_KILLMSG)
	{
		CNetMsg_Sv_KillMsg *pMsg = (CNetMsg_Sv_KillMsg *)pRawMsg;

		// unpack messages
		CKillMsg Kill;
		Kill.m_aVictimName[0] = '\0';
		Kill.m_aKillerName[0] = '\0';

		Kill.m_VictimID = pMsg->m_Victim;
		if(Kill.m_VictimID >= 0 && Kill.m_VictimID < MAX_CLIENTS)
		{
			Kill.m_VictimTeam = m_pClient->m_aClients[Kill.m_VictimID].m_Team;
			Kill.m_VictimDDTeam = m_pClient->m_Teams.Team(Kill.m_VictimID);
			str_copy(Kill.m_aVictimName, m_pClient->m_aClients[Kill.m_VictimID].m_aName, sizeof(Kill.m_aVictimName));
			Kill.m_VictimRenderInfo = m_pClient->m_aClients[Kill.m_VictimID].m_RenderInfo;
		}

		Kill.m_KillerID = pMsg->m_Killer;
		if(Kill.m_KillerID >= 0 && Kill.m_KillerID < MAX_CLIENTS)
		{
			Kill.m_KillerTeam = m_pClient->m_aClients[Kill.m_KillerID].m_Team;
			str_copy(Kill.m_aKillerName, m_pClient->m_aClients[Kill.m_KillerID].m_aName, sizeof(Kill.m_aKillerName));
			Kill.m_KillerRenderInfo = m_pClient->m_aClients[Kill.m_KillerID].m_RenderInfo;
		}

		Kill.m_Weapon = pMsg->m_Weapon;
		Kill.m_ModeSpecial = pMsg->m_ModeSpecial;
		Kill.m_Tick = Client()->GameTick(g_Config.m_ClDummy);

		Kill.m_FlagCarrierBlue = m_pClient->m_Snap.m_pGameDataObj ? m_pClient->m_Snap.m_pGameDataObj->m_FlagCarrierBlue : -1;

		Kill.m_VitctimTextWidth = Kill.m_KillerTextWidth = 0.f;

		float Width = 400 * 3.0f * Graphics()->ScreenAspect();
		float Height = 400 * 3.0f;

		float ScreenX0, ScreenY0, ScreenX1, ScreenY1;
		Graphics()->GetScreen(&ScreenX0, &ScreenY0, &ScreenX1, &ScreenY1);
		Graphics()->MapScreen(0, 0, Width * 1.5f, Height * 1.5f);

		CreateKillmessageNamesIfNotCreated(Kill);

		bool KillMsgValid = (Kill.m_VictimRenderInfo.m_CustomColoredSkin && Kill.m_VictimRenderInfo.m_ColorableRenderSkin.m_Body != -1) || (!Kill.m_VictimRenderInfo.m_CustomColoredSkin && Kill.m_VictimRenderInfo.m_OriginalRenderSkin.m_Body != -1);
		// if killer != victim, killer must be valid too
		KillMsgValid &= Kill.m_KillerID == Kill.m_VictimID || ((Kill.m_KillerRenderInfo.m_CustomColoredSkin && Kill.m_KillerRenderInfo.m_ColorableRenderSkin.m_Body != -1) || (!Kill.m_KillerRenderInfo.m_CustomColoredSkin && Kill.m_KillerRenderInfo.m_OriginalRenderSkin.m_Body != -1));
		if(KillMsgValid)
		{
			// add the message
			m_KillmsgCurrent = (m_KillmsgCurrent + 1) % MAX_KILLMSGS;

			if(m_aKillmsgs[m_KillmsgCurrent].m_VictimTextContainerIndex != -1)
			{
				TextRender()->DeleteTextContainer(m_aKillmsgs[m_KillmsgCurrent].m_VictimTextContainerIndex);
				m_aKillmsgs[m_KillmsgCurrent].m_VictimTextContainerIndex = -1;
			}

			if(m_aKillmsgs[m_KillmsgCurrent].m_KillerTextContainerIndex != -1)
			{
				TextRender()->DeleteTextContainer(m_aKillmsgs[m_KillmsgCurrent].m_KillerTextContainerIndex);
				m_aKillmsgs[m_KillmsgCurrent].m_KillerTextContainerIndex = -1;
			}

			m_aKillmsgs[m_KillmsgCurrent] = Kill;
		}

		Graphics()->MapScreen(ScreenX0, ScreenY0, ScreenX1, ScreenY1);

		if(Kill.m_KillerID >= 0 && Kill.m_Weapon == WEAPON_LASER)
		{
			if(str_comp(m_aClients[Kill.m_KillerID].m_aLastPlayerName, Kill.m_aKillerName) != 0)
			{
				str_copy(m_aClients[Kill.m_KillerID].m_aLastPlayerName, Kill.m_aKillerName, sizeof(m_aClients[Kill.m_KillerID].m_aLastPlayerName));
				m_aClients[Kill.m_KillerID].m_Kills = 0;
			}

			++m_aClients[Kill.m_KillerID].m_Kills;

			if(m_aClients[Kill.m_KillerID].m_Kills == 5)
			{
				CKillTracker KillTrack{};
				KillTrack.m_ClientID = Kill.m_KillerID;
				KillTrack.m_TextContainer = -1;
				m_CurKillTrackers.push_back(KillTrack);
			}

			if(Kill.m_KillerID == GameClient()->m_Snap.m_LocalClientID)
			{
				if(m_aClients[Kill.m_KillerID].m_Kills == 5)
					Sound()->Play(CSounds::CHN_QUAKE, g_aQuakeSounds[QUAKE_SOUND_RAMPAGE], 0);
				else if(m_aClients[Kill.m_KillerID].m_Kills == 10)
					Sound()->Play(CSounds::CHN_QUAKE, g_aQuakeSounds[QUAKE_SOUND_KILLINGSPREE], 0);
				else if(m_aClients[Kill.m_KillerID].m_Kills == 15)
					Sound()->Play(CSounds::CHN_QUAKE, g_aQuakeSounds[QUAKE_SOUND_UNSTOPPABLE], 0);
				else if(m_aClients[Kill.m_KillerID].m_Kills == 20)
					Sound()->Play(CSounds::CHN_QUAKE, g_aQuakeSounds[QUAKE_SOUND_DOMINATING], 0);
				else if(m_aClients[Kill.m_KillerID].m_Kills == 25)
					Sound()->Play(CSounds::CHN_QUAKE, g_aQuakeSounds[QUAKE_SOUND_WHICKEDSICK], 0);
				else if(m_aClients[Kill.m_KillerID].m_Kills == 30)
					Sound()->Play(CSounds::CHN_QUAKE, g_aQuakeSounds[QUAKE_SOUND_GODLIKE], 0);
			}
		}
		if(Kill.m_VictimID >= 0 && Kill.m_Weapon == WEAPON_LASER)
		{
			m_aClients[Kill.m_VictimID].m_Kills = 0;
		}
	}
}

void CKillMessages::OnRender()
{
	if(!g_Config.m_ClShowKillMessages)
		return;

	float Width = 400 * 3.0f * Graphics()->ScreenAspect();
	float Height = 400 * 3.0f;

	Graphics()->MapScreen(0, 0, Width * 1.5f, Height * 1.5f);
	Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);

	float StartX = Width * 1.5f - 10.0f;
	float y = 30.0f + 100.0f * ((g_Config.m_ClShowfps ? 1 : 0) + g_Config.m_ClShowpred);

	for(int i = 1; i <= MAX_KILLMSGS; i++)
	{
		int r = (m_KillmsgCurrent + i) % MAX_KILLMSGS;
		if(Client()->GameTick(g_Config.m_ClDummy) > m_aKillmsgs[r].m_Tick + 50 * 10)
			continue;

		float x = StartX;

		STextRenderColor TColor(1.f, 1.f, 1.f, 1.f);
		STextRenderColor TOutlineColor(0.f, 0.f, 0.f, 0.3f);

		// render victim name
		x -= m_aKillmsgs[r].m_VitctimTextWidth;
		if(m_aKillmsgs[r].m_VictimID >= 0 && g_Config.m_ClChatTeamColors && m_aKillmsgs[r].m_VictimDDTeam)
		{
			ColorRGBA rgb = color_cast<ColorRGBA>(ColorHSLA(m_aKillmsgs[r].m_VictimDDTeam / 64.0f, 1.0f, 0.75f));
			TColor.Set(rgb.r, rgb.g, rgb.b, 1.0f);
		}

		CreateKillmessageNamesIfNotCreated(m_aKillmsgs[r]);

		if(m_aKillmsgs[r].m_VictimTextContainerIndex != -1)
			TextRender()->RenderTextContainer(m_aKillmsgs[r].m_VictimTextContainerIndex, &TColor, &TOutlineColor, x, y + (46.f - 36.f) / 2.f);

		// render victim tee
		x -= 24.0f;

		if(m_pClient->m_Snap.m_pGameInfoObj && m_pClient->m_Snap.m_pGameInfoObj->m_GameFlags & GAMEFLAG_FLAGS)
		{
			if(m_aKillmsgs[r].m_ModeSpecial & 1)
			{
				int QuadOffset = 0;
				if(m_aKillmsgs[r].m_VictimID == m_aKillmsgs[r].m_FlagCarrierBlue)
					++QuadOffset;

				if(QuadOffset == 0)
					Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteFlagRed);
				else
					Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteFlagBlue);

				Graphics()->RenderQuadContainerAsSprite(m_SpriteQuadContainerIndex, QuadOffset, x, y - 16);
			}
		}

		if(m_aKillmsgs[r].m_VictimID >= 0)
			RenderTools()->RenderTee(CAnimState::GetIdle(), &m_aKillmsgs[r].m_VictimRenderInfo, EMOTE_PAIN, vec2(-1, 0), vec2(x, y + 28));
		x -= 32.0f;

		// render weapon
		x -= 44.0f;
		if(m_aKillmsgs[r].m_Weapon >= 0)
		{
			Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteWeapons[m_aKillmsgs[r].m_Weapon]);
			Graphics()->RenderQuadContainerAsSprite(m_SpriteQuadContainerIndex, 4 + m_aKillmsgs[r].m_Weapon, x, y + 28);
		}
		x -= 52.0f;

		if(m_aKillmsgs[r].m_VictimID != m_aKillmsgs[r].m_KillerID)
		{
			if(m_pClient->m_Snap.m_pGameInfoObj && m_pClient->m_Snap.m_pGameInfoObj->m_GameFlags & GAMEFLAG_FLAGS)
			{
				if(m_aKillmsgs[r].m_ModeSpecial & 2)
				{
					int QuadOffset = 2;
					if(m_aKillmsgs[r].m_KillerID == m_aKillmsgs[r].m_FlagCarrierBlue)
						++QuadOffset;

					if(QuadOffset == 2)
						Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteFlagRed);
					else
						Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteFlagBlue);

					Graphics()->RenderQuadContainerAsSprite(m_SpriteQuadContainerIndex, QuadOffset, x - 56, y - 16);
				}
			}

			// render killer tee
			x -= 24.0f;
			if(m_aKillmsgs[r].m_KillerID >= 0)
				RenderTools()->RenderTee(CAnimState::GetIdle(), &m_aKillmsgs[r].m_KillerRenderInfo, EMOTE_ANGRY, vec2(1, 0), vec2(x, y + 28));
			x -= 32.0f;

			// render killer name
			x -= m_aKillmsgs[r].m_KillerTextWidth;

			if(m_aKillmsgs[r].m_KillerTextContainerIndex != -1)
				TextRender()->RenderTextContainer(m_aKillmsgs[r].m_KillerTextContainerIndex, &TColor, &TOutlineColor, x, y + (46.f - 36.f) / 2.f);
		}

		y += 46.0f;
	}

	y += 20.0f;

	auto &&CleanupKillTrack = [=](CKillTracker &KillTrack) -> void {
		if(KillTrack.m_TextContainer != -1)
		{
			TextRender()->DeleteTextContainer(KillTrack.m_TextContainer);
			KillTrack.m_TextContainer = -1;
			KillTrack.m_LastKills = 0;
		}
	};

	auto &&GetTrackName = [](int Kills) -> const char * {
		if(Kills >= 30)
			return "GODLIKE";
		else if(Kills >= 25)
			return "WHICKEDSICK";
		else if(Kills >= 20)
			return "DOMINATING";
		else if(Kills >= 15)
			return "UNSTOPPABLE";
		else if(Kills >= 10)
			return "KILLING SPREE";
		else if(Kills >= 5)
			return "RAMPAGE";

		return "";
	};

	auto &&GetTrackNamePre = [](int Kills) -> const char * {
		if(Kills >= 30)
			return "is ";
		else if(Kills >= 25)
			return "is ";
		else if(Kills >= 20)
			return "is ";
		else if(Kills >= 15)
			return "is ";
		else if(Kills >= 10)
			return "has a ";
		else if(Kills >= 5)
			return "is on a ";

		return "";
	};

	auto &&GetTrackColor = [](int Kills) -> STextRenderColor {
		if(Kills >= 30)
			return STextRenderColor{0.921, 0.847, 0, 1};
		else if(Kills >= 25)
			return STextRenderColor{0x51 / 255.f, 0x1d / 255.f, 0x87 / 255.f, 1};
		else if(Kills >= 20)
			return STextRenderColor{0.721, 0, 0.090, 1};
		else if(Kills >= 15)
			return STextRenderColor{0x32 / 255.f, 0x5e / 255.f, 0xdf / 255.f, 1};
		else if(Kills >= 10)
			return STextRenderColor{0.278, 0.721, 0, 1};
		else if(Kills >= 5)
			return STextRenderColor{0x74 / 255.f, 0xd1 / 255.f, 0xc4 / 255.f, 1};

		return STextRenderColor{1, 0, 0, 1};
	};

	for(TTrackList::iterator it = m_CurKillTrackers.begin(); it != m_CurKillTrackers.end();)
	{
		if(it->m_ClientID >= 0 && it->m_ClientID < MAX_CLIENTS)
		{
			const CNetObj_PlayerInfo *pInfo = m_pClient->m_Snap.m_paPlayerInfos[it->m_ClientID];
			SKillMessagesExtraInfo &Extra = m_aClients[it->m_ClientID];
			if(!pInfo || Extra.m_Kills < 5)
			{
				CleanupKillTrack(*it);
				it = m_CurKillTrackers.erase(it);
				continue;
			}

			CGameClient::CClientData &ClientData = GameClient()->m_aClients[it->m_ClientID];

			bool DoCreateText = false;
			if(it->m_TextContainer == -1)
			{
				DoCreateText = true;
				it->m_LastKills = Extra.m_Kills;
			}

			if(Extra.m_Kills != it->m_LastKills)
			{
				TextRender()->DeleteTextContainer(it->m_TextContainer);
				DoCreateText = true;
				it->m_LastKills = Extra.m_Kills;
			}

			if(DoCreateText)
			{
				char aTmpBuff[1024];
				CTextCursor TmpCursor;
				TextRender()->SetCursor(&TmpCursor, 0, 0, 36, 0);
				it->m_TextWidth = TextRender()->TextWidth(NULL, 36, ClientData.m_aName, -1, -1);
				str_format(aTmpBuff, sizeof(aTmpBuff), " %s", GetTrackNamePre(it->m_LastKills));
				it->m_TextWidth += TextRender()->TextWidth(NULL, 36, aTmpBuff, -1, -1);
				str_format(aTmpBuff, sizeof(aTmpBuff), "%s", GetTrackName(it->m_LastKills));
				it->m_TextWidth += TextRender()->TextWidth(NULL, 36, aTmpBuff, -1, -1);
				str_format(aTmpBuff, sizeof(aTmpBuff), " with %d kills", it->m_LastKills);
				it->m_TextWidth += TextRender()->TextWidth(NULL, 36, aTmpBuff, -1, -1);

				int CurRenderFlags = TextRender()->GetRenderFlags();
				TextRender()->SetRenderFlags(ETextRenderFlags::TEXT_RENDER_FLAG_NO_AUTOMATIC_QUAD_UPLOAD | ETextRenderFlags::TEXT_RENDER_FLAG_NO_FIRST_CHARACTER_X_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_LAST_CHARACTER_ADVANCE);
				TextRender()->SetCursor(&TmpCursor, StartX - it->m_TextWidth, 0, 36, 0);
				it->m_TextContainer = TextRender()->CreateTextContainer(&TmpCursor, ClientData.m_aName);
				STextRenderColor TColor = GetTrackColor(it->m_LastKills);
				str_format(aTmpBuff, sizeof(aTmpBuff), " %s", GetTrackNamePre(it->m_LastKills));
				TextRender()->AppendTextContainer(&TmpCursor, it->m_TextContainer, aTmpBuff);

				TextRender()->TextColor(TColor.m_R, TColor.m_G, TColor.m_B, TColor.m_A);
				str_format(aTmpBuff, sizeof(aTmpBuff), "%s", GetTrackName(it->m_LastKills));
				TextRender()->AppendTextContainer(&TmpCursor, it->m_TextContainer, aTmpBuff);
				TextRender()->TextColor(TextRender()->DefaultTextColor().r, TextRender()->DefaultTextColor().g, TextRender()->DefaultTextColor().b, TextRender()->DefaultTextColor().a);

				str_format(aTmpBuff, sizeof(aTmpBuff), " with %d kills", it->m_LastKills);
				TextRender()->AppendTextContainer(&TmpCursor, it->m_TextContainer, aTmpBuff);

				TextRender()->UploadTextContainer(it->m_TextContainer);
				TextRender()->SetRenderFlags(CurRenderFlags);
			}

			STextRenderColor TextColor = TextRender()->DefaultTextColor();
			STextRenderColor TextOutlineColor = TextRender()->DefaultTextOutlineColor();
			TextRender()->RenderTextContainer(it->m_TextContainer, &TextColor, &TextOutlineColor, 0, y);

			if(GameClient()->m_aClients[it->m_ClientID].m_RenderInfo.m_OriginalRenderSkin.m_Body != -1)
			{
				RenderTools()->RenderTee(CAnimState::GetIdle(), &GameClient()->m_aClients[it->m_ClientID].m_RenderInfo, EMOTE_ANGRY, vec2(1, 0), vec2(StartX - 32 - it->m_TextWidth, y + 28));
			}

			y += 36 + 15;
		}
		else
		{
			CleanupKillTrack(*it);
			it = m_CurKillTrackers.erase(it);
			continue;
		}
		++it;
	}
}

void CKillMessages::RefindSkins()
{
	for(int i = 0; i < MAX_KILLMSGS; i++)
	{
		int r = i % MAX_KILLMSGS;
		if(Client()->GameTick(g_Config.m_ClDummy) > m_aKillmsgs[r].m_Tick + 50 * 10)
			continue;

		if(m_aKillmsgs[r].m_KillerID >= 0)
		{
			CGameClient::CClientData &Client = GameClient()->m_aClients[m_aKillmsgs[r].m_KillerID];
			if(Client.m_aSkinName[0] != '\0')
				m_aKillmsgs[r].m_KillerRenderInfo = Client.m_RenderInfo;
			else
				m_aKillmsgs[r].m_KillerID = -1;
		}

		if(m_aKillmsgs[r].m_VictimID >= 0)
		{
			CGameClient::CClientData &Client = GameClient()->m_aClients[m_aKillmsgs[r].m_VictimID];
			if(Client.m_aSkinName[0] != '\0')
				m_aKillmsgs[r].m_VictimRenderInfo = Client.m_RenderInfo;
			else
				m_aKillmsgs[r].m_VictimID = -1;
		}
	}
}
