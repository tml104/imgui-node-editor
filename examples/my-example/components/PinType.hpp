# pragma once

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

}