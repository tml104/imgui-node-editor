#include "MyGraph.hpp"
#include <fstream>
#include <imgui_internal.h>

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
    std::ifstream classes_f(myGraphClassesJson);

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

    std::ifstream instance_f(myGraphInstancesJson);

    if(!instance_f.is_open()){
        return ;
    }

    json instances_j;
    instance_f >> instances_j;

    maxId = 0;

    for(auto&& node_instance: instances_j["NodeInstances"]){
        NodeInstance new_node_instance(node_instance);

        nodeInstaceMap[new_node_instance.instanceId.Get()] = new_node_instance;
        maxId = std::max(maxId, new_node_instance.instanceId.Get());
    }

    for(auto&& pin_instance: instances_j["PinInstances"]){
        PinInstance new_pin_instace(pin_instance);

        pinInstaceMap[new_pin_instace.instanceId.Get()] = new_pin_instace;
        maxId = std::max(maxId, new_pin_instace.instanceId.Get());
    }

    for(auto&& link_instace: instances_j["Links"]){
        Link new_link(link_instace);
        linkMap[std::make_pair(new_link.startPinInstanceId.Get(), new_link.endPinInstanceId.Get())] = new_link;

        maxId = std::max(maxId, new_link.linkId.Get());
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

void MyGraph::MyGraph::ShowLabel(const char *label, ImColor color)
{
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
    auto size = ImGui::CalcTextSize(label);

    auto padding = ImGui::GetStyle().FramePadding;
    auto spacing = ImGui::GetStyle().ItemSpacing;

    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

    auto rectMin = ImGui::GetCursorScreenPos() - padding;
    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

    auto drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
    ImGui::TextUnformatted(label);
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

void MyGraph::MyGraph::CreateNewNode(ull node_class_id, ImVec2 pos)
{
    auto new_node_class = nodeClassMap[node_class_id];
    NodeInstance new_node_instance(new_node_class, pos, maxId, pinClassMap, pinInstaceMap);
    nodeInstaceMap[new_node_instance.instanceId.Get()] = new_node_instance;
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
    const NodeInstance &node_instance)
{
    builder.Begin(node_instance.instanceId);

    // 绘制Header
    if(node_class.nodeType != NodeType::Simple){
        builder.Header(node_class.color);

        ImGui::Spring(0);
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
    json j;

    std::ofstream instance_f(myGraphInstancesJson, std::ofstream::out | std::ofstream::trunc); // 实际上默认模式就这样

    if(!instance_f.is_open()){
        return ;
    }

    std::vector<json> node_instance_json_vec;

    for(auto& p: nodeInstaceMap){
        node_instance_json_vec.emplace_back(p.second.ToJson());
    }

    j["NodeInstances"] = node_instance_json_vec;

    std::vector<json> pin_instances_json_vec;

    for(auto& p: pinInstaceMap){
        pin_instances_json_vec.emplace_back(p.second.ToJson());
    }

    j["PinInstances"] = pin_instances_json_vec;

    std::vector<json> link_json_vec;

    for(auto& p: linkMap){
        link_json_vec.emplace_back(p.second.ToJson());
    }

    j["Links"] = link_json_vec;

    instance_f << j.dump(4);
}

void MyGraph::MyGraph::OnStart(){
    ed::Config config;
    // config.SettingsFile = (myGraphName+".json").c_str(); // 注意不能这么写，因为内存生命周期的问题

    config.SettingsFile = "MyGraph.json";
    config.UserPointer = this;

    config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool {
        auto self = static_cast<MyGraph*>(userPointer);
        
        auto it = self->nodeInstaceMap.find(nodeId.Get());

        if(it != self->nodeInstaceMap.end()){
            it->second.location = ed::GetNodePosition(nodeId);

            return true;
        }

        return false;
    };

    editorContext = ed::CreateEditor(&config);
    ed::SetCurrentEditor(editorContext);

    LoadGraphData();
    // 加载贴图
    LoadTextures();
}

void MyGraph::MyGraph::OnStop(){
    SaveGraphData();

    // 释放贴图
    ReleaseTextures();

    if (editorContext)
    {
        ed::DestroyEditor(editorContext);
        editorContext = nullptr;
    }

}


void MyGraph::MyGraph::OnFrame(float deltaTime){
    auto& io = ImGui::GetIO();

    // 帧数显示
    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
    ImGui::Separator();

    ed::SetCurrentEditor(editorContext);

    // 右键菜单会用到的三个与右键时上下文有关的变量
    static ed::NodeId context_node_id      = 0;
    static ed::LinkId context_link_id      = 0;
    static ed::PinId  context_pin_id       = 0;

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

                ed::PinId start_pin_id = 0, end_pin_id = 0;

                if (ed::QueryNewLink(&start_pin_id, &end_pin_id)){
                    if(pinInstaceMap.count(start_pin_id.Get()) && pinInstaceMap.count(end_pin_id.Get())){
                        auto start_pin_instance = pinInstaceMap.at(start_pin_id.Get());
                        auto end_pin_instance = pinInstaceMap.at(end_pin_id.Get());
                        auto start_pin_class = pinClassMap.at(start_pin_instance.classId);
                        auto end_pin_class = pinClassMap.at(end_pin_instance.classId);

                        if(start_pin_class.pinKind == PinKind::Input && end_pin_class.pinKind == PinKind::Output){
                            std::swap(start_pin_instance, end_pin_instance);
                            std::swap(start_pin_class, end_pin_class);
                            std::swap(start_pin_id, end_pin_id);
                        }

                        if(start_pin_id == end_pin_id){
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f); // 拒绝用红色，下同
                        }
                        else if(start_pin_class.pinType != end_pin_class.pinType){
                            ShowLabel("x: no compatible pin type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f); // 拒绝用红色，下同
                        }
                        else if(start_pin_class.pinKind == end_pin_class.pinKind){
                            ShowLabel("x: no compatible pin kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f); // 拒绝用红色，下同
                        }
                        else{
                            ShowLabel("+: create new link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) // 接受用绿色
                            {
                                Link new_link{++maxId, start_pin_id, end_pin_id};
                                linkMap[std::make_pair(start_pin_id.Get(), end_pin_id.Get())] = new_link;
                            }

                        }

                    }
                }

                ed::PinId new_node_pin_id;
                if(ed::QueryNewNode(&new_node_pin_id)){
                    // TODO: 创建新节点
                }
            }
            else{
                
            }

            ed::EndCreate();

            if (ed::BeginDelete()){
                ed::NodeId node_id = 0;
                while(ed::QueryDeletedNode(&node_id)){
                    if(ed::AcceptDeletedItem()){

                        // 删除Pin，然后删除对应的Node

                        auto node_instance = nodeInstaceMap.at(node_id.Get());

                        for(auto& input_pin_id: node_instance.inputPinsInstaceId){
                            pinInstaceMap.erase(input_pin_id.Get());
                        }

                        for(auto& output_pin_id: node_instance.outputPinsInstaceId){
                            pinInstaceMap.erase(output_pin_id.Get());
                        }

                        nodeInstaceMap.erase(node_id.Get());
                    }
                }

                ed::LinkId link_id = 0;
                while(ed::QueryDeletedLink(&link_id)){
                    if(ed::AcceptDeletedItem()){
                        auto it = std::find_if(linkMap.begin(), linkMap.end(), [link_id](auto& p){return p.second.linkId == link_id;});
                        if(it != linkMap.end()){
                            linkMap.erase(it);
                        }
                    }
                }
            }
            ed::EndDelete();

        }

        
        ImGui::SetCursorScreenPos(cursorTopLeft); // 恢复光标位置
    }

    // 菜单
    {
        auto open_popup_pos = ImGui::GetMousePos(); // 这个只是用来控制通过右键菜单创建新的节点时，新节点的位置

        ed::Suspend();
        
        if(ed::ShowBackgroundContextMenu()){
            ImGui::OpenPopup("Create New Node");
        }
        ed::Resume();

        ed::Suspend();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8)); // 让内容与窗口边缘有点距离（8个像素）

        if (ImGui::BeginPopup("Create New Node")) // 右键空白区域时的逻辑
        {
            auto new_node_pos = open_popup_pos; // 这个只是用来控制通过右键菜单创建新的节点时，新节点的位置

            #if 0
            if(ImGui::MenuItem("123")){

            }
            ImGui::Separator();

            if(ImGui::MenuItem("456")){

            }
            #endif
            
            for(auto& p: nodeClassMap){
                if(ImGui::MenuItem(p.second.name.c_str())){
                    CreateNewNode(p.second.classId, new_node_pos);
                }
            }

            ImGui::EndPopup();
        }
        
        ImGui::PopStyleVar();
        ed::Resume();
    }

    ed::End();


    // ed::SetCurrentEditor(nullptr);
}