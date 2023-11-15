//////////////////////////////////////////////////////////////////////////////////////////
//	CUBE_GRID.cpp
//	function definitions for grid of cubes for "marching cubes" algorithm
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#include "cbase.h"
#include "CUBE_GRID.h"
#include "cbase.h"

ConVar cl_blobs_resolution_max("cl_blobs_resolution_max", "20", FCVAR_CLIENTDLL);

bool CUBE_GRID::CreateMemory()
{
	vertices=new CUBE_GRID_VERTEX[(cl_blobs_resolution_max.GetInt() + 1) * (cl_blobs_resolution_max.GetInt() + 1) * (cl_blobs_resolution_max.GetInt() + 1)];
	if(!vertices)
	{
		Msg("Unable to allocate memory for %d Grid Vertices", numVertices);
		return false;
	}

	cubes=new CUBE_GRID_CUBE[cl_blobs_resolution_max.GetInt() * cl_blobs_resolution_max.GetInt() * cl_blobs_resolution_max.GetInt()];
	if(!cubes)
	{
		Msg("Unable to allocate memory for %d Grid Cubes", numCubes);
		return false;
	}

	return true;
}
		

bool CUBE_GRID::Init(int gridSize, Vector Pos, Vector Bounds)
{
	
	//VERTICES
	numVertices=(gridSize+1)*(gridSize+1)*(gridSize+1);
	
	int currentVertex=0;

	for(int i=0; i<gridSize+1; i++)
	{
		for(int j=0; j<gridSize+1; j++)
		{
			for(int k=0; k<gridSize+1; k++)
			{
				vertices[currentVertex].position.x = ((Bounds.x / gridSize * i)) + Pos.x;
				vertices[currentVertex].position.y = ((Bounds.y / gridSize * j)) + Pos.y;
				vertices[currentVertex].position.z = ((Bounds.z / gridSize * k)) + Pos.z;

				currentVertex++;
			}
		}
	}

	//CUBES
	numCubes=(gridSize)*(gridSize)*(gridSize);

	int currentCube=0;

	for(int i=0; i<gridSize; i++)
	{
		for(int j=0; j<gridSize; j++)
		{
			for(int k=0; k<gridSize; k++)
			{
				cubes[currentCube].vertices[0]=&vertices[(i*(gridSize+1)+j)*(gridSize+1)+k];
				cubes[currentCube].vertices[1]=&vertices[(i*(gridSize+1)+j)*(gridSize+1)+k+1];
				cubes[currentCube].vertices[2]=&vertices[(i*(gridSize+1)+(j+1))*(gridSize+1)+k+1];
				cubes[currentCube].vertices[3]=&vertices[(i*(gridSize+1)+(j+1))*(gridSize+1)+k];
				cubes[currentCube].vertices[4]=&vertices[((i+1)*(gridSize+1)+j)*(gridSize+1)+k];
				cubes[currentCube].vertices[5]=&vertices[((i+1)*(gridSize+1)+j)*(gridSize+1)+k+1];
				cubes[currentCube].vertices[6]=&vertices[((i+1)*(gridSize+1)+(j+1))*(gridSize+1)+k+1];
				cubes[currentCube].vertices[7]=&vertices[((i+1)*(gridSize+1)+(j+1))*(gridSize+1)+k];

				currentCube++;
			}
		}
	}

	return true;
}

void CUBE_GRID::DrawSurface(float threshold)
{
/*
	numFacesDrawn=0;

	static SURFACE_VERTEX edgeVertices[12];
	
	glBegin(GL_TRIANGLES);
	{
		//loop through cubes
		for(int i=0; i<numCubes; i++)
		{
			//calculate which vertices are inside the surface
			unsigned char cubeIndex=0;
	
			if(cubes[i].vertices[0]->value < threshold)
				cubeIndex |= 1;
			if(cubes[i].vertices[1]->value < threshold)
				cubeIndex |= 2;
			if(cubes[i].vertices[2]->value < threshold)
				cubeIndex |= 4;
			if(cubes[i].vertices[3]->value < threshold)
				cubeIndex |= 8;
			if(cubes[i].vertices[4]->value < threshold)
				cubeIndex |= 16;
			if(cubes[i].vertices[5]->value < threshold)
				cubeIndex |= 32;
			if(cubes[i].vertices[6]->value < threshold)
				cubeIndex |= 64;
			if(cubes[i].vertices[7]->value < threshold)
				cubeIndex |= 128;
	
			//look this value up in the edge table to see which edges to interpolate along
			int usedEdges=edgeTable[cubeIndex];

			//if the cube is entirely within/outside surface, no faces			
			if(usedEdges==0 || usedEdges==255)
				continue;
	
			//update these edges
			for(int currentEdge=0; currentEdge<12; currentEdge++)
			{
				if(usedEdges & 1<<currentEdge)
				{
					CUBE_GRID_VERTEX * v1=cubes[i].vertices[verticesAtEndsOfEdges[currentEdge*2  ]];
					CUBE_GRID_VERTEX * v2=cubes[i].vertices[verticesAtEndsOfEdges[currentEdge*2+1]];
				
					float delta=(threshold - v1->value)/(v2->value - v1->value);
					//edgeVertices[currentEdge].position=v1->position + delta*(v2->position - v1->position);
					edgeVertices[currentEdge].position.x=v1->position.x + delta*(v2->position.x - v1->position.x);
					edgeVertices[currentEdge].position.y=v1->position.y + delta*(v2->position.y - v1->position.y);
					edgeVertices[currentEdge].position.z=v1->position.z + delta*(v2->position.z - v1->position.z);
					//edgeVertices[currentEdge].normal=v1->normal + delta*(v2->normal - v1->normal);
					edgeVertices[currentEdge].normal.x=v1->normal.x + delta*(v2->normal.x - v1->normal.x);
					edgeVertices[currentEdge].normal.y=v1->normal.y + delta*(v2->normal.y - v1->normal.y);
					edgeVertices[currentEdge].normal.z=v1->normal.z + delta*(v2->normal.z - v1->normal.z);
				}
			}

			//send the vertices
			for(int k=0; triTable[cubeIndex][k]!=-1; k+=3)
			{
				glNormal3fv(edgeVertices[triTable[cubeIndex][k+0]].normal);
				glVertex3fv(edgeVertices[triTable[cubeIndex][k+0]].position);

				glNormal3fv(edgeVertices[triTable[cubeIndex][k+2]].normal);
				glVertex3fv(edgeVertices[triTable[cubeIndex][k+2]].position);

				glNormal3fv(edgeVertices[triTable[cubeIndex][k+1]].normal);
				glVertex3fv(edgeVertices[triTable[cubeIndex][k+1]].position);
								
				numFacesDrawn++;
			}
		}
	}
	glEnd();
*/
}

void CUBE_GRID::FreeMemory()
{
	if(vertices)
		delete [] vertices;
	vertices=NULL;
	numVertices=0;

	if(cubes)
		delete [] cubes;
	cubes=NULL;
	numCubes=0;
}