// ObjParser.h
#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <glm/glm.hpp>
#include "../Geometry.h"
#include "ResourceLoader.h"
#include <algorithm>

struct ObjIndex {
    int vIdx, vtIdx, vnIdx;
    bool operator<(const ObjIndex& other) const {
        if (vIdx != other.vIdx) return vIdx < other.vIdx;
        if (vtIdx != other.vtIdx) return vtIdx < other.vtIdx;
        return vnIdx < other.vnIdx;
    }
};

class ObjParser {
public:
    static Geometry* Parse(const std::string& filePath) {
        std::string content = ResourceLoader::LoadTextFile(filePath);
        std::stringstream ss(content);
        std::string line;

        std::vector<glm::vec3> tempPos;
        std::vector<glm::vec3> tempNorm;
        std::vector<glm::vec2> tempUV;

        std::vector<float> finalVertices; // 存入 Geometry 的数据
        std::vector<unsigned int> finalIndices;

        std::map<ObjIndex, unsigned int> indexMap; // 映射 OBJ 组合索引 -> OpenGL 索引

        while (std::getline(ss, line)) {
            if (line.substr(0, 2) == "v ") {
                std::istringstream s(line.substr(2));
                glm::vec3 v; s >> v.x >> v.y >> v.z;
                tempPos.push_back(v);
            } else if (line.substr(0, 3) == "vn ") {
                std::istringstream s(line.substr(3));
                glm::vec3 vn; s >> vn.x >> vn.y >> vn.z;
                tempNorm.push_back(vn);
            } else if (line.substr(0, 3) == "vt ") {
                std::istringstream s(line.substr(3));
                glm::vec2 vt; s >> vt.x >> vt.y;
                tempUV.push_back(vt);
            } else if (line.substr(0, 2) == "f ") {
                std::string vertexStr;
                std::istringstream s(line.substr(2));
                while (s >> vertexStr) {
                    // 解析 "1/1/1" 或 "1//1" 格式
                    ObjIndex currentIdx = {0, 0, 0};
                    std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
                    std::istringstream vStream(vertexStr);
                    
                    vStream >> currentIdx.vIdx;
                    if (vertexStr.find("  ") != std::string::npos) { 
                        // 处理 "v//vn" 格式
                         vStream >> currentIdx.vnIdx;
                    } else {
                        // 处理 "v/vt" 或 "v/vt/vn"
                        vStream >> currentIdx.vtIdx;
                        vStream >> currentIdx.vnIdx;
                    }

                    // OBJ 索引从 1 开始，转为 0
                    currentIdx.vIdx--; currentIdx.vtIdx--; currentIdx.vnIdx--;

                    // 查找或创建新顶点
                    if (indexMap.find(currentIdx) == indexMap.end()) {
                        unsigned int newIndex = (unsigned int)indexMap.size();
                        indexMap[currentIdx] = newIndex;
                        finalIndices.push_back(newIndex);

                        // 填充 Pos
                        glm::vec3 p = tempPos[currentIdx.vIdx];
                        finalVertices.push_back(p.x); finalVertices.push_back(p.y); finalVertices.push_back(p.z);

                        // 填充 Normal (如果没有法线数据，填默认值)
                        if (currentIdx.vnIdx >= 0 && currentIdx.vnIdx < tempNorm.size()) {
                            glm::vec3 n = tempNorm[currentIdx.vnIdx];
                            finalVertices.push_back(n.x); finalVertices.push_back(n.y); finalVertices.push_back(n.z);
                        } else {
                            finalVertices.push_back(0); finalVertices.push_back(1); finalVertices.push_back(0);
                        }

                        // 填充 UV
                        if (currentIdx.vtIdx >= 0 && currentIdx.vtIdx < tempUV.size()) {
                            glm::vec2 t = tempUV[currentIdx.vtIdx];
                            finalVertices.push_back(t.x); finalVertices.push_back(t.y);
                        } else {
                            finalVertices.push_back(0); finalVertices.push_back(0);
                        }

                    } else {
                        finalIndices.push_back(indexMap[currentIdx]);
                    }
                }
            }
        }
        finalVertices = Geometry::ComputeTangents(finalVertices, finalIndices);
        return new Geometry(finalVertices, finalIndices);
    }
};