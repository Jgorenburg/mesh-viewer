
#include "mesh.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "map"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace glm;
using namespace agl;

Mesh::Mesh() 
{
    norm = NULL;
    polygons = NULL;
    vertices = NULL;
}

Mesh::~Mesh()
{
    delete[] norm;
    delete[] polygons;
    delete[] vertices;
    
}

bool Mesh::loadPLY(const std::string& filename)
{
	ifstream image(filename);
    if (!image) {
        cerr << "cannot load image: " << filename << endl;
        return false;
    }
    else {
        string placeholder;
        getline(image, placeholder); // PLY line
        if (placeholder != "ply") {
            cerr << "not a PLY file" << filename << endl;
            return false;
        }

        getline(image, placeholder); // ascii line
        getline(image, placeholder); // comment line

        // reads the 3 words in the next line, ends up saving
        // the final one, which is the # of vertices
        for (int i = 0; i < 3; i++) {
            image >> placeholder;
            cout << placeholder << endl;
        }
        numVerts = stoi(placeholder);
        getline(image, placeholder);
        cout << "vertices: " << numVerts << endl;

        // reads the property lines and records the number of them
        bool readingProperties = true;
        int numProps = 0;
        while (readingProperties) {
            string firstWord;
            image >> firstWord;
            if (firstWord == "property") {
                getline(image, placeholder);
                numProps++;
            }
            else {
                readingProperties = false;
            }
        }

        // reads the 2 remaining words in the next line, ends up saving
        // the final one, which is the # of polygons
        for (int i = 0; i < 2; i++) {
            image >> placeholder;
        }
        numPolygons = stoi(placeholder);
        getline(image, placeholder);
        cout << "polygons: " << numPolygons << endl;

        getline(image, placeholder); // unneeded info
        getline(image, placeholder); // should be end of the header
        if (placeholder != "end_header") {
            cerr << "should be end of header, instead: " << placeholder << endl;
        }
     
        cout << "numer of properties: " << numProps << endl;

        delete[] polygons;
        delete[] vertices;
        delete[] norm;
        polygons = new unsigned int[3 * numPolygons];
        vertices = new float [3 * numVerts];
        norm = new float[3 * numVerts];

      
        // runs for each vertex
        for (int i = 0; i < numVerts; i++) {
            if (numProps == 3) {
                // when only positions
                float x, y, z;

                image >> x;
                image >> y;
                image >> z;

                int j = 3 * i;
                vertices[j] = x;
                vertices[j + 1] = y;
                vertices[j + 2] = z;

                if (i == 0 || x < minX) { minX = x; }
                if (i == 0 || x > maxX) { maxX = x; }
                if (i == 0 || y < minY) { minY = y; }
                if (i == 0 || y > maxY) { maxY = y; }
                if (i == 0 || z < minZ) { minZ = z; }
                if (i == 0 || z > maxZ) { maxZ = z; }
                
            }
            else if (numProps >= 6) {
                // if we include normals as well
                float x, y, z, nx, ny, nz;

                image >> x;
                image >> y;
                image >> z;
                image >> nx;
                image >> ny;
                image >> nz;

                int j = 3 * i;
                vertices[j] = x;
                vertices[j + 1] = y;
                vertices[j + 2] = z;
                norm[j] = nx;
                norm[j + 1] = ny;
                norm[j + 2] = nz;

                if (i == 0 || x < minX) { minX = x; }
                if (i == 0 || x > maxX) { maxX = x; }
                if (i == 0 || y < minY) { minY = y; }
                if (i == 0 || y > maxY) { maxY = y; }
                if (i == 0 || z < minZ) { minZ = z; }
                if (i == 0 || z > maxZ) { maxZ = z; }

                if (numProps > 6) {
                    getline(image, placeholder);
                }
               

            }
            else {
                cerr << "polygon line did not have the expected number of properties: " << numProps << endl; ;
                return false;
            }
        }

        // scaling to 2x2x2 cube around origin
        cout << maxX << " " << minX;

        float xwidth = (maxX - minX) / 2;
        float ywidth = (maxY - minY) / 2;
        float zwidth = (maxZ - minZ) / 2;
        float xoffset = -1 - (minX / xwidth);
        float yoffset = -1 - (minY / ywidth);
        float zoffset = -1 - (minZ / zwidth);

        for (int i = 0; i < numVerts * 3; i++) {
            if (i % 3 == 0) {
                cout << endl;
            }
            cout << vertices[i] << " ";

        }
        cout << "end first\n";

        
        for (int i = 0; i < numVerts; i++) {
            int j = 3 * i;
            vertices[j] = vertices[j] / xwidth + xoffset;
            vertices[j + 1] = vertices[j + 1] / ywidth + yoffset;
            vertices[j + 2] = vertices[j + 2] / zwidth + zoffset;
        }
        




        // runs for the polygons
        cout << "gothere" << endl;
        for (int i = 0; i < numPolygons; i++) {
            image >> placeholder;
            for (int j = 3 * i; j < 3 * i + 3; j++) {
                unsigned int vert;
                image >> vert;
                polygons[j] = vert;
            }
        }

        for (int i = 0; i < numVerts * 3; i++) {
            if (i % 3 == 0) {
                cout << endl;
            }
            cout << vertices[i] << " ";
            
        }
        /*for (int i = 0; i < numPolygons * 3; i++) {
            if (i % 3 == 0) {
                cout << endl;
            }
            cout << polygons[i] << " ";

        }*/

        return true;

    }

  
}

glm::vec3 Mesh::getMinBounds() const
{
  return vec3(minX, minY, minZ);
}

glm::vec3 Mesh::getMaxBounds() const
{
  return vec3(maxX, maxY, maxZ);
}

int Mesh::numVertices() const
{
   return numVerts;
}

int Mesh::numTriangles() const
{
   return numPolygons;
}

float* Mesh::positions() const
{
   return vertices;
}

float* Mesh::normals() const
{
   return norm;
}

unsigned int* Mesh::indices() const
{
  
   return polygons;
}

