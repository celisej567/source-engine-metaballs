#ifndef C_BLOBCONTAINER_H
#define C_BLOBCONTAINER_H

#include "cbase.h"
#include "c_props.h"
#include "METABALL.h"
#include "CUBE_GRID.h"
#include "c_point_blob_element.h"
#include "model_types.h"
#include "studio_stats.h"
#include <vector>

class C_PointBlobContainer : public C_BaseAnimating
{
	DECLARE_CLASS(C_PointBlobContainer, C_BaseAnimating);
public:
	DECLARE_NETWORKCLASS();

	C_PointBlobContainer()
	{
		GridSize = 10;
		cubeGrid = CUBE_GRID();
		GridBounds = {50,50,50};
		colorBoost = 1;

		Ambcolor.SetR(88);
		Ambcolor.SetG(88);
		Ambcolor.SetB(88);
		Ambcolor.SetA(255);

		color.SetR(100);
		color.SetG(100);
		color.SetB(100);
		color.SetA(255);

		cubeGrid.CreateMemory();
	}

	~C_PointBlobContainer()
	{
		cubeGrid.FreeMemory();
		//delete cubeGrid;
		metaballs.clear();
	}

	void UpdateContainer();
	void UpdateMeshData(size_t startpoint, size_t nOfIterations);
	void UpdateResolution();

	virtual int	DrawModel(int flags);
	virtual void GetRenderBounds(Vector& theMins, Vector& theMaxs);

	virtual void Spawn(void);
	virtual void ClientThink(){}
	//virtual void PostDataUpdate(DataUpdateType_t updateType);
	virtual bool ShouldDraw() {return true;}

	virtual void Simulate();

	virtual void OnDataChanged(DataUpdateType_t type){}

	CUBE_GRID cubeGrid;
	int GridSize;
	float colorBoost;
	Vector GridBounds;
	CNetworkColor32(Ambcolor);
	CNetworkColor32(color);
	std::vector<C_PointBlobElement*> metaballs;

	IMesh* pMesh;

	C_PointBlobContainer* parentedContainer;

	Vector white[6];
	
	char BlobMaterialName[MAX_PATH];

	KeyValues* kval;
	IMaterial* CustomMat;
private:
	SURFACE_VERTEX edgeVertices[12];

	bool first = false;

};

#endif // !C_BLOBCONTAINER_H
