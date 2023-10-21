#include "globalNonsense.h"
#include "cMesh.h"

#include <fstream>
#include <sstream>
#include <set>
#include <iostream>


bool genPLYFileFromTerrainFile(
	std::string TerrainDataInputFile,
	std::string& IfItAllEndedInTears)
{
	std::ifstream terrainInFile(TerrainDataInputFile.c_str());

	if ( ! terrainInFile.is_open() )
	{
		IfItAllEndedInTears = "Can't open " + TerrainDataInputFile + " file.";
		return false;
	}

	unsigned int numVertices = 0;
	unsigned int numEdges = 0;
	unsigned int numCities = 0;

	terrainInFile >> numVertices;
	terrainInFile >> numEdges;
	terrainInFile >> numCities;

	std::vector<cMesh::sVertex> vecVertices;
	std::vector<cMesh::sEdge> vecEdges;
	std::vector<cMesh::sTri> vecTris;

	std::vector<cMesh::sVertex> vecCityLocations;

	vecVertices.reserve(numVertices);
	vecEdges.reserve(numEdges);

	for ( unsigned int count = 0; count != numVertices; count++ )
	{
		cMesh::sVertex tempVert;

		// Grass green
		tempVert.r = 70;
		tempVert.g = 127;
		tempVert.b = 0;

		terrainInFile >> tempVert.x >> tempVert.y >> tempVert.z;

		// Make the terrain look like "steps"
		tempVert.x = (unsigned int)tempVert.x;
		tempVert.y = (unsigned int)tempVert.y;
		tempVert.z = (unsigned int)tempVert.z;

		unsigned int isCity = 0;
		terrainInFile >> isCity;
		tempVert.bIsCity = ( isCity ? true : false );

		terrainInFile >> tempVert.cityName;

		if ( tempVert.bIsCity )
		{
			// Red
			tempVert.r = 255;
			tempVert.g = 0;
			tempVert.b = 0;

			vecCityLocations.push_back( tempVert );
		}

		vecVertices.push_back(tempVert);
	}

	for ( unsigned int count = 0; count != numEdges; count += 3 )
	{
		cMesh::sEdge tempEdge[3];

		for ( unsigned int inCount = 0; inCount != 3; inCount++ )
		{
			terrainInFile >> tempEdge[inCount].vertexIDs[0] >> tempEdge[inCount].vertexIDs[1];
			terrainInFile >> tempEdge[inCount].cost;
			unsigned int intRoad = 0;
			terrainInFile >> intRoad;
			tempEdge[inCount].bIsRoad = (intRoad ? true : false );

			// Set the colours to black if there's a road


			if ( tempEdge[inCount].bIsRoad )
			{

//	Brown looks like crap...
//				vecVertices[tempEdge[inCount].vertexIDs[0]].r = 150;
//				vecVertices[tempEdge[inCount].vertexIDs[0]].g = 75;
//				vecVertices[tempEdge[inCount].vertexIDs[0]].b = 0;
//
//				vecVertices[tempEdge[inCount].vertexIDs[1]].r = 150;
//				vecVertices[tempEdge[inCount].vertexIDs[1]].g = 75;
//				vecVertices[tempEdge[inCount].vertexIDs[1]].b = 0;

				// But black looks OK.
				vecVertices[tempEdge[inCount].vertexIDs[0]].r = 0;
				vecVertices[tempEdge[inCount].vertexIDs[0]].g = 0;
				vecVertices[tempEdge[inCount].vertexIDs[0]].b = 0;

				vecVertices[tempEdge[inCount].vertexIDs[1]].r = 0;
				vecVertices[tempEdge[inCount].vertexIDs[1]].g = 0;
				vecVertices[tempEdge[inCount].vertexIDs[1]].b = 0;
			}


		}//for ( unsigned int inCount

		// Edges are listed in threes:
		// 0 1 -14 0 
		// 1 257 13 0
		// 257 0 1 0

		// Edges are:
		// 0 --> 1
		//	edges[0].vertexIDs[0] = curTri.index[0];
		//	edges[0].vertexIDs[1] = curTri.index[1];
		//
		// 1 --> 2
		//	edges[1].vertexIDs[0] = curTri.index[1];
		//	edges[1].vertexIDs[1] = curTri.index[2];
		// 
		// 2 --> 0
		// 2 --> 0
		//	edges[2].vertexIDs[0] = curTri.index[2];
		//	edges[2].vertexIDs[1] = curTri.index[0];

		cMesh::sTri tempTri;
		tempTri.index[0] = tempEdge[0].vertexIDs[0];
		tempTri.index[1] = tempEdge[1].vertexIDs[0];
		tempTri.index[2] = tempEdge[2].vertexIDs[0];

		vecTris.push_back(tempTri);

	}//for ( unsigned int count


	terrainInFile.close();

	// Colour the cities again, just in case
	for (unsigned int index = 0; index != numVertices; index++)
	{
		cMesh::sVertex& tempVert = vecVertices[index];
		if (tempVert.bIsCity)
		{
			// Red
			tempVert.r = 255;
			tempVert.g = 0;
			tempVert.b = 0;
		}
	}


	// Load the house model
	cMesh house;
	house.basePath = "Models";
	std::string errorString;
	if ( ! house.LoadPLYFile("House_1-500.ply", errorString) )
	{
		std::cout << "Error: Didn't load house mesh file because " << errorString << std::endl;
	}

	// Add house to output mesh
	for ( unsigned int houseCount = 0; houseCount != numCities; houseCount++ )
	{
		unsigned int vertIndexOffset = (unsigned int)vecVertices.size();

		cMesh::sVertex &houseBaseLocation = vecCityLocations[houseCount];

		for ( unsigned int vertIndex = 0; vertIndex != house.vecVerts.size(); vertIndex++ )
		{
			// Make the house yellow
			cMesh::sVertex curHouseVert = house.vecVerts[vertIndex];

			curHouseVert.r = 255;
			curHouseVert.g = 255;
			curHouseVert.b = 0;
			curHouseVert.a = 255;

			curHouseVert.x += houseBaseLocation.x;
			curHouseVert.y += houseBaseLocation.y;
			curHouseVert.z += houseBaseLocation.z;
			// Place the house "just" above the terrain
			curHouseVert.z += 2.0f;

			vecVertices.push_back(curHouseVert);
		}

		// Add the triangles for this house
		for ( unsigned int triCount = 0; triCount != house.vecTris.size(); triCount++ )
		{
			cMesh::sTri curTri = house.vecTris[triCount];
			
			curTri.index[0] += vertIndexOffset;
			curTri.index[1] += vertIndexOffset;
			curTri.index[2] += vertIndexOffset;

			vecTris.push_back(curTri);
		}

	}//for (...houseCount

	
	std::stringstream ssPLYFileName;
	ssPLYFileName << "Terrain_Output.ply";
	std::ofstream plyFile(ssPLYFileName.str().c_str());

	if (!plyFile.is_open())
	{
		IfItAllEndedInTears = "Can't open " + ssPLYFileName.str() + " file for writing.";
		return false;
	}

	// Header
	plyFile << "ply" << std::endl;
	plyFile << "format ascii 1.0" << std::endl;
	plyFile << "comment VCGLIB generated" << std::endl;
	plyFile << "element vertex ";

	plyFile << vecVertices.size() << std::endl;

	plyFile << "property float x" << std::endl;
	plyFile << "property float y" << std::endl;
	plyFile << "property float z" << std::endl;
	plyFile << "property uchar red" << std::endl;
	plyFile << "property uchar green" << std::endl;
	plyFile << "property uchar blue" << std::endl;
	plyFile << "property uchar alpha" << std::endl;
	plyFile << "element face ";	
	
	plyFile << (unsigned int)vecTris.size() << std::endl;

	plyFile << "property list uchar int vertex_indices" << std::endl;
	plyFile << "end_header" << std::endl;

	for ( unsigned int index = 0; index != vecVertices.size(); index++ )
	{
		plyFile << vecVertices[index].x << " ";
		plyFile << vecVertices[index].y << " ";
		plyFile << vecVertices[index].z << " ";
		plyFile << (unsigned int)vecVertices[index].r << " ";
		plyFile << (unsigned int)vecVertices[index].g << " ";
		plyFile << (unsigned int)vecVertices[index].b << " ";
		plyFile << (unsigned int)vecVertices[index].a << std::endl;
	}

	for ( unsigned int index = 0; index != vecTris.size(); index++ )
	{
		plyFile << "3 ";
		plyFile << vecTris[index].index[0] << " ";
		plyFile << vecTris[index].index[1] << " ";
		plyFile << vecTris[index].index[2] << std::endl;
	}


	plyFile.close();

	return true;
}

