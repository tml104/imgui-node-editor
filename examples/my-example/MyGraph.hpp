#pragma once

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

struct MyGraph:
    public Application
{
    using Application::Application;

    MyGraph(const char* name, int argc, char** argv): Application(name, argc, argv), myGraphName(name)
    {
    }

    void SaveGraphData();

    void LoadGraphData();

    void LoadTextures();

    void ReleaseTextures();

    bool IsPinLinked(ed::PinId pin_instance_id);

    void DrawPinIcon(const PinClass& pin_class, bool is_connected, int alpha);

    void DrawNode(util::BlueprintNodeBuilder& builder, const NodeClass& node_class, const NodeInstace& node_instance);
    
    void OnStart() override ;

    void OnStop() override;

    void OnFrame(float deltaTime) override ;

    ed::EditorContext* editorContext = nullptr;
    std::string myGraphName;

    std::map<unsigned int, NodeClass> nodeClassMap;
    std::map<unsigned int, PinClass> pinClassMap;

    std::map<unsigned int, NodeInstace> nodeInstaceMap;
    std::map<unsigned int, PinInstance> pinInstaceMap;

    std::map<std::pair<unsigned int, unsigned int>, Link> linkMap;

    // 贴图
    ImTextureID textureHeaderBackground;
    ImTextureID textureSaveIcon;
    ImTextureID textureRestoreIcon;

};


}