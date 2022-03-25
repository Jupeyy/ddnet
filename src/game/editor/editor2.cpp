#include "editor2.h"
#include "base/system.h"
#include "engine/keys.h"
#include "engine/shared/config.h"
#include "game/client/ui.h"
#include "game/layers.h"

#include <cstdint>
#include <game/client/components/mapimages.h>

#include <engine/client.h>
#include <engine/sound.h>
#include <engine/storage.h>
#include <engine/textrender.h>

#ifdef CONF_WEBASM
#include <emscripten.h>
#endif

IEditor *CreateEditor() { return new CEditor2; }

void CEditor2::Init()
{
	m_pInput = Kernel()->RequestInterface<IInput>();
	m_pClient = Kernel()->RequestInterface<IClient>();
	m_pConfig = Kernel()->RequestInterface<IConfigManager>()->Values();
	m_pConsole = Kernel()->RequestInterface<IConsole>();
	m_pGraphics = Kernel()->RequestInterface<IGraphics>();
	m_pTextRender = Kernel()->RequestInterface<ITextRender>();
	m_pStorage = Kernel()->RequestInterface<IStorage>();
	m_pSound = Kernel()->RequestInterface<ISound>();
	CGameClient *pGameClient = (CGameClient *)Kernel()->RequestInterface<IGameClient>();
	m_Map.m_pClient = pGameClient;
	m_Map.OnInit();

	m_UI.Init(m_pGraphics, m_pTextRender);
	m_RenderTools.Init(m_pGraphics, &m_UI, pGameClient);
	UIEx()->Init(UI(), Kernel(), RenderTools(), Input()->GetEventsRaw(), Input()->GetEventCountRaw());
}

