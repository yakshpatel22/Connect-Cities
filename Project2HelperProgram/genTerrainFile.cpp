#include "globalNonsense.h"
#include <sstream>
#include <fstream>
#include <set>

// This was just used for testing the PLY file generator
void MakeSomeRandomPaths(cMesh& terrainMesh);
#include <iostream>


bool genTerrainFile(
	std::string StudentNumber,
	cMesh& finalMesh,
	std::string& IfItAllEndedInTears)
{
	std::stringstream ssFileName;
	ssFileName << "TerrainDataInput" << StudentNumber << ".txt";

	std::string fileName = ssFileName.str();

	std::ofstream theFile(fileName.c_str());
	if ( ! theFile.is_open() )
	{
		IfItAllEndedInTears = "Error: Can't open " + fileName + " for output.";
		return false;
	}

// ***************************************
// **** FOR TESTING THE PLY GENERATOR ****
//	MakeSomeRandomPaths(finalMesh);
// ***************************************


	// Format:
	//  numberOfVertices (unsigned int)
	//  numberOfEdges (unsigned int)
	//	numberOfCities (unsigned int)
	//  { the vertices	
	//    x, y, height, isCity, cityName
	//
	//    isCity is a "boolean int", 0 or 1 where 0 = "false" or 1 = "true"
	//    cityName is single string word (no spaces) if a city or "x" if not
	//  }
	// 
	//  { the edges
	//    vertexID 0, vertex ID 1, cost, isRoad
	// 
	//    vertexIDs are unsigned ints
	//    cost is an int (can be negative)
	//    isRoad is a "boolean int", 0 or 1 where 0 = "false" or 1 = "true"
	//  }

	theFile << finalMesh.vecVerts.size() << std::endl;

	theFile << finalMesh.vecEdges.size() << std::endl;

	theFile << finalMesh.numCities << std::endl;

	for ( unsigned int index = 0; index != finalMesh.vecVerts.size(); index++ )
	{
		cMesh::sVertex &curVert = finalMesh.vecVerts[index];

		theFile 
			<< curVert.x << " " 
			<< curVert.y << " " 
			<< curVert.z << " ";

		theFile << ( curVert.bIsCity ? 1 : 0 ) << " ";

		theFile << ( curVert.bIsCity ? curVert.cityName : "x" );

		theFile << std::endl;
		
	}//for...


	for ( unsigned int index = 0; index != finalMesh.vecEdges.size(); index++ )
	{
		cMesh::sEdge &curEdge = finalMesh.vecEdges[index];

		theFile 
			<< curEdge.vertexIDs[0] << " "
			<< curEdge.vertexIDs[1] << " ";

		theFile << curEdge.cost << " ";

		if ( curEdge.bIsRoad )
		{
			theFile << 1 << " ";
		}
		else
		{
			theFile << 0 << " ";
		}

		theFile << std::endl;

	}//for...
	
	theFile.close();

	return true;
}



void getConnectedVertices(
	cMesh& zeMesh, unsigned int testVertIndex,
	std::vector<unsigned int>& vecConnectedVerts,
	bool bOnlyDirected = false)
{
	std::set<unsigned int> setConnectedVerts;

	for (unsigned int index = 0; index != zeMesh.vecEdges.size(); index++)
	{
		if (zeMesh.vecEdges[index].vertexIDs[0] == testVertIndex)
		{
			setConnectedVerts.insert(zeMesh.vecEdges[index].vertexIDs[1]);

			if (bOnlyDirected == false)
			{
				// Add the other way, too
				if (zeMesh.vecEdges[index].vertexIDs[1] == testVertIndex)
				{
					setConnectedVerts.insert(zeMesh.vecEdges[index].vertexIDs[0]);
				}
			}//if (bOnlyDirected == false)

		}//for (...index

	}//for (...stepCount

	vecConnectedVerts.clear();
	std::copy(setConnectedVerts.begin(), setConnectedVerts.end(), std::back_inserter(vecConnectedVerts));

	return;
}

