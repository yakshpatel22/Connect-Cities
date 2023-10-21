#ifndef _globalNonsense_HG_
#define _globalNonsense_HG_

#include <vector>
#include <string>

#include "cMesh.h"

namespace F3135
{
	enum eMode
	{
		GENERATE_INITIAL_DATA_Q3 = 99,
		ADD_ROADS_GENERATE_PLY_MESH,
		MAKE_SOME_RANDOM_PATHS_PLEASE,
		WT_HECK_IS_GOING_ON
	};

	const unsigned int argv_SN_INDEX = 1;
	const unsigned int argv_TERRAINFILE_0_INDEX = 2;
	const unsigned int argv_TERRAINFILE_1_INDEX = 3;
	const unsigned int argv_TERRAINFILE_2_INDEX = 4;
	const unsigned int argv_TERRAIN_DATA_INPUT_FILE_INDEX = 1;
	
	const unsigned int vecParams_SN_INDEX = 0;
	const unsigned int vecParams_TERRAINFILE_0_INDEX = 1;
	const unsigned int vecParams_TERRAINFILE_1_INDEX = 2;
	const unsigned int vecParams_TERRAINFILE_2_INDEX = 3;
	const unsigned int vecParams_TERRAIN_DATA_INPUT_FILE_INDEX = 0;

	float randBetweenRange( float low, float high );
};

bool parseCommandLine(
	int argc, char* argv[],
	std::vector<std::string> &vecParams,
	std::string &whyWeCantHaveNiceThings, 
	F3135::eMode &detectedMode);

// Geneates the file for question 3
bool genTerrainFile(
	std::string StudentNumber,
	cMesh &finalMesh, 
	std::string &IfItAllEndedInTears);

bool genPLYFileFromTerrainFile(
	std::string TerrainDataInputFile,
	std::string &IfItAllEndedInTears);

static const unsigned int NUMBER_OF_CITYS_TO_GENERATE = 10;



#endif // globalNonsense