void CEditor2::UpdateAndRender()
{
	Graphics()->Clear(0.5f, 0.5f, 0.5f);

	m_Map.Render();

	static float s_MouseX = 0.0f;
	static float s_MouseY = 0.0f;
	int Buttons = 0;

	UI()->MapScreen();

	// handle mouse movement
	{
		float mx, my, Mwx, Mwy;
		int rx = 0, ry = 0;
		{
			Input()->MouseModeAbsolute();
			Input()->NativeMousePos(&rx, &ry);
			UIEx()->ResetMouseSlow();

			m_MouseDeltaX = rx - s_MouseX;
			m_MouseDeltaY = ry - s_MouseY;

			s_MouseX = clamp<float>(rx, 0.0f, Graphics()->WindowWidth());
			s_MouseY = clamp<float>(ry, 0.0f, Graphics()->WindowHeight());

			// update the ui
			mx = UI()->Screen()->w * ((float)s_MouseX / Graphics()->WindowWidth());
			my = UI()->Screen()->h * ((float)s_MouseY / Graphics()->WindowHeight());
			Mwx = 0;
			Mwy = 0;

			// fix correct world x and y
			/*CLayerGroup *g = GetSelectedGroup();
		if(g)
		{
			float aPoints[4];
			g->Mapping(aPoints);

			float WorldWidth = aPoints[2] - aPoints[0];
			float WorldHeight = aPoints[3] - aPoints[1];

			Mwx = aPoints[0] + WorldWidth * ((float)s_MouseX / Graphics()->WindowWidth());
			Mwy = aPoints[1] + WorldHeight * ((float)s_MouseY / Graphics()->WindowHeight());

			m_MouseDeltaWx = m_MouseDeltaX * (WorldWidth / Graphics()->ScreenWidth());
			m_MouseDeltaWy = m_MouseDeltaY * (WorldHeight / Graphics()->ScreenHeight());
		}*/
			if(Input()->KeyIsPressed(KEY_MOUSE_1))
				Buttons |= 1;
			if(Input()->KeyIsPressed(KEY_MOUSE_2))
				Buttons |= 2;
			if(Input()->KeyIsPressed(KEY_MOUSE_3))
				Buttons |= 4;

			UI()->Update(mx, my, Mwx, Mwy, Buttons);
		}
	}

	// render beautiful mouse cursor
	/*
	float mx = UI()->MouseX();
	float my = UI()->MouseY();
	Graphics()->WrapClamp();
	Graphics()->TextureSet(m_CursorTexture);
	Graphics()->QuadsBegin();
	IGraphics::CQuadItem QuadItem(mx, my, 16.0f, 16.0f);
	Graphics()->QuadsDrawTL(&QuadItem, 1);
	Graphics()->QuadsEnd();
	Graphics()->WrapNormal();
	*/

	const float OptionBarSize = 40.0f;
	int Zoom = 0;

	// Determines in which direction to zoom.
	if(m_Map.IsLoaded())
	{
		if(Input()->KeyPress(KEY_MOUSE_WHEEL_UP))
			Zoom--;
		if(Input()->KeyPress(KEY_MOUSE_WHEEL_DOWN))
			Zoom++;
	}

	UI()->MapScreen();
	CUIRect OptionBar = *UI()->Screen();

	OptionBar.HSplitTop(OptionBarSize, &OptionBar, nullptr);

	m_RenderTools.DrawUIRect(&OptionBar, ColorRGBA{0.6, 0.6, 0.8, 0.75}, 0, 5);

	typedef std::function<bool()> TBtnColorFunc;

	auto DoALabel = [&](const char *pLabelText, auto &&FuncClick, TBtnColorFunc &&FuncBtnColor, float SplitW = 150.0f) {
		CUIRect Label;
		OptionBar.VSplitLeft(SplitW, &Label, &OptionBar);

		m_RenderTools.DrawUIRect(&Label, FuncBtnColor() ? ColorRGBA{0.2, 0.2, 0.2, 1} : ColorRGBA{0.5, 0.2, 0.2, 1}, CUI::CORNER_ALL, 5);
		UI()->DoLabel(&Label, pLabelText, 25.0f, ETextAlignment::TEXTALIGN_CENTER);
		static int s_OptionButton = 0;
		if(UI()->DoButtonLogic(&s_OptionButton, 0, &Label))
		{
			FuncClick();
		}
		OptionBar.VSplitLeft(10.0f, nullptr, &OptionBar);
	};

	DoALabel(
		"Toggle borders", [this]() { m_Map.GetRenderBorderRef() = !m_Map.GetRenderBorderRef(); }, [this]() { return !m_Map.GetRenderBorderRef(); });
	DoALabel(
		"Toggle animations", [this]() { m_Map.GetDoAnimsRef() = !m_Map.GetDoAnimsRef(); }, [this]() { return !m_Map.GetDoAnimsRef(); });
	DoALabel(
		"Toggle entities", [this]() { m_Map.GetDoEntitiesRef() = !m_Map.GetDoEntitiesRef(); }, [this]() { return !m_Map.GetDoEntitiesRef(); });
	DoALabel(
		"Zoom-", [&]() { Zoom++; }, [] { return true; }, 100.0f);
	DoALabel(
		"Zoom+", [&]() { Zoom--; }, [] { return true; }, 100.0f);
	DoALabel(
		"Reset zoom", [&]() { m_ZoomLevel = 200; }, [] { return true; });

	if(Zoom != 0)
	{
		float ZoomFactor = (int)((m_ZoomLevel / 5.0f) + 1) * 1;
		m_ZoomLevel = maximum<float>(m_ZoomLevel + Zoom * ZoomFactor, 10);
	}

	m_Map.m_Camera.m_Zoom = (m_ZoomLevel / 100.0f);

	float aPoints[4];
	m_Map.MapScreenToGroup(0, 0, m_Map.GetLayers()->GameGroup(), m_Map.GetCurCamera()->m_Zoom);
	Graphics()->GetScreen(&aPoints[0], &aPoints[1], &aPoints[2], &aPoints[3]);

	if(m_Map.IsLoaded())
	{
		static bool NewPos = false;
		if(Input()->KeyPress(KEY_MOUSE_WHEEL_UP))
			Zoom--;
		if(Input()->KeyPress(KEY_MOUSE_WHEEL_DOWN))
			Zoom++;
		if((Buttons & (1 | 2 | 4)) && s_MouseY > OptionBarSize)
		{
			float WorldWidth = aPoints[2] - aPoints[0];
			float WorldHeight = aPoints[3] - aPoints[1];

			float DeltaX = WorldWidth * ((float)m_MouseDeltaX / Graphics()->WindowWidth());
			float DeltaY = WorldHeight * ((float)m_MouseDeltaY / Graphics()->WindowHeight());

			//m_Map.m_Camera.m_Center.x -= m_MouseDeltaX * (m_ZoomLevel / 100.0f);
			//m_Map.m_Camera.m_Center.y -= m_MouseDeltaY * (m_ZoomLevel / 100.0f);
			m_Map.m_Camera.m_Center.x -= DeltaX;
			m_Map.m_Camera.m_Center.y -= DeltaY;

			NewPos = true;
		}

		int64_t TimeNow = time_get_microseconds();
		if(NewPos && TimeNow - m_LastTime > 500000)
		{
			NewPos = false;
			int XCord = m_Map.m_Camera.m_Center.x;
			int YCord = m_Map.m_Camera.m_Center.y;
#ifdef CONF_WEBASM
			EM_ASM({
				var NewCordStr = window.location.href;
				var TmpEsc = String.fromCodePoint(92);
				var CurXCoord = NewCordStr.match("[&]{0,1}" + TmpEsc + "?x=[-0-9]*");
				if(CurXCoord != null)
					NewCordStr = NewCordStr.replaceAll(CurXCoord[0], "");

				var CurYCoord = NewCordStr.match("[&]{0,1}" + TmpEsc + "?y=[-0-9]*");
				if(CurYCoord != null)
					NewCordStr = NewCordStr.replaceAll(CurYCoord[0], "");

				var TmpFirstAnd = "&";
				if(NewCordStr.search(TmpEsc + "?") == -1)
					TmpFirstAnd = "";

				window.history.replaceState("", "", NewCordStr + TmpFirstAnd + "?x=" + $0.toString() + "&?y=" + $1.toString());
			},
				XCord, YCord);
#endif
			m_LastTime = TimeNow;
		}
	}

	Input()->Clear();
}

