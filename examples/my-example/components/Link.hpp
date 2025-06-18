#pragma once

#include "PinType.hpp"
#include "NodeType.hpp"

#include "Pin.hpp"
#include "Node.hpp"

namespace MyGraph{


using namespace ax;

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

struct Link
{
    ed::LinkId ID;

    ed::PinId StartPinID;
    ed::PinId EndPinID;

    ImColor Color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId):
        ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
    {
    }
};


}