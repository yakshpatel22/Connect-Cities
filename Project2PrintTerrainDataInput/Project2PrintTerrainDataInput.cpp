// Project2TerrainDataInput.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <float.h>
#include <vector>
#include <Windows.h>

void PrintUsage(void);

// Global variables
std::ifstream terrainDataInputFile;

int main(int argc, char* argv[])
{
	unsigned int vertices = 0, edges = 0, cities = 0, startVertex = 0, endVertex = 0;
	int cost = 0;
	float X = 0.0f, Y = 0.0f, Z = 0.0f;
	bool isCity = false, isRoad = false;
	std::string city;

	if (argc != 2) {
		std::cout << "Error: Improper usage.." << std::endl;
		PrintUsage();
		return -1;
	}

	std::string fileName(argv[1]);

	terrainDataInputFile.open(fileName, std::ifstream::in);

	if (!terrainDataInputFile.is_open())
	{
		std::cout << "Error: Can't open '" + fileName + "' for input.";
		return -1;
	}

	if (terrainDataInputFile.good()) terrainDataInputFile >> vertices;
	if (terrainDataInputFile.good()) terrainDataInputFile >> edges;
	if (terrainDataInputFile.good()) terrainDataInputFile >> cities;

	std::cout << vertices << std::endl;
	std::cout << edges << std::endl;
	std::cout << cities << std::endl;

	while (vertices--) {
		terrainDataInputFile >> X >> Y >> Z >> isCity >> city;
		std::cout << X << " " << Y << " " << Z << " " << isCity << " " << city << std::endl;
	}

	while (edges--) {
		terrainDataInputFile >> startVertex >> endVertex >> cost >> isRoad;
		std::cout << startVertex << " " << endVertex << " " << cost << " " << isRoad << " " << std::endl;
	}

	terrainDataInputFile.close();
}



void PrintUsage(void) {
	std::cout << "Usage: Project2TerrainDataInput.exe TerrainDataInputXXX.txt" << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file