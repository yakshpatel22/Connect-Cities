// Project2ConnectCities.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <float.h>
#include <vector>
#include <Windows.h>

void fileSeekLine(unsigned int line);
float getDistance(float origin[], float location[]);
void PrintUsage(void);
float buildRoad(unsigned int currentVertex, float distanceToDestination, float roadCost);
unsigned int findClosestCity(const unsigned int fromVertex); // Returns city vertex
std::vector<float> findLocation(unsigned int vertex);
std::string findCityName(unsigned int vertex);

// Global variables
std::ifstream terrainDataInputFile;
std::ofstream terrainRoadOutputFile;
std::vector<unsigned int> roads; // Edges
float totalCost = 0.0f;
unsigned int zerothEdge = 0;
unsigned int vertices = 0, cities = 0;
std::streampos startOfVertices = 0;
unsigned int destVertex = 0;
std::vector<float> destLoc = { 0.0f, 0.0f, 0.0f };

int main(int argc, char* argv[])
{
	unsigned int edges = 0, vertex = 0, closestVertex = 0, saved = 0, closestNeighbour = 0;
	float origin[3], closest[3], distance = 0.0f, closeDistance = FLT_MAX, farDistance = 0.0f, cost = 0.0f;
	bool isCity = false, isRoad = false;
	std::string city, closestCity, farthestCity;
	unsigned int startVertex = 0, endVertex = 0, farthestVertex = 0;
	float location[3] = { 0.0f, 0.0f, 0.0f }, farthest[3] = { 0.0f, 0.0f, 0.0f };

	if (argc != 2) {
		std::cout << "Error: Improper usage.." << std::endl;
		PrintUsage();
		return -1;
	}

	std::string fileName(argv[1]);

	terrainDataInputFile.open(fileName, std::ifstream::in);

	if (!terrainDataInputFile.is_open())
	{
		std::cout << "Error: Can't open '" + fileName + "' for input." << std::endl;
		return -1;
	}

	if (terrainDataInputFile.good()) terrainDataInputFile >> vertices;
	if (terrainDataInputFile.good()) terrainDataInputFile >> edges;
	if (terrainDataInputFile.good()) terrainDataInputFile >> cities;

	startOfVertices = terrainDataInputFile.tellg();  // Bookmark the start of vertices

	if (terrainDataInputFile.good()) terrainDataInputFile >> origin[0] >> origin[1] >> origin[2] >> isCity >> city;

	zerothEdge = 3 + vertices;

	closest[0] = farthest[0] = origin[0];
	closest[1] = farthest[1] = origin[1];
	closest[2] = farthest[2] = origin[2];

	saved = vertices;
	vertex = 0;
	while (terrainDataInputFile.good() && cities > 0 && vertices-- > 0) {
		++vertex;
		terrainDataInputFile >> location[0] >> location[1] >> location[2] >> isCity >> city;
		if (isCity) {
			distance = getDistance(origin, location);
			if (closeDistance > distance) {
				closest[0] = location[0];
				closest[1] = location[1];
				closest[2] = location[2];
				closeDistance = distance;
				closestCity = city;
				closestVertex = vertex;
			}
			if (farDistance < distance) {
				farthest[0] = location[0];
				farthest[1] = location[1];
				farthest[2] = location[2];
				farDistance = distance;
				farthestCity = city;
				farthestVertex = vertex;
			}
		}
	}
	vertices = saved;

	startVertex = findClosestCity(closestVertex);
	destVertex = findClosestCity(farthestVertex);
	destLoc = findLocation(destVertex);

	std::cout << "Building road between '" << findCityName(startVertex) << "' and '" << findCityName(destVertex) <<
	"' { START[COST] NEXT: } -> ";
	totalCost = buildRoad(startVertex, getDistance(findLocation(startVertex).data(), destLoc.data()), 0.0f);

	terrainDataInputFile.seekg(startOfVertices, terrainDataInputFile.beg); // 

	std::string outFile(fileName.replace(fileName.find(".txt"), std::string("_road.txt").length(), "_cities.txt"));

	terrainRoadOutputFile.open(outFile, std::ofstream::out);

	if (!terrainRoadOutputFile.is_open())
	{
		std::cout << "Error: Can't open '" + fileName + "' for input.";
		return -1;
	}

	vertices += 3;
	terrainDataInputFile.seekg(0, terrainDataInputFile.beg);
	char buffer[64];
	while (vertices-- && terrainDataInputFile.good() && terrainRoadOutputFile.good()) {
		terrainDataInputFile.getline(buffer, 64);
		terrainRoadOutputFile << buffer << std::endl;
	}

	std::cout << "Total roads " << roads.size() << std::endl;

	vertices = saved;
	unsigned int edge = 3 + vertices;
	for (unsigned int road : roads) {
		while (edge < road && terrainDataInputFile.good() && terrainRoadOutputFile.good()) {
			terrainDataInputFile.getline(buffer, 64);
			terrainRoadOutputFile << buffer << std::endl;
			++edge;
		}
		terrainDataInputFile >> startVertex >> endVertex >> cost >> isRoad;
		terrainRoadOutputFile << startVertex << " " << endVertex << " " << cost << " " << 1 << " " << std::endl;
		++edge;
	}
	while (terrainDataInputFile.good() && terrainRoadOutputFile.good()) {
		terrainDataInputFile.getline(buffer, 64);
		terrainRoadOutputFile << buffer << std::endl;
	}
	vertices = saved;

	terrainDataInputFile.close();
	terrainRoadOutputFile.close();
}

