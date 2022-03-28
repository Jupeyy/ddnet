#include "matecam.h"
#include "engine/shared/config.h"

#include <game/client/gameclient.h>

void CMateCam::OnRender()
{
	if(g_Config.m_ClMateCamSize > 0 && Client()->DummyConnected() && GameClient()->GetDummyID() >= 0 && m_pClient->m_Snap.m_aCharacters[GameClient()->GetDummyID()].m_Active)
	{
		float W = Graphics()->ScreenWidth();
		float H = Graphics()->ScreenHeight();
		float MateCamSize = g_Config.m_ClMateCamSize / 100.0f;
		// set to dummy viewport
		Graphics()->UpdateViewport((1.0f - MateCamSize) * W, 0, W * MateCamSize, H * MateCamSize, false);

		Graphics()->MapScreen(0, 0, 1, 1);

		ColorRGBA ClearClr = Client()->GetClearColor();

		Graphics()->TextureClear();
		Graphics()->QuadsBegin();
		Graphics()->SetColor(ClearClr.r, ClearClr.g, ClearClr.b, 1.0f);
		IGraphics::CQuadItem QuadItem(0, 0, 1, 1);
		Graphics()->QuadsDrawTL(&QuadItem, 1);
		Graphics()->QuadsEnd();

		auto CurCenter = GameClient()->m_Camera.m_Center;
		GameClient()->m_Camera.m_Center = m_pClient->m_Snap.m_aCharacters[GameClient()->GetDummyID()].m_Position;

		GameClient()->m_MapLayersBackGround.OnRender();

		GameClient()->m_Players.OnRender();

		GameClient()->m_MapLayersForeGround.OnRender();

		GameClient()->m_Camera.m_Center = CurCenter;
		// reset viewport to default
		Graphics()->UpdateViewport(0, 0, W, H, false);
	}
}
