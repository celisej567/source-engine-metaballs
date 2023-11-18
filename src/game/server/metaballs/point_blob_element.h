#ifndef C_BLOBELEMENT_H
#define C_BLOBELEMENT_H

#include "cbase.h"
#include "props.h"
#include "props_shared.h"

/////////////////////////////////////
/// 
/// This is a blob element, or single blob if you prefer. He needs to initialise METABALL variable.
/// You have to use this with C_PointBlobContainer to see the blob itself.
/// 
/////////////////////////////////////

class CPointBlobElement : public CBaseEntity
{
	DECLARE_CLASS(CPointBlobElement, CBaseEntity);
public:
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPointBlobElement(){

		radius = 10;
		radiusSquared = radius * radius;
	}

    virtual void Spawn()
    {
		SetTransmitState(FL_EDICT_ALWAYS);
		m_nRenderMode = kRenderNormal;
		m_nRenderFX = kRenderFxNone;

	}

	bool KeyValue(const char* szKeyName, const char* szValue)
	{
		if (FStrEq(szKeyName, "radius"))
		{
			radius = atof(szValue);
			radiusSquared = radius * radius;
		}
		return BaseClass::KeyValue(szKeyName, szValue);
	}

	CNetworkVar(float, radius);
	CNetworkVar(float, radiusSquared);

};

#endif // C_BLOBELEMENT_H
