#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <application.h>
#include "utilities/builders.h"
#include "utilities/widgets.h"

#include <imgui_node_editor.h>
#include <imgui_internal.h>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <utility>

#include "components/NodeType.hpp"
#include "components/PinType.hpp"

#include "components/Pin.hpp"
#include "components/Node.hpp"
#include "components/Link.hpp"


namespace MyGraph{

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

using ull = unsigned long long;

struct MyGraph:
    public Application
{
    using Application::Application;

    MyGraph(const char* name, int argc, char** argv): 
        Application(name, argc, argv), 
        myGraphName(name),
        myGraphClassesJson(myGraphName + "Classes.json"),
        myGraphInstancesJson(myGraphName + "Instances.json")
    {
    }

    void SaveGraphData();

    void LoadGraphData();

    void LoadTextures();

    void ReleaseTextures();

    void ShowLabel(const char* label, ImColor color);

    bool IsPinLinked(ed::PinId pin_instance_id);

    void DrawPinIcon(const PinClass& pin_class, bool is_connected, int alpha);

    void DrawNode(util::BlueprintNodeBuilder& builder, const NodeClass& node_class, const NodeInstace& node_instance);
    
    void OnStart() override ;

    void OnStop() override;

    void OnFrame(float deltaTime) override ;

    ed::EditorContext* editorContext = nullptr;

    // 名字与文件名
    std::string myGraphName;
    std::string myGraphClassesJson;
    std::string myGraphInstancesJson;


    std::map<ull, NodeClass> nodeClassMap;
    std::map<ull, PinClass> pinClassMap;

    std::map<ull, NodeInstace> nodeInstaceMap;
    std::map<ull, PinInstance> pinInstaceMap;

    std::map<std::pair<ull, ull>, Link> linkMap;
    
    // id 维护
    ull maxId;

    // 贴图
    ImTextureID textureHeaderBackground;
    ImTextureID textureSaveIcon;
    ImTextureID textureRestoreIcon;

};


}