#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace myParser {
	class Vertex {
	public:
		//Universal setter
		void setVertexLoc(float, int);

		//Getter for x
		//float getX();

		//Getter for y
		//float getY();

		//Getter for z
		//float getZ();

	//private:
		//Vertex components
		float x = 0, y = 0, z = 0;
	};

	class Face {
	public:
		//Determine the number of vertices per face
		//void setNumVer(int);

		//Add data for all vertices of the face
		//void addVertex(Vertex);

		//Access to the vertices vector
		//std::vector<Vertex> getVertices();

		//Access to the number of vertices of the face
		//int getNumVer();

	//private:
		int nVer = 0;
		std::vector<Vertex> ver;
	};

	class Object {
	public:
		std::vector <Vertex> vertices;
		std::vector <Face> faces;
	};

	void readVertexCoords(std::string, Vertex&);

	void readFaceVertices(std::string, std::vector<int>&, int);

	void pairFaceVertices(std::vector<Vertex>, std::vector<int>, Face&);

	void executeParsing(std::string,
		std::vector<Object>& obj);

}