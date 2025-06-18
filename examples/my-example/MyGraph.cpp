#include "MyGraph.hpp"
#include <fstream>

static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

void MyGraph::MyGraph::LoadGraphData()
{
    std::ifstream classes_f(myGraphName + "Classes.json");

    if(!classes_f.is_open()){
        return ;
    }

    json classes_j;
    classes_f >> classes_j;

    // 解析classes
    for(auto&& node_class: classes_j["NodeClasses"]){
        NodeClass new_node_class(node_class);
        nodeClassMap[new_node_class.classId] = new_node_class;
    }

    for(auto&& pin_class: classes_j["PinClasses"]){
        PinClass new_pin_class(pin_class);
        pinClassMap[new_pin_class.classId] = new_pin_class;
    }

    std::ifstream instance_f(myGraphName + "Instances.json");

    if(!instance_f.is_open()){
        return ;
    }

    json instances_j;
    instance_f >> instances_j;

    for(auto&& node_instance: instances_j["NodeInstaces"]){
        NodeInstace new_node_instance(node_instance);

        nodeInstaceMap[new_node_instance.instanceId.Get()] = new_node_instance;
    }

    for(auto&& pin_instance: instances_j["PinInstaces"]){
        PinInstance new_pin_instace(pin_instance);

        pinInstaceMap[new_pin_instace.instanceId.Get()] = new_pin_instace;
    }

    for(auto&& link_instace: instances_j["Links"]){
        Link new_link(link_instace);
        linkMap[std::make_pair(new_link.startPinInstanceId.Get(), new_link.endPinInstanceId.Get())] = new_link;
    }

}

void MyGraph::MyGraph::LoadTextures()
{
    textureHeaderBackground = LoadTexture("data/BlueprintBackground.png");
    textureSaveIcon = LoadTexture("data/ic_save_white_24dp.png");
    textureRestoreIcon = LoadTexture("data/ic_restore_white_24dp.png");
}

void MyGraph::MyGraph::ReleaseTextures()
{
    auto releaseTexture = [this](ImTextureID& id)
    {
        if (id)
        {
            DestroyTexture(id);
            id = nullptr;
        }
    };

    releaseTexture(textureHeaderBackground);
    releaseTexture(textureSaveIcon);
    releaseTexture(textureRestoreIcon);
}

void MyGraph::MyGraph::SaveGraphData()
{
    // TODO: 导出到json中，写法与LoadGraphData中的类似
    // 由于ToJson没写，所以暂时放一放

}

void MyGraph::MyGraph::OnStart(){
    ed::Config config;
    config.SettingsFile = (myGraphName+".json").c_str();
    editorContext = ed::CreateEditor(&config);

    LoadGraphData();

    // 加载贴图
    LoadTextures();
}

void MyGraph::MyGraph::OnStop(){

    if (editorContext)
    {
        ed::DestroyEditor(editorContext);
        editorContext = nullptr;
    }

    SaveGraphData();

    // 释放贴图
    ReleaseTextures();
}


void MyGraph::MyGraph::OnFrame(float deltaTime){
    auto& io = ImGui::GetIO();

    // 帧数显示
    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
    ImGui::Separator();

    ed::SetCurrentEditor(editorContext);

    ed::Begin("Node editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();

        util::BlueprintNodeBuilder builder(
            textureHeaderBackground, 
            GetTextureWidth(textureHeaderBackground), 
            GetTextureHeight(textureHeaderBackground));

        

    }
    ed::End();


    ed::SetCurrentEditor(nullptr);
}