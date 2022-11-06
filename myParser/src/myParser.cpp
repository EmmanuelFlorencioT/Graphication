#include "myParser.h"

#include <fstream>
#include <string>
#include <vector>

namespace myParser {
    
    void Vertex::setVertexLoc(float data, int axis) {
        switch (axis) {
        case 0:
            x = data;
            break;
        case 1:
            y = data;
            break;
        case 2:
            z = data;
            break;
        default:
            break;
        }
    }

    void readVertexCoords(std::string line, Vertex& v) {
        int begin = 2, j;
        std::string buff;

        for (int i = 0; i < 3; i++) {
            for (j = begin; (line[j] != ' ' && j < line.size()); j++) {
                buff += line[j];
            }
            v.setVertexLoc(stof(buff), i);
            buff.clear();
            begin = j + 1;
        }
    }

    void readFaceVertices(std::string line, std::vector<int>& index, int offset) {
        int begin = 2, j;
        std::string buff;

        while (begin < line.size()) {
            for (j = begin; (line[j] != ' ' && j < line.size()); j++) {
                buff += line[j];
            }
            index.push_back(stoi(buff) - 1 - offset);
            buff.clear();
            begin = j + 1;
        }
    }

    void pairFaceVertices(std::vector<Vertex> ver, std::vector<int> index, Face& f) {
        f.nVer = index.size();
        for (int i = 0; i < index.size(); i++) {
            f.ver.push_back(ver[index[i]]);
        }
    }

    void executeParsing(std::string nameFile, std::vector<Object>& obj) {

        Object objAux;
        bool notFirst = false;

        int offset = 0; //Offset in vertices indices by multiple objects in the obj
        int cont = 0;

        std::fstream myFile;
        myFile.open(nameFile, std::ios::in);

        if (myFile.is_open()) {
            std::string s;
            while (getline(myFile, s)) {
                if (s[0] == 'v' && s[1] == ' ') {
                    Vertex tempV;
                    readVertexCoords(s, tempV);
                    objAux.vertices.push_back(tempV);
                    cont++;
                }
                if (s[0] == 'f' && s[1] == ' ') {
                    std::vector <int> indices;
                    Face tempF;
                    readFaceVertices(s, indices, offset);
                    pairFaceVertices(objAux.vertices, indices, tempF);
                    objAux.faces.push_back(tempF);
                }
                if (s[0] == 'o' && s[1] == ' ') {
                    if (notFirst) {
                        obj.push_back(objAux);

                        objAux.faces.clear();
                        objAux.vertices.clear();
                    }
                    else
                        notFirst = true;

                    offset = cont;
                }
            }
            obj.push_back(objAux);

            myFile.close();
        }
    }

}