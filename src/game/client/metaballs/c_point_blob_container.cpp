
#include "cbase.h"
#include "c_props.h"
#include "METABALL.h"
#include "CUBE_GRID.h"
#include "debugoverlay_shared.h"
#include "c_point_blob_element.h"
#include "c_point_blob_container.h"
#include "cdll_util.h"

#include <omp.h>

#include "materialsystem/imesh.h"

LINK_ENTITY_TO_CLASS(point_blob_container, C_PointBlobContainer);

IMPLEMENT_NETWORKCLASS_ALIASED(PointBlobContainer, DT_PointBlobContainer)
BEGIN_NETWORK_TABLE(C_PointBlobContainer, DT_PointBlobContainer)
	RecvPropInt(RECVINFO(GridSize)),
	RecvPropVector(RECVINFO(GridBounds)),
	RecvPropInt(RECVINFO(color), 0, RecvProxy_IntToColor32),
	RecvPropInt(RECVINFO(Ambcolor), 0, RecvProxy_IntToColor32),
	RecvPropFloat(RECVINFO(colorBoost)),
	RecvPropString(RECVINFO(BlobMaterialName)),

END_NETWORK_TABLE()

ConVar r_drawblobs("r_drawblobs", "1", FCVAR_CLIENTDLL);
ConVar cl_blobs_updatecontainers("cl_blobs_updatecontainers", "1", FCVAR_CLIENTDLL);
//ConVar cl_blobs_resolution("cl_blobs_resolution", "10", FCVAR_CLIENTDLL);
ConVar cl_blobthreshold("cl_blobthreshold", "1", FCVAR_CLIENTDLL);
ConVar cl_blobvaluethreshold("cl_blobvaluethreshold", "0.03", FCVAR_CLIENTDLL);

//why? why not! (it works better then Q_strcmp that redirects to V_strcmp that redirects to default strcmp)
int Quake_strcmp (const char* s1, const char* s2)
{
	while (1)
	{
		if (*s1 != *s2)
			return -1;        // strings not equal
		if (!*s1)
			return 0;        // strings are equal
		s1++;
		s2++;
	}

	return -1;
}

void C_PointBlobContainer::Spawn()
{
	//get already existed container to save compute resources and just use already existed one.
	///////
	size_t count = 0;
	// Iterate through all the entities in the clientside world
	for (int i = 0; i < cl_entitylist->GetHighestEntityIndex() + 1; i++)
	{
		if(parentedContainer != NULL)
		{ 
			C_BaseEntity* entity = cl_entitylist->GetBaseEntity(i);
			if (entity && entity->GetClientClass()->m_pNetworkName)
			{
				// Check if the entity is valid and matches the desired class name
				if (Quake_strcmp(entity->GetClientClass()->m_pNetworkName, "CPointBlobContainer") == 0)
				{
					//add entity into array
					parentedContainer = ((C_PointBlobContainer*)(entity));
					Msg("%u\n", count);
				}
			}
		}
	}
	////////

	BaseClass::Spawn();
}

