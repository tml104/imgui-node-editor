# pragma once

#include "imgui.h"

namespace MyGraph{

enum class PinType{
    Flow,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
};


enum class PinKind{
    Output,
    Input
};

ImColor GetIconColor(PinType type);

}