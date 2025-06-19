#pragma once

#include "PinType.hpp"
#include "NodeType.hpp"

#include "Pin.hpp"
#include "Node.hpp"

#include "json.hpp"

namespace MyGraph{

using json = nlohmann::json;

using namespace ax;

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

struct Link
{
    ed::LinkId linkId;

    ed::PinId startPinInstanceId;
    ed::PinId endPinInstanceId;

    ImColor color;

    Link() {}

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId):
        linkId(id), startPinInstanceId(startPinId), endPinInstanceId(endPinId), color(255, 255, 255)
    {
    }

    Link(const json& j){
        FromJson(j);
    }

    void FromJson(const json& j){
        linkId = ed::LinkId(j["LinkId"]);
        startPinInstanceId = ed::PinId(j["StartPinInstanceId"]);
        endPinInstanceId = ed::PinId(j["EndPinInstanceId"]);

        color = ImColor(
            static_cast<int>(j["Color"][0]), 
            static_cast<int>(j["Color"][1]), 
            static_cast<int>(j["Color"][2])
        );
    }

    json ToJson(){
        json j;

        j["LinkId"] = linkId.Get();
        j["StartPinInstanceId"] = startPinInstanceId.Get();
        j["EndPinInstanceId"] = endPinInstanceId.Get();

        j["Color"] = std::vector<int>{
            static_cast<int>(color.Value.x * 255.0), 
            static_cast<int>(color.Value.y * 255.0), 
            static_cast<int>(color.Value.z * 255.0)
        };

        return j;
    }
};

}