#include <cmath>
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Torus.h"

using namespace std;

//procedurally generated torus
//For the Torus the algorithm positions a vertex to the right of the origin and then rotates
//that vertex in a circle on the XY plane using a rotation around the Z axis to form a ring.
//The ring is then moved outward by the inner radius distance. Texture coordinates and
//normal vectors are computed for each of these vertices as they are built.
//An additional vector tangent to the surface of the torus is also generated for each vertex.
//Vertices for additional torus rings are formed by rotating the original ring around the Y axis.
//Tangent and normal vectors for each resulting vertex are computed by also rotating the tangent
//and normal vectors of the original ring around the Y axis.

Torus::Torus(float innerRadius, float outerRadius, int precln) {
	prec = precln;
	inner = innerRadius;
	outer = outerRadius;
	init();

}

float Torus::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f / 360.0f); }

void Torus::init() {
	numVertices = (prec + 1) * (prec + 1);
	numIndices = prec * prec * 6;
	for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
	for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { sTangents.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { tTangents.push_back(glm::vec3()); }
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

	//calculate first ring
	for (int i = 0; i < prec + 1; i++) {
		float amt = toRadians(i * 360.0f / prec);

		glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 initPos(rMat * glm::vec4(0.0f, outer, 0.0f, 1.0f));
		vertices[i] = glm::vec3(initPos + glm::vec3(inner, 0.0f, 0.0f));
		texCoords[i] = glm::vec2(0.0f, ((float)i / (float)prec));

		rMat = glm::rotate(glm::mat4(1.0f), amt + (3.14159f / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		tTangents[i] = glm::vec3(rMat * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));
		sTangents[i] = glm::vec3(0.0f, 0.0f, -1.0f);
		normals[i] = glm::cross(tTangents[i], sTangents[i]);
	}



	//rotate the first ring around Y to get the other rings
	for (int ring = 1; ring < prec + 1; ring++) {
		for (int vert = 0; vert < prec + 1; vert++) {
			//rotate the vertex positions of the original ring around the Y axis
			float amt = (float(toRadians(ring * 360.0f / prec)));
			glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			vertices[ring * (prec + 1) + vert] = glm::vec3(rMat * glm::vec4(vertices[vert], 1.0f));

			//compute the texture coordinates for the vertices in the new rings

			texCoords[ring * (prec + 1) + vert] = glm::vec2((float)ring / (float)prec, texCoords[vert].t);

			//rotate the tangent and bitangent vectors around the Y axis
			rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			sTangents[ring * (prec + 1) + vert] = glm::vec3(rMat * glm::vec4(sTangents[vert], 1.0f));
			rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			tTangents[ring * (prec + 1) + vert] = glm::vec3(rMat * glm::vec4(tTangents[vert], 1.0f));

			//rotate the normal vector around the Y axis
			rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			normals[ring * (prec + 1) + vert] = glm::vec3(rMat * glm::vec4(normals[vert], 1.0f));
		}
	}

	//calculate triangle indices corresponding to the two triangles built per vertex
	for (int ring = 0; ring < prec; ring++) {
		for (int vert = 0; vert < prec; vert++) {
			indices[((ring * prec + vert) * 2) * 3 + 0] = ring * (prec + 1) + vert;
			indices[((ring * prec + vert) * 2) * 3 + 1] = (ring + 1) * (prec + 1) + vert;
			indices[((ring * prec + vert) * 2) * 3 + 2] = ring * (prec + 1) + vert + 1;
			indices[((ring * prec + vert) * 2 + 1) * 3 + 0] = ring * (prec + 1) + vert + 1;
			indices[((ring * prec + vert) * 2 + 1) * 3 + 1] = (ring + 1) * (prec + 1) + vert;
			indices[((ring * prec + vert) * 2 + 1) * 3 + 2] = (ring + 1) * (prec + 1) + vert + 1;
		}
	}
}

//accessors
int Torus::getNumVertices() { return numVertices; }
int Torus::getNumIndices() { return numIndices; }
std::vector<int> Torus::getIndices() { return indices; }
std::vector<glm::vec3> Torus::getVertices() { return vertices; }
std::vector<glm::vec2> Torus::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Torus::getNormals() { return normals; }
std::vector<glm::vec3> Torus::getsTangents() { return sTangents; }
std::vector<glm::vec3> Torus::gettTangents() { return tTangents; }
