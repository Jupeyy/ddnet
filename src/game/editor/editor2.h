#ifndef GAME_EDITOR_EDITOR2_H
#define GAME_EDITOR_EDITOR2_H

#include "game/client/components/camera.h"
#include "game/client/components/maplayers.h"
#include "game/client/render.h"
#include "game/client/ui_ex.h"
#include "game/editor/editor.h"
#include <engine/editor.h>
#include <engine/graphics.h>

#include <game/client/components/background.h>

class CMapRenderMap : public CBackgroundEngineMap
{
	MACRO_INTERFACE("maprender_enginemap", 0)
};

class CMapRender : public CBackground
{
public:
	CCamera m_Camera;

	CBackgroundEngineMap *CreateBGMap() override;

	bool m_IsInit = false;

	CMapRender();
	~CMapRender() override {}
	virtual int Sizeof() const override { return sizeof(*this); }

	void OnInit() override;
	void OnMapLoad() override;
	void OnRender() override;

	void LoadMenuBackground(const char *pMapName);

	bool Render();

	class CCamera *GetCurCamera() override;

	IEngineMap *GetMap() { return m_pMap; }
	CLayers *GetLayers() { return m_pLayers; }

	bool IsLoaded() { return m_Loaded; }

	bool &GetRenderBorderRef() { return m_DrawBorders; }
	bool &GetDoAnimsRef() { return m_DoAnims; }
	bool &GetDoEntitiesRef() { return m_DoEntities; }

	void MapScreenToGroup(float CenterX, float CenterY, CMapItemGroup *pGroup, float Zoom = 1.0f) { CMapLayers::MapScreenToGroup(CenterX, CenterY, pGroup, Zoom); }
};

class CEditor2 : public IEditor
{
	class IInput *m_pInput;
	class IClient *m_pClient;
	class CConfig *m_pConfig;
	class IConsole *m_pConsole;
	class IGraphics *m_pGraphics;
	class ITextRender *m_pTextRender;
	class ISound *m_pSound;
	class IStorage *m_pStorage;
	CMapRender m_Map;

	CRenderTools m_RenderTools;
	CUI m_UI;
	CUIEx m_UIEx;

	float m_MouseDeltaX = 0;
	float m_MouseDeltaY = 0;
	float m_ZoomLevel = 200;

	int64_t m_LastTime = 0;

public:
	class IInput *Input() { return m_pInput; }
	class IClient *Client() { return m_pClient; }
	class CConfig *Config() { return m_pConfig; }
	class IConsole *Console() { return m_pConsole; }
	class IGraphics *Graphics() { return m_pGraphics; }
	class ISound *Sound() { return m_pSound; }
	class ITextRender *TextRender() { return m_pTextRender; }
	class IStorage *Storage() { return m_pStorage; }
	CUI *UI() { return &m_UI; }
	CUIEx *UIEx() { return &m_UIEx; }
	CRenderTools *RenderTools() { return &m_RenderTools; }

	CEditor2()
	{
	}

	virtual void Init();
	virtual void UpdateAndRender();
	virtual bool HasUnsavedData() const { return false; }
	virtual void UpdateMentions() {}
	virtual void ResetMentions() {}
	virtual int Load(const char *pFilename, int StorageType);
	virtual int Save(const char *pFilename);
};
#endif
