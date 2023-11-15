#ifndef C_BLOBELEMENT_H
#define C_BLOBELEMENT_H

#include "cbase.h"
#include "c_props.h"
#include "debugoverlay_shared.h"
#include "METABALL.h"

/////////////////////////////////////
/// 
/// This is a blob element, or single blob if you prefer. He needs to initialise METABALL variable.
/// You have to use this with C_PointBlobContainer to see the blob itself.
/// 
/////////////////////////////////////

class C_PointBlobElement : public C_BaseEntity
{
	DECLARE_CLASS(C_PointBlobElement, C_BaseEntity);
public:
	DECLARE_NETWORKCLASS();

	C_PointBlobElement();
	C_PointBlobElement(int newRadius);
	~C_PointBlobElement(){}


	virtual void Spawn(void);
	virtual void ClientThink(){}

	virtual void OnDataChanged(DataUpdateType_t type){}

	virtual void Simulate();

	float radius;
	float radiusSquared;
	METABALL metaball;
	Vector pos;
	float SpawnTime = 0;
	bool First = false;
};

#endif // C_BLOBELEMENT_H
