#include "globalNonsense.h"

#include <sstream>

F3135::eMode detectStateFromCommandLine(int argc, char* argv[])
{

	if (argc == 5)
	{
		// Project2HelperProgram.exe 
		//	8506394 
		//	Terrain_seed_24_W.ply Terrain_seed_11_J.ply Terrain_seed_16_O.ply

		std::stringstream ssNum;
		std::string sSN; sSN.append(argv[1]);
		ssNum << sSN;
		unsigned int SN = 0;
		ssNum >> SN;
		if ( SN != 0 )	// It ain't zero...
		{
			if ((sSN.length() >= 4) && (sSN.length() <= 7))
			{
				// It's an integer, between 4 and 7 digits, 
				//	so we're going to assume it's a student number

				// There's 3 more things, which we'll assume are files.
				return F3135::GENERATE_INITIAL_DATA_Q3;
			}//if ((sSN.length()
		}//if ( SN != 0 )
	}//if (argc == 5)

	if (argc == 2)
	{
		return F3135::ADD_ROADS_GENERATE_PLY_MESH;
	}//if (argc == 2)

	if (argc == 3)
	{
		if (strcmp(argv[1], "MakeSomeRandomPaths_ThanksSoMuch") == 0)
		{
			return F3135::MAKE_SOME_RANDOM_PATHS_PLEASE;
		}
		// Else...
	}

	 


	return F3135::WT_HECK_IS_GOING_ON;
}


// Could be:
// - Are passing the 
bool parseCommandLine(
	int argc, char* argv[],
	std::vector<std::string>& vecParams,
	std::string& whyWeCantHaveNiceThings, 
	F3135::eMode& detectedMode)
{
	detectedMode = detectStateFromCommandLine(argc, argv);

	switch (detectedMode)
	{
	case F3135::WT_HECK_IS_GOING_ON:
		{
			whyWeCantHaveNiceThings = "ERROR: Unknown number of parameters passed.";
			return false;
		}
		break;


	case F3135::GENERATE_INITIAL_DATA_Q3:
		{
			vecParams.push_back(argv[F3135::argv_SN_INDEX]);	// SN
			vecParams.push_back(argv[F3135::argv_TERRAINFILE_0_INDEX]);	// Terrain file 0
			vecParams.push_back(argv[F3135::argv_TERRAINFILE_1_INDEX]);	// Terrain file 1
			vecParams.push_back(argv[F3135::argv_TERRAINFILE_2_INDEX]);	// Terrain file 2
		}
		break;

	case F3135::ADD_ROADS_GENERATE_PLY_MESH:
		{
			vecParams.push_back(argv[F3135::argv_TERRAIN_DATA_INPUT_FILE_INDEX]);
		}
		break;

	case F3135::MAKE_SOME_RANDOM_PATHS_PLEASE:
		{
			// This is just for Quick-N-Dirty testing.
			// 3rd param is the name of the TerrainDataInputXXXX.txt file
			vecParams.push_back(argv[2]);	
		}
		break;

	}//switch (detectedMode)


	return true;
}