int C_PointBlobContainer::DrawModel(int flags)
{
	if(r_drawblobs.GetBool())
	{
	if (metaballs.size() > 0)
	{

		///Source dont like when you do CMeshBuilder stuff in DrawModel. So sometimes lighting looks like shit. Im making it unlit because i cant feagure out how to fix this

		CMatRenderContextPtr pRenderContext(materials);
		
		pRenderContext->Bind(CustomMat, NULL);
		pRenderContext->SetLightingOrigin(GetAbsOrigin());

		g_pStudioRender->SetAmbientLightColors(white);

		//// Disable all the lights..
		//pRenderContext->DisableAllLocalLights();
		
		pMesh = pRenderContext->GetDynamicMesh();
		
		CMeshBuilder meshBuilder;

		const float threshold = cl_blobthreshold.GetFloat();

		meshBuilder.Begin(pMesh, MATERIAL_TRIANGLES, cubeGrid.numVertices);

		//loop through cubes
		#pragma omp parallel for 
		for (int i = 0; i < cubeGrid.numCubes; i++)
		{

			//calculate which vertices are inside the surface
			unsigned char cubeIndex = 0;
			if (cubeGrid.cubes[i].vertices[0]->value < threshold)
				cubeIndex |= 1;
			if (cubeGrid.cubes[i].vertices[1]->value < threshold)
				cubeIndex |= 2;
			if (cubeGrid.cubes[i].vertices[2]->value < threshold)
				cubeIndex |= 4;
			if (cubeGrid.cubes[i].vertices[3]->value < threshold)
				cubeIndex |= 8;
			if (cubeGrid.cubes[i].vertices[4]->value < threshold)
				cubeIndex |= 16;
			if (cubeGrid.cubes[i].vertices[5]->value < threshold)
				cubeIndex |= 32;
			if (cubeGrid.cubes[i].vertices[6]->value < threshold)
				cubeIndex |= 64;
			if (cubeGrid.cubes[i].vertices[7]->value < threshold)
				cubeIndex |= 128;

			//look this value up in the edge table to see which edges to interpolate along
			int usedEdges = edgeTable[cubeIndex];


			//if the cube is entirely within/outside surface, no faces
			if (usedEdges == 0 || usedEdges == 255)
				continue;

			//update these edges
			for (int currentEdge = 0; currentEdge < 12; currentEdge++)
			{
				if (usedEdges & 1 << currentEdge)
				{
					CUBE_GRID_VERTEX* v1 = cubeGrid.cubes[i].vertices[verticesAtEndsOfEdges[currentEdge * 2]];
					CUBE_GRID_VERTEX* v2 = cubeGrid.cubes[i].vertices[verticesAtEndsOfEdges[currentEdge * 2 + 1]];

					float delta = (threshold - v1->value) / (v2->value - v1->value);
					//edgeVertices[currentEdge].position=v1->position + delta*(v2->position - v1->position);
					edgeVertices[currentEdge].position.x = v1->position.x + delta * (v2->position.x - v1->position.x);
					edgeVertices[currentEdge].position.y = v1->position.y + delta * (v2->position.y - v1->position.y);
					edgeVertices[currentEdge].position.z = v1->position.z + delta * (v2->position.z - v1->position.z);
					//edgeVertices[currentEdge].normal=v1->normal + delta*(v2->normal - v1->normal);
					edgeVertices[currentEdge].normal.x = v1->normal.x + delta * (v2->normal.x - v1->normal.x);
					edgeVertices[currentEdge].normal.y = v1->normal.y + delta * (v2->normal.y - v1->normal.y);
					edgeVertices[currentEdge].normal.z = v1->normal.z + delta * (v2->normal.z - v1->normal.z);
				}
			}

			//send the vertices
			for (int k = 0; triTable[cubeIndex][k] != -1; k += 3)
			{
				//Vector pos = Vector(edgeVertices[triTable[cubeIndex][k + 0]].position.x, edgeVertices[triTable[cubeIndex][k + 0]].position.y, edgeVertices[triTable[cubeIndex][k + 0]].position.z);
				//Vector vertnormal = Vector(edgeVertices[triTable[cubeIndex][k + 0]].normal.x, edgeVertices[triTable[cubeIndex][k + 0]].normal.y, edgeVertices[triTable[cubeIndex][k + 0]].normal.z);

				meshBuilder.Normal3fv(edgeVertices[triTable[cubeIndex][k + 0]].normal.Base());
				meshBuilder.Position3fv(edgeVertices[triTable[cubeIndex][k + 0]].position.Base());
				meshBuilder.TexCoord2f(0, 1, 1);
				meshBuilder.Color3f(1,0,0);
				meshBuilder.AdvanceVertex();

				//pos = Vector(edgeVertices[triTable[cubeIndex][k + 1]].position.x, edgeVertices[triTable[cubeIndex][k + 1]].position.y, edgeVertices[triTable[cubeIndex][k + 1]].position.z);
				//vertnormal = Vector(edgeVertices[triTable[cubeIndex][k + 1]].normal.x, edgeVertices[triTable[cubeIndex][k + 1]].normal.y, edgeVertices[triTable[cubeIndex][k + 1]].normal.z);

				meshBuilder.Normal3fv(edgeVertices[triTable[cubeIndex][k + 1]].normal.Base());
				meshBuilder.Position3fv(edgeVertices[triTable[cubeIndex][k + 1]].position.Base());
				meshBuilder.TexCoord2f(0, 1, 0);
				meshBuilder.Color3f(0, 1, 0);
				meshBuilder.AdvanceVertex();

				//pos = Vector(edgeVertices[triTable[cubeIndex][k + 2]].position.x, edgeVertices[triTable[cubeIndex][k + 2]].position.y, edgeVertices[triTable[cubeIndex][k + 2]].position.z);
				//vertnormal = Vector(edgeVertices[triTable[cubeIndex][k + 2]].normal.x, edgeVertices[triTable[cubeIndex][k + 2]].normal.y, edgeVertices[triTable[cubeIndex][k + 2]].normal.z);

				meshBuilder.Normal3fv(edgeVertices[triTable[cubeIndex][k + 2]].normal.Base());
				meshBuilder.Position3fv(edgeVertices[triTable[cubeIndex][k + 2]].position.Base());
				meshBuilder.TexCoord2f(0, 0, 1);
				meshBuilder.Color3f(0, 0, 1);
				meshBuilder.AdvanceVertex();

			}
		}
		//pRenderContext->SetFlashlightMode(false);
		meshBuilder.End();
		//modelrender->SuppressEngineLighting(false);

		pMesh->Draw();
		pRenderContext->Flush();

		//delete pMesh;
		//pRenderContext->PopMatrix();
		//pRenderContext->Flush();
	}
	}

	return 1;
	//return BaseClass::DrawModel(flags);
	//return InternalDrawModel(STUDIO_RENDER | extraFlags);
}