std::string findCityName(unsigned int vertex) {
	bool isCity = false;
	std::string city;
	float location[3] = { 0.0f, 0.0f, 0.0f };

	terrainDataInputFile.seekg(startOfVertices, terrainDataInputFile.beg); // Return back to the start of vertices bookmark
	do {
		terrainDataInputFile >> location[0] >> location[1] >> location[2] >> isCity >> city;
	} while (terrainDataInputFile.good() && cities > 0 && vertex--);

	return city;
}

unsigned int findClosestCity(const unsigned int fromVertex) { // Returns city vertex
	float nearest = FLT_MAX, distance = 0.0f;
	unsigned int saved = 0, vertex = 0, closestCity = 0;
	bool isCity = false;
	std::string city;
	float location[3] = { 0.0f, 0.0f, 0.0f };

	std::vector<float> origin = findLocation(fromVertex);

	saved = vertices;
	terrainDataInputFile.seekg(startOfVertices, terrainDataInputFile.beg); // Return back to the start of vertices bookmark
	while (terrainDataInputFile.good() && cities > 0 && vertices-- > 0) {
		terrainDataInputFile >> location[0] >> location[1] >> location[2] >> isCity >> city;
		if (vertex == fromVertex) { ++vertex; continue; }// Same city so skip

		if (isCity) {
			/*
			std::cout << "location -> " << isCity << " " << location[0] << "," << location[1] << "," << location[2] << std::endl;

			std::cout << fromVertex << " <> " << findLocation(fromVertex).data()[0] << ","
				<< findLocation(fromVertex).data()[1] << ","
				<< findLocation(fromVertex).data()[2]
				<< std::endl;
			*/
			distance = getDistance(origin.data(), location);

			//std::cout << distance << std::endl;

			if (distance < nearest) {
				nearest = distance;
				closestCity = vertex;
			}
		}
		++vertex;
	}
	vertices = saved;
	return closestCity;
}

std::vector<float> findLocation(unsigned int vertex) {
	bool isCity = false;
	std::string city;
	unsigned int saved = vertices;
	float location[3] = { 0.0f, 0.0f, 0.0f };

	terrainDataInputFile.seekg(startOfVertices, terrainDataInputFile.beg); // Return back to the start of vertices bookmark
	while (terrainDataInputFile.good() && cities > 0 && vertices-- > 0) {
		//		if (closestVertex == vertex || farthestVertex = vertex) continue; // Same city so skip
		terrainDataInputFile >> location[0] >> location[1] >> location[2] >> isCity >> city;
		if (vertex-- == 0) {
			vertices = saved;
			return { location[0], location[1], location[2] };
		}
	}

	vertices = saved;
	return { 0.0f, 0.0f, 0.0f };
}

void fileSeekLine(unsigned int line) {
	char buffer[64];
	terrainDataInputFile.seekg(0, terrainDataInputFile.beg);
	while (line-- && terrainDataInputFile.good()) terrainDataInputFile.getline(buffer, 64);
}

// “greedy algorithm”, where it takes the best, immediate option
float buildRoad(unsigned int currentVertex, float distanceToDestination, float roadCost) {
	unsigned int startVertex = 0, endVertex = 0, nextVertex = 0, cheapestVertex = 0;
	float cheapestCost = FLT_MAX, cost = 0.0f, nextDistance = 0.0f, distanceFromNext = 0.0f;
	bool isRoad = false;
	unsigned int currentEdge = 0, cheapestEdge = 0;
	float location[3] = { 0.0f, 0.0f, 0.0f };

	if (currentVertex == destVertex) {
		std::cout << std::endl;
		std::cout << "Road successfully built at a cost of " << roadCost << std::endl;
		return roadCost;
	}

	fileSeekLine(zerothEdge); currentEdge = zerothEdge;
	while (terrainDataInputFile.good()) { // Scan through the edges
		terrainDataInputFile >> startVertex >> endVertex >> cost >> isRoad;
		++currentEdge;

		if (startVertex != currentVertex && endVertex != currentVertex) continue; // the while loop

		nextVertex = (startVertex == currentVertex) ? endVertex : startVertex;

		// Calculate distance to destination and skip if the next route takes us away
		std::streampos inputFilePos = terrainDataInputFile.tellg();  // Bookmark the Edge to return back
		fileSeekLine(nextVertex + 3); // Go to the vertex to fetch the location
		if (terrainDataInputFile.good()) terrainDataInputFile >> location[0] >> location[1] >> location[2];
		terrainDataInputFile.seekg(inputFilePos, terrainDataInputFile.beg); // Return back to the bookmark
		distanceFromNext = getDistance(location, destLoc.data());
		// End of distance calculation

		if (distanceFromNext < distanceToDestination && cost >= 0 && cost < cheapestCost) {
			nextDistance = distanceFromNext;
			cheapestCost = cost;
			cheapestVertex = nextVertex;
			cheapestEdge = currentEdge - 1;
		}
	}

	if (cheapestCost == FLT_MAX) std::cout << "PROBLEM: All nodes from here goes far away from destination!!" << std::endl;
	std::cout << "{" << currentVertex << "  [" << cheapestCost << "] " << cheapestVertex << "} : ";
	roads.push_back(cheapestEdge);
	terrainDataInputFile.clear();
	return buildRoad(cheapestVertex, nextDistance, roadCost + cheapestCost);
}

float getDistance(float origin[], float destn[])
{
	double deltaX = (double)origin[0] - (double)destn[0];
	double deltaY = (double)origin[1] - (double)destn[1];
	double deltaZ = (double)origin[2] - (double)destn[2];
	return (float)sqrt((deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
}

void PrintUsage(void) {
	std::cout << "Usage: Project2ConnectCities.exe TerrainDataInputXXX.txt" << std::endl;
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