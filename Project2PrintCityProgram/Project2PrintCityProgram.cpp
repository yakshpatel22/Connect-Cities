// Project2PrintCityProgram.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

void PrintUsage(void);

int main(int argc, char* argv[])
{
	std::ifstream terrainDataInputFile;
	unsigned int vertices = 0, edges = 0, cities = 0;
	float location[3];
	bool isCity = false;
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

	//	std::getline(terrainDataInputFile, line);

	while (terrainDataInputFile.good() && cities > 0 && vertices-- > 0) {
		terrainDataInputFile >> location[0] >> location[1] >> location[2] >> isCity >> city;
		if (isCity) {
			std::cout << city << std::endl;
		}
	}

	terrainDataInputFile.close();
}

void PrintUsage(void) {
	std::cout << "Usage: Project2PrintCityProgram.exe TerrainDataInputXXX.txt" << std::endl;
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
