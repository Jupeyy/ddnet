/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <base/vmath.h>
#include <engine/shared/protocol.h>

#include <list>

enum
{
	CANTMOVE_LEFT = 1 << 0,
	CANTMOVE_RIGHT = 1 << 1,
	CANTMOVE_UP = 1 << 2,
	CANTMOVE_DOWN = 1 << 3,
};

vec2 ClampVel(int MoveRestriction, vec2 Vel);

typedef bool (*CALLBACK_SWITCHACTIVE)(int Number, void *pUser);
struct CAntibotMapData;

class CCollision
{
	class CTile *m_pTiles;
	int m_Width;
	int m_Height;
	class CLayers *m_pLayers;

public:
	CCollision();
	~CCollision();
	void Init(class CLayers *pLayers);
	void FillAntibot(CAntibotMapData *pMapData);
	bool CheckPoint(EngineFloat x, EngineFloat y) const { return IsSolid(round_to_int(x), round_to_int(y)); }
	bool CheckPoint(vector2_base<EngineFloat> Pos) const { return CheckPoint(Pos.x, Pos.y); }
	int GetCollisionAt(EngineFloat x, EngineFloat y) const { return GetTile(round_to_int(x), round_to_int(y)); }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int IntersectLine(vector2_base<EngineFloat> Pos0, vector2_base<EngineFloat> Pos1, vector2_base<EngineFloat> *pOutCollision, vector2_base<EngineFloat> *pOutBeforeCollision) const;
	int IntersectLineTeleWeapon(vector2_base<EngineFloat> Pos0, vector2_base<EngineFloat> Pos1, vector2_base<EngineFloat> *pOutCollision, vector2_base<EngineFloat> *pOutBeforeCollision, int *pTeleNr) const;
	int IntersectLineTeleHook(vector2_base<EngineFloat> Pos0, vector2_base<EngineFloat> Pos1, vector2_base<EngineFloat> *pOutCollision, vector2_base<EngineFloat> *pOutBeforeCollision, int *pTeleNr) const;
	void MovePoint(vector2_base<EngineFloat> *pInoutPos, vector2_base<EngineFloat> *pInoutVel, EngineFloat Elasticity, int *pBounces) const;
	void MoveBox(vector2_base<EngineFloat> *pInoutPos, vector2_base<EngineFloat> *pInoutVel, vector2_base<EngineFloat> Size, EngineFloat Elasticity) const;
	bool TestBox(vector2_base<EngineFloat> Pos, vector2_base<EngineFloat> Size) const;

	// DDRace

	void Dest();
	void SetCollisionAt(float x, float y, int id);
	void SetDTile(float x, float y, bool State);
	void SetDCollisionAt(float x, float y, int Type, int Flags, int Number);
	int GetDTileIndex(int Index) const;
	int GetDTileFlags(int Index) const;
	int GetDTileNumber(int Index) const;
	int GetFCollisionAt(float x, float y) const { return GetFTile(round_to_int(x), round_to_int(y)); }
	int IntersectNoLaser(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int IntersectNoLaserNW(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int IntersectAir(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int GetIndex(int x, int y) const;
	int GetIndex(vec2 PrevPos, vec2 Pos) const;
	int GetFIndex(int x, int y) const;

	int GetMoveRestrictions(CALLBACK_SWITCHACTIVE pfnSwitchActive, void *pUser, vec2 Pos, float Distance = 18.0f, int OverrideCenterTileIndex = -1);
	int GetMoveRestrictions(vec2 Pos, float Distance = 18.0f)
	{
		return GetMoveRestrictions(0, 0, Pos, Distance);
	}

	int GetTile(int x, int y) const;
	int GetFTile(int x, int y) const;
	int Entity(int x, int y, int Layer) const;
	int GetPureMapIndex(float x, float y) const;
	int GetPureMapIndex(vec2 Pos) const { return GetPureMapIndex(Pos.x, Pos.y); }
	std::list<int> GetMapIndices(vec2 PrevPos, vec2 Pos, unsigned MaxIndices = 0) const;
	int GetMapIndex(vec2 Pos) const;
	bool TileExists(int Index) const;
	bool TileExistsNext(int Index) const;
	vec2 GetPos(int Index) const;
	int GetTileIndex(int Index) const;
	int GetFTileIndex(int Index) const;
	int GetTileFlags(int Index) const;
	int GetFTileFlags(int Index) const;
	int IsTeleport(int Index) const;
	int IsEvilTeleport(int Index) const;
	int IsCheckTeleport(int Index) const;
	int IsCheckEvilTeleport(int Index) const;
	int IsTeleportWeapon(int Index) const;
	int IsTeleportHook(int Index) const;
	int IsTCheckpoint(int Index) const;
	int IsSpeedup(int Index) const;
	int IsTune(int Index) const;
	void GetSpeedup(int Index, vec2 *Dir, int *Force, int *MaxSpeed) const;
	int GetSwitchType(int Index) const;
	int GetSwitchNumber(int Index) const;
	int GetSwitchDelay(int Index) const;

	int IsSolid(int x, int y) const;
	bool IsThrough(int x, int y, int xoff, int yoff, vec2 pos0, vec2 pos1) const;
	bool IsHookBlocker(int x, int y, vec2 pos0, vec2 pos1) const;
	int IsWallJump(int Index) const;
	int IsNoLaser(int x, int y) const;
	int IsFNoLaser(int x, int y) const;

	int IsCheckpoint(int Index) const;
	int IsFCheckpoint(int Index) const;

	int IsMover(int x, int y, int *pFlags) const;

	vec2 CpSpeed(int index, int Flags = 0) const;

	class CTeleTile *TeleLayer() { return m_pTele; }
	class CSwitchTile *SwitchLayer() { return m_pSwitch; }
	class CTuneTile *TuneLayer() { return m_pTune; }
	class CLayers *Layers() { return m_pLayers; }
	int m_NumSwitchers;

	struct SSwitchers
	{
		bool m_Status[MAX_CLIENTS];
		bool m_Initial;
		int m_EndTick[MAX_CLIENTS];
		int m_Type[MAX_CLIENTS];
	};
	SSwitchers *m_pSwitchers;

private:
	class CTeleTile *m_pTele;
	class CSpeedupTile *m_pSpeedup;
	class CTile *m_pFront;
	class CSwitchTile *m_pSwitch;
	class CTuneTile *m_pTune;
	class CDoorTile *m_pDoor;
};

void ThroughOffset(vec2 Pos0, vec2 Pos1, int *Ox, int *Oy);
#endif
