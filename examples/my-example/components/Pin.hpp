#pragma once

#include "PinType.hpp"
#include "NodeType.hpp"

#include "utilities/builders.h"
#include "utilities/widgets.h"

#include <imgui_node_editor.h>
#include <imgui_internal.h>

#include <string>

#include "json.hpp"

namespace MyGraph{

using namespace ax;
using json = nlohmann::json;

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

struct PinClass{
    unsigned int classId;
    std::string name;
    PinType pinType;
    PinKind pinKind;

    PinClass(unsigned int class_id, const char* name, PinType pin_type, PinKind pin_kind)
    : classId(class_id), name(name), pinType(pin_type), pinKind(pin_kind)
    {
        
    }

    PinClass() {}

    PinClass(const json& j){
        FromJson(j);
    }

    void FromJson(const json& j){
        classId = j["ClassId"];
        name = j["Name"];
        pinType = static_cast<PinType>(j["PinType"]);
        pinKind = static_cast<PinKind>(j["PinKind"]);
    }

};


struct PinInstance{

    ed::PinId   instanceId;
    unsigned int classId;
    unsigned int nodeInstanceId;

    PinInstance() {}

    PinInstance(unsigned int instance_id, unsigned int class_id, unsigned int node_instance_id):
        instanceId(instance_id), classId(class_id), nodeInstanceId(node_instance_id)
    {
        
    }

    PinInstance(const json& j){
        FromJson(j);
    }

    void FromJson(const json& j){
        instanceId = ed::PinId(j["InstanceId"]);
        classId = j["ClassId"];
        nodeInstanceId = j["NodeInstanceId"];
    }
};

}