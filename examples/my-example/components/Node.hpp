#pragma once

#include "PinType.hpp"
#include "NodeType.hpp"

#include "utilities/builders.h"
#include "utilities/widgets.h"

#include <imgui_node_editor.h>
#include <imgui_internal.h>

#include "Pin.hpp"

#include <vector>

#include "json.hpp"
namespace MyGraph{


using namespace ax;
using json = nlohmann::json;

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using ull = unsigned long long;

struct NodeClass{
    ull classId;
    NodeType nodeType;
    std::string name;
    ImColor color;

    ImVec2 Size; // 只对Comment类节点有用，ed::Group 会用到这个参数

    std::vector<ull> inputPinsClassId;
    std::vector<ull> outputPinsClassId;

    NodeClass() {}

    NodeClass(const json& j){
        FromJson(j);
    }
    
    void FromJson(const json& j){

        classId = j["ClassId"];
        nodeType = static_cast<NodeType>(j["NodeType"]);
        name = j["Name"];

        color = ImColor(
            static_cast<int>(j["Color"][0]), 
            static_cast<int>(j["Color"][1]), 
            static_cast<int>(j["Color"][2])
        );

        if(j.contains("Size")){
            Size = ImVec2(static_cast<int>(j["Size"][0]), static_cast<int>(j["Size"][1]));
        }

        inputPinsClassId.clear();
        outputPinsClassId.clear();

        for(unsigned int element: j["InputPinsClassId"]){
            inputPinsClassId.emplace_back(element);
        }

        for(unsigned int element: j["OutputPinsClassId"]){
            outputPinsClassId.emplace_back(element);
        }
    }
};

struct NodeInstace{
    ed::NodeId instanceId;
    unsigned int classId;
    ImVec2 location;

    std::vector<ed::PinId> inputPinsInstaceId;
    std::vector<ed::PinId> outputPinsInstaceId;

    NodeInstace() {}

    NodeInstace(const json& j) {
        FromJson(j);
    }

    void FromJson(const json& j){
        instanceId = ed::NodeId(j["InstanceId"]);
        classId = j["ClassId"];

        if(j.contains("Location")){
            location = ImVec2(static_cast<int>(j["Location"][0]), static_cast<int>(j["Location"][1]));
            ed::SetNodePosition(instanceId, location);
        }

        for(unsigned int element: j["InputPinsInstanceId"]){
            inputPinsInstaceId.emplace_back(element);
        }

        for(unsigned int element: j["OutputPinsInstanceId"]){
            outputPinsInstaceId.emplace_back(element);
        }

    }

    json ToJson(){
        json j;

        j["InstanceId"] = instanceId.Get();
        j["ClassId"] = classId;

        j["Location"] = std::vector<int>{
            static_cast<int>(location.x),
            static_cast<int>(location.y)
        };

        std::vector<ull> pins_vec;
        std::transform(
            inputPinsInstaceId.begin(), 
            inputPinsInstaceId.end(), 
            std::back_inserter(pins_vec), 
            [](const ed::PinId& pin_id){
                return pin_id.Get();
            }
        );

        j["InputPinsInstanceId"] = pins_vec;

        pins_vec.clear();
        std::transform(
            outputPinsInstaceId.begin(), 
            outputPinsInstaceId.end(), 
            std::back_inserter(pins_vec), 
            [](const ed::PinId& pin_id){
                return pin_id.Get();
            }
        );

        j["OutputPinsInstanceId"] = pins_vec;

        return j;
    }

};

// struct Node{
//     ed::NodeId ID;
//     std::string Name;
//     std::vector<ed::PinId> Inputs;
//     std::vector<ed::PinId> Outputs;

//     ImColor Color;
//     NodeType Type;
//     ImVec2 Size;

//     std::string State;
//     std::string SavedState;

//     Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)):
//     ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
//     {
//     }

//     bool operator()(const Node& a, const Node& b){
//         return a.ID.AsPointer() < b.ID.AsPointer();
//     }
// };

}