void C_PointBlobContainer::GetRenderBounds(Vector& theMins, Vector& theMaxs)
{

	theMaxs = Vector(GridBounds.x, GridBounds.y, GridBounds.z);
	theMins = Vector(0,0,0);

}


void C_PointBlobContainer::UpdateResolution()
{
	//GridSize = cl_blobs_resolution.GetInt();
	cubeGrid.FreeMemory();
	cubeGrid.CreateMemory();
}

void C_PointBlobContainer::UpdateMeshData(size_t startpoint, size_t nOfIterations)
{

	//Vector ballToPoint;
	//float squaredRadius;
	//Vector ballPosition;
	//float normalScale = 0;
	const float ValueThreshold = cl_blobvaluethreshold.GetFloat();

	//+fps for me
	#pragma omp parallel for 
	for (size_t i = startpoint; i < startpoint + nOfIterations ; i++)
	{
		Vector ballPosition = metaballs[i]->GetAbsOrigin();
		float squaredRadius = metaballs[i]->radiusSquared;

		for (size_t j = 0; j < cubeGrid.numVertices; j++)
		{
			Vector ballToPoint = cubeGrid.vertices[j].position - ballPosition;
			vec_t squareDistance = ballToPoint.LengthSqr();

			if (squareDistance != 0)
			{
				float valueIncrement = squaredRadius / squareDistance;
				cubeGrid.vertices[j].value += (valueIncrement > ValueThreshold) ? valueIncrement : 0;

				float normalScale = squaredRadius / (squareDistance * squareDistance);
				cubeGrid.vertices[j].normal += ballToPoint * normalScale;
			}
		}
	}
}

