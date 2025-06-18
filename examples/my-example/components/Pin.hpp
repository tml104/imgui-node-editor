#pragma once

#include "PinType.hpp"
#include "NodeType.hpp"

#include "utilities/builders.h"
#include "utilities/widgets.h"

#include <imgui_node_editor.h>
#include <imgui_internal.h>

#include <string>

namespace MyGraph{

using namespace ax;

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

struct Node;

struct Pin{

    ed::PinId   ID;
    // Node*     Node;
    ed::NodeId NodeID;
    std::string Name;
    PinType     Type;
    PinKind     Kind;

    Pin(int id, const char* name, PinType type):
        ID(id), NodeID(0), Name(name), Type(type), Kind(PinKind::Input)
    {
    }
};

}