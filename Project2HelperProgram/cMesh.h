#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <string>
#include <vector>

class cMesh
{
public:
	cMesh() : numCities(0), m_ExtentsCalculated(false){};
	struct sVertex
	{
		sVertex() : 
			x(0.0f), y(0.0f), z(0.0f),
			r(0), g(0), b(0), a(255),
			bIsCity(false){};
		float x, y, z;
		unsigned char r, g, b, a;

		bool bIsCity;
		std::string cityName;

		sVertex operator+(const sVertex& right) const;
		sVertex& operator+=(const sVertex& right);
		sVertex& operator/=(const float& right);
		static float getDistance(sVertex a, sVertex b);
	};


	struct sEdge
	{
		sEdge();
		unsigned int vertexIDs[2];
		int cost;	// Differene in height x 100 
		bool bIsRoad;

	};

	struct sTri
	{
		sTri();
		unsigned int index[3];
	};	

	void Clear(void);
	std::vector<sTri> vecTris;
	std::vector<sVertex> vecVerts;
	std::vector<sEdge> vecEdges;

	unsigned int numCities;

	std::string fileName_NO_path;
	std::string basePath;
	std::string fileName;
	bool LoadPLYFile(std::string plyFileName, std::string& lwiay);
//	bool SavePLYFile(std::string plyFileName, std::string& lwiay);

	// Done when all the meshes are loaded
	void MakeEdgeList(void);

	// Appends to existing mesh
//	void AppendHouseMesh(cMesh newMesh);


	sVertex getMin(void);
	sVertex getMax(void);
	sVertex getDeltas(void);

	unsigned int findClosestVertexToThisPoint(cMesh::sVertex vertex);

private:
	bool m_ExtentsCalculated;
	void m_CalculateExtents(void);

	sVertex m_Min;
	sVertex m_Max;
	sVertex m_Deltas;

private:
};



#endif // cMesh