void C_PointBlobContainer::UpdateContainer()
{

	//cubeGrid->Init(GridSize, GetAbsOrigin(), GridBounds);
	if(parentedContainer == NULL)
	{ 
		metaballs.clear();

		// Iterate through all the entities in the clientside world
		for (int i = 0; i < cl_entitylist->GetHighestEntityIndex()+1; i++)
		{
			C_BaseEntity* entity = cl_entitylist->GetBaseEntity(i);
			if (entity && entity->GetClientClass()->m_pNetworkName)
			{
				// Check if the entity is valid and matches the desired class name
				if (Quake_strcmp(entity->GetClientClass()->m_pNetworkName, "CPointBlobElement") == 0)
				{
					//add entity into array
					metaballs.push_back((C_PointBlobElement*)(entity));
				}
			}
		}
	}
	else
	{
		metaballs = parentedContainer->metaballs;
	}

	if (cl_blobs_updatecontainers.GetBool())
	{

		//clear the field
		for (size_t i = 0; i < cubeGrid.numVertices; i++)
		{
			cubeGrid.vertices[i].value = 0.0f;
			cubeGrid.vertices[i].normal = {0,0,0};
		}
		UpdateMeshData(0, metaballs.size());
	}
}

void C_PointBlobContainer::Simulate()
{
	BaseClass::Simulate();
	//if(cl_blobs_updatecontainers.GetBool())
	UpdateContainer();
	//if((cl_blobs_resolution.GetInt() != GridSize) || ((ConVarRef("cl_blobs_resolution_max").GetInt()) != ( cbrtf(cubeGrid->numVertices) - 3 )))
	//	UpdateResolution();
	if(developer.GetInt() > 0)
	{
		Vector min;
		Vector max;
		GetRenderBounds(min,max);
			NDebugOverlay::Box(GetAbsOrigin(), min, max, 255, 0, 0, 0, 0.0015f);
	}

	// Why like this? 
	// When you spawn entity using ent_create, engine spawns entity at 0 0 0, calls Spawn(), transfer all the netvar data and only then he changes position to view point.
	// Simulate() called only AFTER changing position, so this shit will work more reliably here. 
	// (Yes, that means game dont transfer data before Spawn(), but after. Thats why i put BlobMaterialName check here.)
	if (!first)
	{
		cubeGrid.Init(GridSize, GetAbsOrigin(), GridBounds);

		for(size_t i=0; i<6; i++)
		{
			//white[i] = Vector(RemapVal(color.GetR(), 0, 255, 0, 1) * colorBoost, RemapVal(color.GetG(), 0, 255, 0, 1) * colorBoost, RemapVal(color.GetB(), 0, 255, 0, 1) * colorBoost);
			white[i] = Vector(((float)Ambcolor.GetR()/255) * colorBoost, ((float)Ambcolor.GetG()/255) * colorBoost, ((float)Ambcolor.GetB()/255) * colorBoost);
			//white[i] = Vector((Ambcolor.GetR()/0xFF) * colorBoost, (Ambcolor.GetG()/0xFF) * colorBoost, (Ambcolor.GetB()/0xFF) * colorBoost);
			//white[i] = Vector(1 * colorBoost,1 * colorBoost,1 * colorBoost);
		}

		if (!BlobMaterialName[0])
		{
			kval = new KeyValues("Vertexlitgeneric");
			kval->SetString("$color", VarArgs( "{ %i %i %i }", color.GetR(), color.GetG(), color.GetB() ) );
			//kval->SetString("$vertexcolor", "1");
			//kval->SetString("$vertexalpha", "1");
			//kval->SetString("$envmap", "editor/cubemap");
			CustomMat = materials->CreateMaterial("blobik.vmt", kval);
		}
		else{
			PrecacheMaterial(BlobMaterialName);
			CustomMat = materials->FindMaterial(BlobMaterialName, TEXTURE_GROUP_OTHER);
		}
		first = true;
	}

	//if (GridSize != cl_blobs_resolution.GetInt())
	//{
	//	GridSize = cl_blobs_resolution.GetInt();
	//}
}