int CEditor2::Load(const char *pFilename, int StorageType)
{
	m_Map.LoadMenuBackground(pFilename);

#ifdef CONF_WEBASM
	int XCord = EM_ASM_INT({
		var TmpEsc = String.fromCodePoint(92);
		var TmpX = window.location.href.match(TmpEsc + "?x=[-0-9]+");
		if(TmpX != null)
		{
			TmpX = TmpX[0].match("[-0-9]+");
			return Number(TmpX[0]);
		}
		return 0;
	});
	int YCord = EM_ASM_INT({
		var TmpEsc = String.fromCodePoint(92);
		var TmpY = window.location.href.match(TmpEsc + "?y=[-0-9]+");
		if(TmpY != null)
		{
			TmpY = TmpY[0].match("[-0-9]+");
			return Number(TmpY[0]);
		}
		return 0;
	});

	m_Map.m_Camera.m_Center.x = XCord;
	m_Map.m_Camera.m_Center.y = YCord;
#endif

	g_Config.m_ClEditor = 1;
	return 0;
}

int CEditor2::Save(const char *pFilename) { return 0; }

CMapRender::CMapRender() :
	CBackground(CMapLayers::TYPE_FULL_DESIGN, false)
{
	m_Camera.m_Center = vec2(0.0f, 0.0f);
	m_Camera.m_PrevCenter = vec2(0.0f, 0.0f);

	m_IsInit = false;
}

CBackgroundEngineMap *CMapRender::CreateBGMap()
{
	return new CMapRenderMap;
}

void CMapRender::OnInit()
{
	m_pBackgroundMap = CreateBGMap();
	m_pMap = m_pBackgroundMap;

	m_IsInit = true;

	m_pImages->m_pClient = GameClient();
	Kernel()->RegisterInterface<CMapRenderMap>((CMapRenderMap *)m_pBackgroundMap);

	m_Camera.m_pClient = GameClient();
	m_Camera.m_ZoomSet = false;
	m_Camera.m_ZoomSmoothingTarget = 0;
}

void CMapRender::LoadMenuBackground(const char *pMapName)
{
	if(!m_IsInit)
		return;

	if(m_Loaded && m_pMap == m_pBackgroundMap)
		m_pMap->Unload();

	m_Loaded = false;
	m_pMap = m_pBackgroundMap;
	m_pLayers = m_pBackgroundLayers;
	m_pImages = m_pBackgroundImages;

	bool NeedImageLoading = false;

	str_copy(m_aMapName, pMapName, sizeof(m_aMapName));

	if(pMapName[0] != '\0')
	{
		char aBuf[128];

		if(!m_Loaded)
		{
			if(pMapName[0] == '/')
				str_format(aBuf, sizeof(aBuf), "%s", pMapName);
			else
				str_format(aBuf, sizeof(aBuf), "maps/%s", pMapName);
			if(m_pMap->Load(aBuf))
			{
				m_Loaded = true;
			}
		}

		if(m_Loaded)
		{
			m_pLayers->InitBackground(m_pMap);
			NeedImageLoading = true;

			CMapLayers::OnMapLoad();
			if(NeedImageLoading)
				m_pImages->LoadBackground(m_pLayers, m_pMap);
		}

		m_LastLoad = time_get();
	}
}

void CMapRender::OnMapLoad()
{
}

void CMapRender::OnRender()
{
}

bool CMapRender::Render()
{
	if(!m_Loaded)
		return false;

	if(Client()->State() >= IClient::STATE_ONLINE)
		return false;

	CMapLayers::OnRender();

	return true;
}

CCamera *CMapRender::GetCurCamera()
{
	return &m_Camera;
}
