#include "MyGraph.hpp"

void MyGraph::MyGraph::OnStart(){
    ed::Config config;
    config.SettingsFile = (myGraphName+".json").c_str();
    editorContext = ed::CreateEditor(&config);
}

void MyGraph::MyGraph::OnStop(){

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




    ed::SetCurrentEditor(nullptr);
}