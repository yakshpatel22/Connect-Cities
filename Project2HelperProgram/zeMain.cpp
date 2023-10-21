// Written in November 2020 for the Fall 2020 INFO-3135 "Data Structures" course
// at Fanshawe College. 
// by Michael Feeney
// mfeeney (at) fanshawec (dot) ca



#include "globalNonsense.h"
#include "cMesh.h"
#include <iostream>
#include <sstream>
#include <float.h>

void PrintUsage(void);

void LoadCityNames(std::vector<std::string> &vecCityNames);
std::string pickARandomString(std::vector<std::string> &vecCityNames);
void PlaceSomeCities(cMesh &terrainMesh, std::string sSN, unsigned int& citiesTested);


int main(int argc, char* argv[])
{
	F3135::eMode detectedMode = F3135::WT_HECK_IS_GOING_ON;
	std::vector<std::string> vecParams;
	std::string whyWeCantHaveNiceThings;


	if ( ! parseCommandLine(argc, argv, vecParams, whyWeCantHaveNiceThings, detectedMode) )
	{
		std::cout << whyWeCantHaveNiceThings << std::endl;
		std::cout << std::endl;
		PrintUsage();
		
		// Early exit
		return -1;

	}//if (!parseCommandLine

	cMesh terrainFiles[3];
	std::string errorString;

	switch (detectedMode)
	{
	case F3135::GENERATE_INITIAL_DATA_Q3:
		{
			std::cout << "Detected mode is: GENERATE_INITIAL_DATA_Q3" << std::endl;
			std::cout << std::endl;

			terrainFiles[0].basePath = "Models";
			terrainFiles[1].basePath = "Models";
			terrainFiles[2].basePath = "Models";

			std::cout << "Loading " << vecParams[F3135::vecParams_TERRAINFILE_0_INDEX] << "...";
			if ( !terrainFiles[0].LoadPLYFile(vecParams[F3135::vecParams_TERRAINFILE_0_INDEX], errorString))
			{
				std::cout << "Error: " << errorString << std::endl;
				return -1;
			}
			std::cout << "done." << std::endl;

			std::cout << "Loading " << vecParams[F3135::vecParams_TERRAINFILE_1_INDEX] << "...";
			if ( !terrainFiles[1].LoadPLYFile(vecParams[F3135::vecParams_TERRAINFILE_1_INDEX], errorString))
			{
				std::cout << "Error: " << errorString << std::endl;
				return -1;
			}	
			std::cout << "done." << std::endl;

			std::cout << "Loading " << vecParams[F3135::vecParams_TERRAINFILE_2_INDEX] << "...";
			if ( !terrainFiles[2].LoadPLYFile(vecParams[F3135::vecParams_TERRAINFILE_2_INDEX], errorString))
			{
				std::cout << "Error: " << errorString << std::endl;
				return -1;
			}	
			std::cout << "done." << std::endl;

			std::cout << std::endl;

			std::cout << "Combining meshes...";

			// Assume the edges are valid for terrain mesh #0.
			// Take averages for each mesh vertex. 
			// See, it doesn't actually matter if the vertices match, in space, 
			//	since we are just taking the averages... 
			for ( unsigned int vertIndex = 0; vertIndex != terrainFiles[0].vecVerts.size(); vertIndex++ ) 
			{
				terrainFiles[0].vecVerts[vertIndex] 
					+= terrainFiles[1].vecVerts[vertIndex] + terrainFiles[2].vecVerts[vertIndex];

				terrainFiles[0].vecVerts[vertIndex] /= 3.0f;
			}//for ( unsigned int vertIndex

			terrainFiles[0].MakeEdgeList();

			std::cout << "done." << std::endl;

			std::cout << "Placing cities..." << std::endl;
			unsigned int citiesTested = 0;
			PlaceSomeCities(terrainFiles[0], vecParams[F3135::vecParams_SN_INDEX], citiesTested);
			std::cout << "...done (tried " << citiesTested << " locations)." << std::endl;


			std::cout << "Generating TerrainDataInputXXX file...";
			if ( ! genTerrainFile(vecParams[F3135::vecParams_SN_INDEX], terrainFiles[0], errorString) )
			{
				std::cout << "Error: " << errorString << std::endl;
				return -1;
			}
			
			std::cout << "done." << std::endl;

		}
		break;
	
	case F3135::ADD_ROADS_GENERATE_PLY_MESH:
		{
			std::cout << "Detected mode is: ADD_ROADS_GENERATE_PLY_MESH" << std::endl;
			std::cout << std::endl;

			std::cout << "Generating terrain file...";
			if ( ! genPLYFileFromTerrainFile(vecParams[F3135::vecParams_TERRAIN_DATA_INPUT_FILE_INDEX], errorString) )
			{
				std::cout << "Error: " << errorString << std::endl;
				return -1;
			}
			std::cout << "done." << std::endl;
			std::cout << std::endl;

		}
		break;


	}//switch (detectedMode)

	return 0;
}


