#pragma once
#include <D3DX10math.h>
#include "Mesh.h"
#include <fstream>

namespace MeshGeneration
{

	static void GenerateTangents(Mesh::VertexData* vertices, int numvertices, bool printResult = false)
	{

		int numTriangles = numvertices / 3;
		int loopIndex = 0;

		for (int i = 0; i < numTriangles; i++)
		{

			D3DXVECTOR3 vector1, vector2, tangent, binormal;
			D3DXVECTOR2 texUVector, texVVector;
			float den, length;

			vector1 = vertices[loopIndex + 1].position - vertices[loopIndex].position;			
			vector2 = vertices[loopIndex + 2].position - vertices[loopIndex].position;
			
			texUVector.x = vertices[loopIndex + 1].texture.x - vertices[loopIndex].texture.x;
			texUVector.y = vertices[loopIndex + 2].texture.x - vertices[loopIndex].texture.x;

			texVVector.x = vertices[loopIndex + 1].texture.y - vertices[loopIndex].texture.y;
			texVVector.y = vertices[loopIndex + 2].texture.y - vertices[loopIndex].texture.y;

			den = 1.0f / (texUVector.x * texVVector.y - texUVector.y * texVVector.x);

			tangent.x = (texVVector.y * vector1.x - texVVector.x * vector2.x) * den;
			tangent.y = (texVVector.y * vector1.y - texVVector.x * vector2.y) * den;
			tangent.z = (texVVector.y * vector1.z - texVVector.x * vector2.z) * den;

			length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
			tangent = tangent / length;
					
			// get the binormal from the tangent and normal
			D3DXVec3Cross(&binormal, &tangent, &vertices[loopIndex].normal);
		
			vertices[loopIndex].tangent = tangent;			
			vertices[loopIndex + 1].tangent = tangent;			
			vertices[loopIndex + 2].tangent = tangent;			

			vertices[loopIndex].binormal = binormal;			
			vertices[loopIndex + 1].binormal = binormal;			
			vertices[loopIndex + 2].binormal = binormal;
			

			if (printResult)
			{
				printf("Face : %i\n TANGENT \n%f %f %f \n", i + 1, vertices[loopIndex].tangent.x, vertices[loopIndex].tangent.y, vertices[loopIndex].tangent.z);
				printf("%f %f %f \n", vertices[loopIndex + 1].tangent.x, vertices[loopIndex + 1].tangent.y, vertices[loopIndex + 1].tangent.z);
				printf("%f %f %f \n", vertices[loopIndex + 2].tangent.x, vertices[loopIndex + 2].tangent.y, vertices[loopIndex + 2].tangent.z);

				printf("BINORMAL\n%f %f %f \n", vertices[loopIndex].binormal.x, vertices[loopIndex].binormal.y, vertices[loopIndex].binormal.z);
				printf("%f %f %f \n", vertices[loopIndex + 1].binormal.x, vertices[loopIndex + 1].binormal.y, vertices[loopIndex + 1].binormal.z);
				printf("%f %f %f \n\n", vertices[loopIndex + 2].binormal.x, vertices[loopIndex + 2].binormal.y, vertices[loopIndex + 2].binormal.z);
			}

			loopIndex += 3;
		}

	}

	static void PrintVerticesTofile(Mesh::VertexData* vertices, unsigned long numVertices)
	{
		
		unsigned long numTriangles = numVertices / 3;
		int loopIndex = 0;

		std::ofstream file("verts");

		for (int i = 0; i < numTriangles; i++)
		{

			file << vertices[loopIndex + 0].position.x << " " << vertices[loopIndex + 0].position.y  << " " << vertices[loopIndex + 0].position.z << "				"
				 << vertices[loopIndex + 0].texture.x  << " " << vertices[loopIndex + 0].texture.y   << "				 "
				 << vertices[loopIndex + 0].normal.x   << " " << vertices[loopIndex + 0].normal.y    << " " << vertices[loopIndex + 0].normal.z   << "				"
			     << vertices[loopIndex + 0].tangent.x  << " " << vertices[loopIndex + 0].tangent.y   << " " << vertices[loopIndex + 0].tangent.z  << "				"
				 << vertices[loopIndex + 0].binormal.x << " " << vertices[loopIndex + 0].binormal.y  << " " << vertices[loopIndex + 0].binormal.z << "\n"

				 << vertices[loopIndex + 1].position.x << " "  << vertices[loopIndex + 1].position.y << " " << vertices[loopIndex + 1].position.z << "				"
				 << vertices[loopIndex + 1].texture.x  << " "  << vertices[loopIndex + 1].texture.y  << "				 "
				 << vertices[loopIndex + 1].normal.x   << " "  << vertices[loopIndex + 1].normal.y   << " " << vertices[loopIndex + 1].normal.z   << "				"
				 << vertices[loopIndex + 1].tangent.x  << " "  << vertices[loopIndex + 1].tangent.y  << " " << vertices[loopIndex + 1].tangent.z  << "				"
				 << vertices[loopIndex + 1].binormal.x << " "  << vertices[loopIndex + 1].binormal.y << " " << vertices[loopIndex + 1].binormal.z << "\n"

				 << vertices[loopIndex + 2].position.x << " "  << vertices[loopIndex + 2].position.y << " " << vertices[loopIndex + 2].position.z << "			    "
				 << vertices[loopIndex + 2].texture.x  << " "  << vertices[loopIndex + 2].texture.y  << "				 "
				 << vertices[loopIndex + 2].normal.x   << " "  << vertices[loopIndex + 2].normal.y   << " " << vertices[loopIndex + 2].normal.z   << "				"
				 << vertices[loopIndex + 2].tangent.x  << " "  << vertices[loopIndex + 2].tangent.y  << " " << vertices[loopIndex + 2].tangent.z  << "			    "
				 << vertices[loopIndex + 2].binormal.x << " "  << vertices[loopIndex + 2].binormal.y << " " << vertices[loopIndex + 2].binormal.z << "\n\n";
			

			loopIndex += 3;
		}
	
	}

}