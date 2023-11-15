

#include "cbase.h"
//#include "c_props.h"
#include "METABALL.h"
#include "c_point_blob_element.h"

// memdbgon must be the last include file in a .cpp file!!! :apple_advertisement:
//#include "tier0/memdbgon.h"


LINK_ENTITY_TO_CLASS(point_blob_element, C_PointBlobElement);

IMPLEMENT_NETWORKCLASS_ALIASED(PointBlobElement, DT_PointBlobElement)

BEGIN_NETWORK_TABLE(C_PointBlobElement, DT_PointBlobElement)
RecvPropFloat(RECVINFO(radius)),
RecvPropFloat(RECVINFO(radiusSquared)),
END_NETWORK_TABLE()

void C_PointBlobElement::Spawn()
{	
	BaseClass::Spawn();
}

C_PointBlobElement::C_PointBlobElement()
{
	radius = 10;
	radiusSquared = radius * radius;
	metaball = METABALL();
	
}

C_PointBlobElement::C_PointBlobElement(int newRadius)
{
	radius = newRadius;
	radiusSquared = radius * radius;
	metaball = METABALL();
	
}

void C_PointBlobElement::Simulate()
{
	BaseClass::Simulate();
	//if(!First)
	//{ 
	//	pos = GetAbsOrigin();
	//	SpawnTime = gpGlobals->curtime;
	//	First = true;
	//}
	
	//Vector ghgh;
	//ghgh.x = pos.x + sin(gpGlobals->curtime - SpawnTime);
	//ghgh.y = pos.y + sin(gpGlobals->curtime - SpawnTime);
	//ghgh.z = pos.z + sin(gpGlobals->curtime - SpawnTime);
	//SetAbsOrigin(ghgh);

	//metaball.squaredRadius = radius * radius;
	//metaball.position = GetAbsOrigin();
	//NDebugOverlay::Sphere(GetAbsOrigin(), sqrt(radius), 255, 0, 0, true, 0.0015f);
	//Msg("dkgsfhgkklgsfklgjdfjklbujdfhgu");
}