void PrintUsage(void)
{
	std::cout
		<< "There's 2 modes, where you pass these parameters" << std::endl
		<< std::endl
		<< "1. Making the TerrainDataInputXXX.txt file, where you pass:" << std::endl
		<< "\tYourStudentNumber "
		<< "three_terrain_files" << std::endl
		<< std::endl
		<< "Example: " << std::endl
		<< std::endl
		<< "Project2HelperProgram.exe 8506394 Terrain_seed_24_W.ply Terrain_seed_11_J.ply Terrain_seed_16_O.ply" << std::endl
		<< std::endl
		<< std::endl

		<< "2. Adding a road to generate a mesh (ply) file, where you pass:" << std::endl
		<< std::endl
		<< "\tTerrainDataInputXXX_File RoadEdgeList_File" << std::endl
		<< std::endl
		<< "Example: " << std::endl
		<< std::endl
		<< "Project2HelperProgram.exe TerrainDataInput8506394.txt MyRoads.txt" << std::endl
		<< std::endl
		<< std::endl
		<< "Sorry it didn't work out." << std::endl
		<< "Stay safe" << std::endl
		<< std::endl;

	return;
}



void PlaceSomeCities(cMesh& terrainMesh, std::string sSN, unsigned int &citiesTested)
{
	std::stringstream ssSN;
	ssSN << sSN;
	unsigned int SN = 0;
	ssSN >> SN;

	struct sCityInfo
	{
		sCityInfo() : vertIndex(0) {};
		cMesh::sVertex location;
		unsigned int vertIndex;
	};
	std::vector<sCityInfo> vecCities;

	// Picking some city names
	std::vector<std::string> vecCityNames;
	LoadCityNames(vecCityNames);


	srand(SN);

	float limit10PercentX = terrainMesh.getMin().x + (terrainMesh.getDeltas().x * 0.1f);
	float limit10PercentY = terrainMesh.getMin().y + (terrainMesh.getDeltas().y * 0.1f);
	float limit90PercentX = terrainMesh.getMin().x + (terrainMesh.getDeltas().x * 0.9f);
	float limit90PercentY = terrainMesh.getMin().y + (terrainMesh.getDeltas().y * 0.9f);
	citiesTested = 0;
	do 
	{
		citiesTested++;

		sCityInfo testCity;

		// Pick a random vertex
		testCity.location.x = F3135::randBetweenRange(limit10PercentX, limit90PercentX);
		testCity.location.y = F3135::randBetweenRange(limit10PercentX, limit90PercentX);

		// Assume it's OK...
		bool bCityIsOK = true;

		// Is this "too close" to another city? 
		// To close is closer than 15% of the width of the mesh 
		float tooCloseDistance = terrainMesh.getDeltas().x * 0.15f;

		for ( unsigned int index = 0; index != vecCities.size(); index++ )
		{
			float distance = cMesh::sVertex::getDistance(testCity.location, vecCities[index].location);

			if ( distance < tooCloseDistance ) { bCityIsOK = false; }
		}

		if ( bCityIsOK )
		{
			// What's the closest vertex?
			testCity.vertIndex = terrainMesh.findClosestVertexToThisPoint(testCity.location);
			// Adjust the city location to match vertex
			testCity.location = terrainMesh.vecVerts[testCity.vertIndex];

			// Add it to the running total
			vecCities.push_back(testCity);
			terrainMesh.vecVerts[testCity.vertIndex].bIsCity = true;
			std::string cityName = pickARandomString(vecCityNames);
			terrainMesh.vecVerts[testCity.vertIndex].cityName = cityName;
			std::cout 
				<< cityName << " at "
				<< testCity.location.x << ", "
				<< testCity.location.y << ", "
				<< testCity.location.z << std::endl;
		}

	}while ( vecCities.size() < NUMBER_OF_CITYS_TO_GENERATE);

	terrainMesh.numCities = (unsigned int)vecCities.size();

	return;
}

float F3135::randBetweenRange(float low, float high)
{
	return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
}

std::string pickARandomString(std::vector<std::string>& vecCityNames)
{
	unsigned int index = rand() % (unsigned int)vecCityNames.size();
	return vecCityNames[index];
}

