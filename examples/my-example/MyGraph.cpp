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

bool MyGraph::MyGraph::IsPinLinked(ed::PinId pin_instance_id)
{
    for(auto& p: linkMap){
        if(p.first.first == pin_instance_id.Get() || p.first.second == pin_instance_id.Get()){
            return true;
        }
    }
    return false;
}

void MyGraph::MyGraph::DrawPinIcon(const PinClass &pin_class, bool is_connected, int alpha)
{
    IconType iconType;
    ImColor  color = GetIconColor(pin_class.pinType);
    color.Value.w = alpha / 255.0f;

    switch (pin_class.pinType){

        case PinType::Flow:     iconType = IconType::Flow;   break;
        case PinType::Bool:     iconType = IconType::Circle; break;
        case PinType::Int:      iconType = IconType::Circle; break;
        case PinType::Float:    iconType = IconType::Circle; break;
        case PinType::String:   iconType = IconType::Circle; break;
        case PinType::Object:   iconType = IconType::Circle; break;
        case PinType::Function: iconType = IconType::Circle; break;
        default:
            return;
    }

    static float pin_icon_size = 24.0f;

    ax::Widgets::Icon(
        ImVec2(pin_icon_size, pin_icon_size),
        iconType,
        is_connected,
        color,
        ImColor(32, 32, 32, alpha)
    );
}

void MyGraph::MyGraph::DrawNode(
    util::BlueprintNodeBuilder& builder, 
    const NodeClass &node_class, 
    const NodeInstace &node_instance)
{
    builder.Begin(node_instance.instanceId);

    // 绘制Header
    if(node_class.nodeType != NodeType::Simple){
        builder.Header(node_class.color);

        ImGui::Spring(1);
        ImGui::TextUnformatted(node_class.name.c_str());
        ImGui::Spring(1);
        ImGui::Dummy(ImVec2(0, 28));
        ImGui::Spring(0);
        
        builder.EndHeader();
    }

    // 输入引脚部分
    for(auto& input_pin_instance_id: node_instance.inputPinsInstaceId){
        builder.Input(input_pin_instance_id);

        // TODO: new link alpha
        auto alpha = ImGui::GetStyle().Alpha;

        auto& input_pin_instance = pinInstaceMap.at(input_pin_instance_id.Get());
        auto& input_pin_class = pinClassMap.at(input_pin_instance.classId);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        DrawPinIcon(input_pin_class, IsPinLinked(input_pin_instance.instanceId), (int)(alpha * 255));
        ImGui::Spring(0);

        // 显示名字（注意绘制顺序，对于输出引脚来说这个要反过来）
        if(!input_pin_class.name.empty()){
            ImGui::TextUnformatted(input_pin_class.name.c_str());
            ImGui::Spring(0);
        }

        ImGui::PopStyleVar();
        builder.EndInput();
    }

    // 如果是simple类型的节点，那么在输入引脚之后在居中部位会有一个文本标签
    if(node_class.nodeType == NodeType::Simple){
        builder.Middle();
        ImGui::Spring(1, 0);
        ImGui::TextUnformatted(node_class.name.c_str());
        ImGui::Spring(1, 0);
    }

    // 输出引脚部分
    for(auto& output_pin_instance_id: node_instance.outputPinsInstaceId){
        builder.Output(output_pin_instance_id);

        // TODO: new link alpha
        auto alpha = ImGui::GetStyle().Alpha;

        auto& output_pin_instance = pinInstaceMap.at(output_pin_instance_id.Get());
        auto& output_pin_class = pinClassMap.at(output_pin_instance.classId);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        // 显示名字（注意绘制顺序，对于输出引脚来说这个要反过来）
        if(!output_pin_class.name.empty()){
            ImGui::Spring(0);
            ImGui::TextUnformatted(output_pin_class.name.c_str());
        }
        ImGui::Spring(0);
        DrawPinIcon(output_pin_class, IsPinLinked(output_pin_instance.instanceId), (int)(alpha * 255));

        ImGui::PopStyleVar();
        builder.EndOutput();
    }


    builder.End();
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

    static bool is_create_new_node = false;

    ed::Begin("Node editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos(); // 保存光标位置

        util::BlueprintNodeBuilder builder(
            textureHeaderBackground, 
            GetTextureWidth(textureHeaderBackground), 
            GetTextureHeight(textureHeaderBackground));

        // 绘制节点
        for(auto& node_instace_pair: nodeInstaceMap){
            auto& node_instace = node_instace_pair.second;
            DrawNode(builder, nodeClassMap.at(node_instace.classId), node_instace);
        }

        // 绘制连接
        for(auto& p: linkMap){
            auto& link = p.second;
            ed::Link(link.linkId, link.startPinInstanceId, link.endPinInstanceId, link.color, 2.0f);
        }

        // 处理对link的交互
        if(!is_create_new_node){ // 这个变量的变化是在上一帧做出的，也即如果上一帧处在createNewNode == true的状态那么下文不会执行
            // 如果上一帧没有创建新的节点，那么调用ed::BeginCreate，开始执行创建新节点的逻辑
            // 只有当用户开始从pin拉出新线的时候，会开始调用下文内容
            
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f)){

            }
            else{
                
            }

        }

        
        ImGui::SetCursorScreenPos(cursorTopLeft); // 恢复光标位置
    }
    ed::End();


    ed::SetCurrentEditor(nullptr);
}