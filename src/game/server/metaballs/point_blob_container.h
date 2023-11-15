#ifndef C_BLOBCONTAINER_H
#define C_BLOBCONTAINER_H

#include "cbase.h"
#include "props.h"
#include "point_blob_element.h"

//just empty shit to be able to spawn entity using ent_create and use entity for fgd for map

class CPointBlobContainer : public	CBreakableProp
{
	DECLARE_CLASS(CPointBlobContainer, CBreakableProp);
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CPointBlobContainer()
	{
		GridSize = 10;
		GridBounds = Vector(50,50,50);
		color = {255,255,255};
	}
	
	virtual void Spawn()
	{
		SetTransmitState(FL_EDICT_PVSCHECK);

		m_nRenderMode = kRenderNormal;
		m_nRenderFX = kRenderFxNone;
		SetLightingOrigin(this);

		//PrecacheModel("models/error.mdl");
		//SetModel("models/error.mdl");
		//BaseClass::Spawn();

		// to shut up everything
		SetMoveType(MOVETYPE_NONE);
		m_takedamage = DAMAGE_NO;
		SetNextThink(TICK_NEVER_THINK);
		
		m_flAnimTime = gpGlobals->curtime;
		m_flPlaybackRate = 0.0;
		SetCycle(0);
	}

	CNetworkVar(int, GridSize);
	CNetworkVar(float, colorBoost);
	CNetworkVar(Vector, GridBounds);
	CNetworkVar(string_t, BlobMaterialName);
	CNetworkColor32(color);
	CNetworkColor32(Ambcolor);

};

#endif // !C_BLOBCONTAINER_H