// Returns vecEdges.size() if not found.
// preferred = -1 is pick random
unsigned int findConnectedEdgeWithoutRoad(cMesh& zeMesh, unsigned int startVertexIndex, int preferred = -1)
{
	std::set<unsigned int> setGoodEdges;

	for (unsigned int edgeIndex = 0; edgeIndex != zeMesh.vecEdges.size(); edgeIndex++)
	{
		if ((zeMesh.vecEdges[edgeIndex].vertexIDs[0] == startVertexIndex) ||
			(zeMesh.vecEdges[edgeIndex].vertexIDs[1] == startVertexIndex))
		{
			// Has a road alredy? 
			if (!zeMesh.vecEdges[edgeIndex].bIsRoad)
			{
				// Nope, so this one is good. 
				setGoodEdges.insert(edgeIndex);
			}
		}
	}

	// Did we find any?
	if ( setGoodEdges.empty() )
	{
		return (unsigned int)zeMesh.vecEdges.size();
	}

	std::vector<unsigned int> vecEdges;
	std::copy(setGoodEdges.begin(), setGoodEdges.end(), std::back_inserter(vecEdges));

	if ( preferred == -1 )
	{
		// Now pick a random one
		return vecEdges[rand() % vecEdges.size()];
	}
	// Pick a specific one
	if ( preferred > (int)vecEdges.size() )
	{
		preferred %= (int)vecEdges.size();
	}

	return vecEdges[preferred];
}

void MakeSomeRandomPaths(cMesh& terrainMesh)
{
	unsigned int numRoads = (rand() % 10) + 5;

	std::cout << "Generating roads starting." << std::endl;
	for (unsigned int roadCount = 0; roadCount != numRoads; roadCount++)
	{
		// Pick a vertex
		cMesh::sVertex randVert;
		randVert.x = F3135::randBetweenRange(terrainMesh.getMin().x, terrainMesh.getMax().x);
		randVert.y = F3135::randBetweenRange(terrainMesh.getMin().y, terrainMesh.getMax().y);
		randVert.z = F3135::randBetweenRange(terrainMesh.getMin().z, terrainMesh.getMax().z);

		unsigned int closestVertIndex = terrainMesh.findClosestVertexToThisPoint(randVert);

		unsigned int numStepsInPath = (rand() % 150) + 25;

		std::cout << "Making road #" << roadCount << "...";
		unsigned int stepCount = 0;
		int direction = -1;	// Random direction to start
		int directedStepCount = (rand() % 20) + 5;
		for (; stepCount != numStepsInPath; stepCount++)
		{
			std::vector<unsigned int> vecConnectedVerts;


			directedStepCount--;
			if ( directedStepCount == 0 )
			{
				direction = -1;	
			}
			// Pick one
			unsigned int goodEdgeID = findConnectedEdgeWithoutRoad(terrainMesh, closestVertIndex, direction);

			if ( direction == -1 )
			{
				// Pick a random direction
				direction = rand() % 20;	
				directedStepCount = (rand() % 20) + 5;
			}


			// If we DIDN'T find one, then there's no where to go from here, so quit
			if (goodEdgeID == terrainMesh.vecEdges.size())
			{
				// Dead end
				break;
			}
			else
			{
				// Mark it, baby!
				terrainMesh.vecEdges[goodEdgeID].bIsRoad = true;
			}

			// Move to that vertex
			if ( terrainMesh.vecEdges[goodEdgeID].vertexIDs[0] == closestVertIndex )
			{
				closestVertIndex = terrainMesh.vecEdges[goodEdgeID].vertexIDs[1];
			}
			else
			{	// Pick the "other" one
				closestVertIndex = terrainMesh.vecEdges[goodEdgeID].vertexIDs[0];
			}

		}//for (...stepCount
		
		if ( stepCount == numStepsInPath )
		{
			std::cout << "done, after " << stepCount << " steps." << std::endl;
		}
		else
		{
			std::cout << "done, after " << stepCount << " steps (dead end)." << std::endl;
		}

	}//for (...pathCount
	std::cout << "Road generation is done." << std::endl;

	//std::cout << "Writing file...";
	//std::string errorString;
	//genTerrainFile("TESTSTUDENT", terrainMesh, errorString);
	//std::cout << "done." << std::endl;

	return;
}
