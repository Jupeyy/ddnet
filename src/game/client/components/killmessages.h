/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_KILLMESSAGES_H
#define GAME_CLIENT_COMPONENTS_KILLMESSAGES_H
#include <engine/shared/protocol.h>
#include <game/client/component.h>

#include <game/client/render.h>
#include <list>

struct SKillMessagesExtraInfo
{
	char m_aLastPlayerName[64];
	int m_Kills;
};

class CKillMessages : public CComponent
{
	SKillMessagesExtraInfo m_aClients[MAX_CLIENTS];

	int m_SpriteQuadContainerIndex;

public:
	// kill messages
	struct CKillMsg
	{
		CKillMsg()
		{
			m_KillerTextContainerIndex = m_VictimTextContainerIndex = -1;
		}

		int m_Weapon;

		int m_VictimID;
		int m_VictimTeam;
		int m_VictimDDTeam;
		char m_aVictimName[64];
		int m_VictimTextContainerIndex;
		float m_VitctimTextWidth;
		CTeeRenderInfo m_VictimRenderInfo;

		int m_KillerID;
		int m_KillerTeam;
		char m_aKillerName[64];
		int m_KillerTextContainerIndex;
		float m_KillerTextWidth;
		CTeeRenderInfo m_KillerRenderInfo;

		int m_ModeSpecial; // for CTF, if the guy is carrying a flag for example
		int m_Tick;
		int m_FlagCarrierBlue;
	};

	struct CKillTracker
	{
		int m_ClientID;
		int m_TextContainer;
		int m_LastKills;
		float m_TextWidth;
	};

	enum
	{
		MAX_KILLMSGS = 5,
	};

private:
	void CreateKillmessageNamesIfNotCreated(CKillMsg &Kill);

public:
	CKillMsg m_aKillmsgs[MAX_KILLMSGS];
	int m_KillmsgCurrent;

	typedef std::list<CKillTracker> TTrackList;
	TTrackList m_CurKillTrackers;

	virtual int Sizeof() const override { return sizeof(*this); }
	virtual void OnWindowResize() override;
	virtual void OnReset() override;
	virtual void OnRender() override;
	virtual void OnMessage(int MsgType, void *pRawMsg) override;
	virtual void OnInit() override;

	void OnMapLoad() override;

	void RefindSkins();
};

#endif
