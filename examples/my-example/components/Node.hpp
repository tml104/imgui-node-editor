#pragma once

#include "PinType.hpp"
#include "NodeType.hpp"

#include "Pin.hpp"

#include <vector>

namespace MyGraph{

struct Node{
    ed::NodeId ID;
    std::string Name;
    std::vector<ed::PinId> Inputs;
    std::vector<ed::PinId> Outputs;

    ImColor Color;
    NodeType Type;
    ImVec2 Size;

    std::string State;
    std::string SavedState;

    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)):
    ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
    {
    }

    bool operator()(const Node& a, const Node& b){
        return a.ID.AsPointer() < b.ID.AsPointer();
    }
};

}