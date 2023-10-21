#include "cMesh.h"
#include <fstream>

namespace F3135
{
	bool AbsorbToThisToken(std::ifstream& theStream, std::string token, std::string &lwiay)
	{
		std::string nextToken;
		while (theStream >> nextToken)
		{
			if (nextToken == token)
			{
				return true;
			}
		}
		// Didn't find that token
		lwiay = "Didn't find token >" + token + "<";
		return false;
	}//bool AbsorbToThisToken()
};


cMesh::sTri::sTri()
{
	memset(this->index, 3, 0);
}

cMesh::sEdge::sEdge()
{
	memset(this->vertexIDs, 2, 0);
	this->cost = 0;
	this->bIsRoad = false;
}

void cMesh::Clear(void)
{
	this->vecEdges.clear();
	this->vecTris.clear();
	this->vecVerts.clear();
	return;
}

cMesh::sVertex cMesh::sVertex::operator+(const cMesh::sVertex& right) const
{
	sVertex result;
	result.x = this->x + right.x;
	result.y = this->y + right.y;
	result.z = this->z + right.z;
	return result;
}

cMesh::sVertex& cMesh::sVertex::operator+=(const cMesh::sVertex& right)
{
	this->x += right.x;
	this->y += right.y;
	this->z += right.z;
	return *this;
}

cMesh::sVertex& cMesh::sVertex::operator/=(const float& right)
{
	this->x /= right;
	this->y /= right;
	this->z /= right;
	return *this;
}

//static 
float cMesh::sVertex::getDistance(cMesh::sVertex a, cMesh::sVertex b)
{
	double deltaX = (double)a.x - (double)b.x;
	double deltaY = (double)a.y - (double)b.y;
	double deltaZ = (double)a.z - (double)b.z;
	return (float)sqrt( (deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ) );
}



bool cMesh::LoadPLYFile(std::string plyFileName, std::string &lwiay)
{
	this->fileName_NO_path = plyFileName;

	// Make path
	if ( this->basePath == "" )
	{	// No base path
		this->fileName = this->fileName_NO_path;
	}
	else
	{
		this->fileName = this->basePath + "/" + this->fileName_NO_path;
	}


	std::ifstream theFile(this->fileName.c_str());
	if ( ! theFile.is_open() )
	{ 
		lwiay = "Can't open " + this->fileName + " file";
		return false;
	}

	this->Clear();

	// Header : vertices
	if ( ! F3135::AbsorbToThisToken(theFile, "vertex", lwiay) )
	{
		lwiay += " (At header read of " + this->fileName + ")";
		return false;
	}
	unsigned int numVertices = 0;
	theFile >> numVertices;

	// Header : faces
	if ( ! F3135::AbsorbToThisToken(theFile, "face", lwiay) )
	{
		lwiay += " (At header read of " + this->fileName + ")";
		return false;
	}
	unsigned int numFaces = 0;
	theFile >> numFaces;

	// Header : end_header
	if ( ! F3135::AbsorbToThisToken(theFile, "end_header", lwiay) )
	{
		lwiay += " (At header read of " + this->fileName + ")";
		return false;
	}	
	
	this->vecVerts.reserve(numVertices);
	this->vecTris.reserve(numFaces);

	for (unsigned int index = 0; index != numVertices; index++)
	{
		cMesh::sVertex tempVert;
		theFile >> tempVert.x >> tempVert.y >> tempVert.z;
		this->vecVerts.push_back(tempVert);
	}

	for (unsigned int index = 0; index != numFaces; index++)
	{
		cMesh::sTri tempTri;
		unsigned int discard = 0;
		theFile >> discard >> tempTri.index[0] >> tempTri.index[1] >> tempTri.index[2];
		this->vecTris.push_back(tempTri);
	}

	
	return true;
}

void cMesh::MakeEdgeList(void)
{
	// Geneate edge list
	for (unsigned int index = 0; index != this->vecTris.size(); index++)
	{
		cMesh::sTri& curTri = this->vecTris[index];

		// Triangle has 3 edges, yo: 0, 1, and 2
		cMesh::sEdge edges[3];

		// Edges are:
		// 0 --> 1
		// 1 --> 2
		// 2 --> 0

		// 0 --> 1
		edges[0].vertexIDs[0] = curTri.index[0];
		edges[0].vertexIDs[1] = curTri.index[1];

		float heightDelta =
			this->vecVerts[curTri.index[1]].z - this->vecVerts[curTri.index[0]].z;
		edges[0].cost = (int)(heightDelta * 100.0f);


		// 1 --> 2
		edges[1].vertexIDs[0] = curTri.index[1];
		edges[1].vertexIDs[1] = curTri.index[2];
		heightDelta = this->vecVerts[curTri.index[2]].z - this->vecVerts[curTri.index[1]].z;
		edges[1].cost = (int)(heightDelta * 100.0f);

		// 2 --> 0
		edges[2].vertexIDs[0] = curTri.index[2];
		edges[2].vertexIDs[1] = curTri.index[0];
		heightDelta = this->vecVerts[curTri.index[0]].z - this->vecVerts[curTri.index[2]].z;
		edges[2].cost = (int)(heightDelta * 100.0f);

		this->vecEdges.push_back(edges[0]);
		this->vecEdges.push_back(edges[1]);
		this->vecEdges.push_back(edges[2]);
	}
	return;
}

cMesh::sVertex cMesh::getMin(void)
{
	if ( ! this->m_ExtentsCalculated )
	{
		this->m_CalculateExtents();
	}

	return this->m_Min;
}

cMesh::sVertex cMesh::getMax(void)
{
	if (!this->m_ExtentsCalculated)
	{
		this->m_CalculateExtents();
	}

	return this->m_Max;
}

cMesh::sVertex cMesh::getDeltas(void)
{
	if (!this->m_ExtentsCalculated)
	{
		this->m_CalculateExtents();
	}

	return this->m_Deltas;
}


void cMesh::m_CalculateExtents(void)
{
	this->m_Min = this->vecVerts[0];
	this->m_Max = this->vecVerts[0];
	

	for ( unsigned int index = 0; index != this->vecVerts.size(); index++ )
	{
		cMesh::sVertex& curVert = this->vecVerts[index];

		if ( curVert.x < this->m_Min.x )	{ this->m_Min.x = curVert.x; }
		if ( curVert.y < this->m_Min.y )	{ this->m_Min.y = curVert.y; }
		if ( curVert.z < this->m_Min.z )	{ this->m_Min.z = curVert.z; }

		if ( curVert.x > this->m_Max.x )	{ this->m_Max.x = curVert.x; }
		if ( curVert.y > this->m_Max.y )	{ this->m_Max.y = curVert.y; }
		if ( curVert.z > this->m_Max.z )	{ this->m_Max.z = curVert.z; }
	}

	this->m_Deltas.x = this->m_Max.x - this->m_Min.x;
	this->m_Deltas.y = this->m_Max.y - this->m_Min.y;
	this->m_Deltas.z = this->m_Max.z - this->m_Min.z;

	this->m_ExtentsCalculated = true;

	return;
}

unsigned int cMesh::findClosestVertexToThisPoint(cMesh::sVertex vertex)
{
	float minDistance = FLT_MAX;
	unsigned int closestVertex = 0;

	for (unsigned int index = 0; index != this->vecVerts.size(); index++)
	{
		float curDistance = cMesh::sVertex::getDistance(this->vecVerts[index], vertex);
		if (curDistance < minDistance)
		{
			// Closest city... so far
			minDistance = curDistance;
			closestVertex = index;
		}
	}//for ( unsigned int index

	return closestVertex;
}