void LoadCityNames(std::vector<std::string>& vecCityNames)
{
	vecCityNames.clear();

	vecCityNames.push_back("Alston");
	vecCityNames.push_back("Amble");
	vecCityNames.push_back("Ambleside");
	vecCityNames.push_back("Amersham");
	vecCityNames.push_back("Appleby-in-Westmorland");
	vecCityNames.push_back("Barkerville");
	vecCityNames.push_back("Barnsley");
	vecCityNames.push_back("Barnstaple");
	vecCityNames.push_back("Barrie");
	vecCityNames.push_back("Bathurst");
	vecCityNames.push_back("Batoche");
	vecCityNames.push_back("Belleville");
	vecCityNames.push_back("Biddulph");
	vecCityNames.push_back("Blackwater");
	vecCityNames.push_back("Bootle");
	vecCityNames.push_back("Borden");
	vecCityNames.push_back("Bradninch");
	vecCityNames.push_back("Brantford");
	vecCityNames.push_back("Bridge");
	vecCityNames.push_back("Brigg");
	vecCityNames.push_back("Brockville");
	vecCityNames.push_back("Brooks");
	vecCityNames.push_back("Buckingham");
	vecCityNames.push_back("Budleigh");
	vecCityNames.push_back("Burlington");
	vecCityNames.push_back("Burnaby");
	vecCityNames.push_back("Cambridge");
	vecCityNames.push_back("Carshalton");
	vecCityNames.push_back("Chambly");
	vecCityNames.push_back("Channel-Port");
	vecCityNames.push_back("Charlottetown");
	vecCityNames.push_back("Chatham-Kent");
	vecCityNames.push_back("Chertsey");
	vecCityNames.push_back("Chibougamau");
	vecCityNames.push_back("Chippenham");
	vecCityNames.push_back("Chudleigh");
	vecCityNames.push_back("Cleckheaton");
	vecCityNames.push_back("Cornwall");
	vecCityNames.push_back("Corringham");
	vecCityNames.push_back("Courtenay");
	vecCityNames.push_back("Coventry");
	vecCityNames.push_back("Craven");
	vecCityNames.push_back("Cullompton");
	vecCityNames.push_back("Cumberland");
	vecCityNames.push_back("Darlington");
	vecCityNames.push_back("Dauphin");
	vecCityNames.push_back("Dawson");
	vecCityNames.push_back("Delta");
	vecCityNames.push_back("Denholme");
	vecCityNames.push_back("Dorking");
	vecCityNames.push_back("Dorval");
	vecCityNames.push_back("Downham");
	vecCityNames.push_back("Drayton");
	vecCityNames.push_back("Edgware");
	vecCityNames.push_back("Elliot");
	vecCityNames.push_back("Epsom");
	vecCityNames.push_back("Epworth");
	vecCityNames.push_back("Erith");
	vecCityNames.push_back("Esquimalt");
	vecCityNames.push_back("Estevan");
	vecCityNames.push_back("Ferryland");
	vecCityNames.push_back("Fordbridge");
	vecCityNames.push_back("Fort");
	vecCityNames.push_back("Fowey");
	vecCityNames.push_back("Gains");
	vecCityNames.push_back("Gander");
	vecCityNames.push_back("Glasgow");
	vecCityNames.push_back("Godmanchester");
	vecCityNames.push_back("Granby");
	vecCityNames.push_back("Grand");
	vecCityNames.push_back("Grande");
	vecCityNames.push_back("Grantham");
	vecCityNames.push_back("Guelph");
	vecCityNames.push_back("Halifax");
	vecCityNames.push_back("Hamilton");
	vecCityNames.push_back("Harbour");
	vecCityNames.push_back("Hednesford");
	vecCityNames.push_back("Helston");
	vecCityNames.push_back("Hertford");
	vecCityNames.push_back("Heywood");
	vecCityNames.push_back("Hull");
	vecCityNames.push_back("Ilkley");
	vecCityNames.push_back("Inuvik");
	vecCityNames.push_back("Ivybridge");
	vecCityNames.push_back("Kamloops");
	vecCityNames.push_back("Kawartha");
	vecCityNames.push_back("Kimberley");
	vecCityNames.push_back("Kirkland");
	vecCityNames.push_back("Kirton-in-Lindsey");
	vecCityNames.push_back("Kitchener");
	vecCityNames.push_back("Kitimat");
	vecCityNames.push_back("Knutsford");
	vecCityNames.push_back("Lake");
	vecCityNames.push_back("Lancaster");
	vecCityNames.push_back("Laurentian");
	vecCityNames.push_back("Laval");
	vecCityNames.push_back("Leighton-Linslade");
	vecCityNames.push_back("Lethbridge");
	vecCityNames.push_back("London");
	vecCityNames.push_back("Longueuil");
	vecCityNames.push_back("Louisbourg");
	vecCityNames.push_back("Lunenburg");
	vecCityNames.push_back("Lytham");
	vecCityNames.push_back("Marazion");
	vecCityNames.push_back("Medicine");
	vecCityNames.push_back("Middleham");
	vecCityNames.push_back("Milton");
	vecCityNames.push_back("Minehead");
	vecCityNames.push_back("Miramichi");
	vecCityNames.push_back("Mitcham");
	vecCityNames.push_back("Montr");
	vecCityNames.push_back("Nailsea");
	vecCityNames.push_back("Nanaimo");
	vecCityNames.push_back("New");
	vecCityNames.push_back("Newhaven");
	vecCityNames.push_back("Newry");
	vecCityNames.push_back("Niagara");
	vecCityNames.push_back("North");
	vecCityNames.push_back("Northwich");
	vecCityNames.push_back("Oldham");
	vecCityNames.push_back("Ongar");
	vecCityNames.push_back("Orillia");
	vecCityNames.push_back("Ottawa");
	vecCityNames.push_back("Paddock");
	vecCityNames.push_back("Painswick");
	vecCityNames.push_back("Perc");
	vecCityNames.push_back("Perth");
	vecCityNames.push_back("Plymouth");
	vecCityNames.push_back("Prince");
	vecCityNames.push_back("Quebec");
	vecCityNames.push_back("Quedgeley");
	vecCityNames.push_back("Quesnel");
	vecCityNames.push_back("Radstock");
	vecCityNames.push_back("Red");
	vecCityNames.push_back("Redhill");
	vecCityNames.push_back("Rimouski");
	vecCityNames.push_back("Rouyn-Noranda");
	vecCityNames.push_back("Rowley");
	vecCityNames.push_back("Royal");
	vecCityNames.push_back("Royston");
	vecCityNames.push_back("Saffron");
	vecCityNames.push_back("Saguenay");
	vecCityNames.push_back("Saint");
	vecCityNames.push_back("Sainte-Anne-de-Beaupr");
	vecCityNames.push_back("Sainte-Foy");
	vecCityNames.push_back("Saint-Hubert");
	vecCityNames.push_back("Salisbury");
	vecCityNames.push_back("Saxmundham");
	vecCityNames.push_back("Scarborough");
	vecCityNames.push_back("Sherbrooke");
	vecCityNames.push_back("Smethwick");
	vecCityNames.push_back("Souris");
	vecCityNames.push_back("Southwell");
	vecCityNames.push_back("Stocksbridge");
	vecCityNames.push_back("Stonehouse");
	vecCityNames.push_back("Swan");
	vecCityNames.push_back("Sydney");
	vecCityNames.push_back("Thunder");
	vecCityNames.push_back("Toronto");
	vecCityNames.push_back("Trenton");
	vecCityNames.push_back("Truro");
	vecCityNames.push_back("Tunbridge");
	vecCityNames.push_back("Tynemouth");
	vecCityNames.push_back("Uppingham");
	vecCityNames.push_back("Uranium");
	vecCityNames.push_back("Uxbridge");
	vecCityNames.push_back("Val-d");
	vecCityNames.push_back("Vancouver");
	vecCityNames.push_back("Vernon");
	vecCityNames.push_back("Wabana");
	vecCityNames.push_back("Wantage");
	vecCityNames.push_back("Watchet");
	vecCityNames.push_back("Watson");
	vecCityNames.push_back("Welland");
	vecCityNames.push_back("West");
	vecCityNames.push_back("Westminster");
	vecCityNames.push_back("Whaley");
	vecCityNames.push_back("White");
	vecCityNames.push_back("Wickwar");
	vecCityNames.push_back("Widnes");
	vecCityNames.push_back("Wigton");
	vecCityNames.push_back("Windsor");
	vecCityNames.push_back("Winnipeg");
	vecCityNames.push_back("Wiveliscombe");
	vecCityNames.push_back("Woburn");
	vecCityNames.push_back("Wokingham");
	vecCityNames.push_back("Yateley");
	vecCityNames.push_back("Yellowknife");
	vecCityNames.push_back("London");
	vecCityNames.push_back("Toronto");

	return;
}

