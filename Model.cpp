#include"Model.h"

Model::Model(const char* file)
{
	std::string text = get_file_contents(file);
	JSON = json::parse(text);

	Model::file = file;
	data = getData();

	traverseNode(0);
}

void Model::Draw(Shader& shader, Camera& camera)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Mesh::Draw(shader, camera, matricesMeshes[i]);
	}
}

void Model::loadMesh(unsigned int meshIndex)
{
	unsigned int positionAccesorIndex = JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normalAccessorIndex  = JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int textureAccessorIndex = JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indicesAccessorIndex = JSON["meshes"][meshIndex]["primitives"][0]["indices"];

	std::vector<float> positionVector = getFloats(JSON["accessors"][positionAccesorIndex]);
	std::vector<glm::vec3> positions = groupFloatsVec3(positionVector);
	std::vector<float> normalVectors = getFloats(JSON["accessors"][normalAccessorIndex]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normalVectors);
	std::vector<float> textureVectors = getFloats(JSON["accessors"][textureAccessorIndex]);
	std::vector<glm::vec2> textureUVs = groupFloatsVec2(textureVectors);

	std::vector<Vertex> vertices = assembleVertices(positions, normals, textureUVs);
	std::vector<GLuint> indices = getIndices(JSON["accessors"][indicesAccessorIndex]);
	std::vector<Texture> textures = getTextures();

	meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
	json node = JSON["nodes"][nextNode];
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	
	if (node.find("translation") != node.end())
	{
		float translationValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			translationValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(translationValues);
	}
	
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	
	if (node.find("rotation") != node.end())
	{
		float rotationValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotationValues);
	}
	
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}
	
	glm::mat4 matrixNode = glm::mat4(1.0f);
	
	if (node.find("matrix") != node.end())
	{
		float matrixValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matrixValues[i] = (node["matrix"][i]);
		matrixNode = glm::make_mat4(matrixValues);
	}

	glm::mat4 translationMatrix = glm::mat4(1.0f);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);

	translationMatrix = glm::translate(translationMatrix, translation);
	rotationMatrix = glm::mat4_cast(rotation);
	scaleMatrix = glm::scale(scaleMatrix, scale);

	glm::mat4 matrixNextNode = matrix * matrixNode * translationMatrix * rotationMatrix * scaleMatrix;

	if (node.find("mesh") != node.end())
	{
		translationsMeshes.push_back(translation);
		rotationsMeshes.push_back(rotation);
		scalesMeshes.push_back(scale);
		matricesMeshes.push_back(matrixNextNode);

		loadMesh(node["mesh"]);
	}

	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matrixNextNode);
	}
}

std::vector<unsigned char> Model::getData()
{
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	std::string fileString = std::string(file);
	std::string fileDirectory = fileString.substr(0, fileString.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDirectory + uri).c_str());

	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor)
{
	std::vector<float> floatVector;

	unsigned int bufferViewIndex = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	json bufferView = JSON["bufferViews"][bufferViewIndex];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);

	unsigned int vertexNums;
	
	if (type == "SCALAR") vertexNums = 1;
	else if (type == "VEC2") vertexNums = 2;
	else if (type == "VEC3") vertexNums = 3;
	else if (type == "VEC4") vertexNums = 4;
	else throw std::invalid_argument("Type is invalid (Must be one of: SCALAR, VEC2, VEC3 or VEC4)");

	unsigned int beginningOfData = byteOffset + accessorByteOffset;
	unsigned int lengthOfData = count * 4 * vertexNums;
	
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4)
	{
		unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVector.push_back(value);
	}

	return floatVector;
}

std::vector<GLuint> Model::getIndices(json accessor)
{
	std::vector<GLuint> indices;

	unsigned int bufferViewIndex = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	json bufferView = JSON["bufferViews"][bufferViewIndex];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);

	unsigned int beginningOfData = byteOffset + accessorByteOffset;
	if (componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 4; i += 4)
		{
			unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2; i += 2)
		{
			unsigned char bytes[] = { data[i], data[i + 1] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2; i += 2)
		{
			unsigned char bytes[] = { data[i], data[i + 1] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}

	return indices;
}

std::vector<Texture> Model::getTextures()
{
	std::vector<Texture> textures;

	std::string fileString = std::string(file);
	std::string fileDirectory = fileString.substr(0, fileString.find_last_of('/') + 1);

	for (unsigned int i = 0; i < JSON["images"].size(); i++)
	{
		std::string texturePath = JSON["images"][i]["uri"];
		bool skip = false;

		for (unsigned int j = 0; j < loadedTextureName.size(); j++)
		{
			if (loadedTextureName[j] == texturePath)
			{
				textures.push_back(loadedTexture[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			if (texturePath.find("base") != std::string::npos)
			{
				Texture diffuse = Texture((fileDirectory + texturePath).c_str(), "diffuse", loadedTexture.size());
				textures.push_back(diffuse);
				loadedTexture.push_back(diffuse);
				loadedTextureName.push_back(texturePath);
			}
			else if (texturePath.find("metallic") != std::string::npos)
			{
				Texture specular = Texture((fileDirectory + texturePath).c_str(), "specular", loadedTexture.size());
				textures.push_back(specular);
				loadedTexture.push_back(specular);
				loadedTextureName.push_back(texturePath);
			}
		}
	}

	return textures;
}

std::vector<Vertex> Model::assembleVertices(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> textureUVs
)
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back
		(
			Vertex
			{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				textureUVs[i]
			}
		);
	}
	return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVector)
{
	const unsigned int floatsPerVector = 2;

	std::vector<glm::vec2> vectors;
	for (unsigned int i = 0; i < floatVector.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec2(0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVector[i + j];
		}
	}
	return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVector)
{
	const unsigned int floatsPerVector = 3;

	std::vector<glm::vec3> vectors;
	for (unsigned int i = 0; i < floatVector.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec3(0, 0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVector[i + j];
		}
	}
	return vectors;
}

std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVector)
{
	const unsigned int floatsPerVector = 4;

	std::vector<glm::vec4> vectors;
	for (unsigned int i = 0; i < floatVector.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec4(0, 0, 0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVector[i + j];
		}
	}
	return vectors;
}