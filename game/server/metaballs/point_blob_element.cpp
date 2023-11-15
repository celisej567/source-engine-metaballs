

#include "cbase.h"
//#include "c_props.h"
#include "point_blob_element.h"

// memdbgon must be the last include file in a .cpp file!!! :apple_advertisement:
//#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(point_blob_element, CPointBlobElement);

BEGIN_DATADESC(CPointBlobElement)
//DEFINE_KEYFIELD(radius, FIELD_FLOAT, "radius"),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CPointBlobElement, DT_PointBlobElement)
SendPropFloat(SENDINFO(radius)),
SendPropFloat(SENDINFO(radiusSquared)),
END_SEND_TABLE()




