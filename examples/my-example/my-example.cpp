#define IMGUI_DEFINE_MATH_OPERATORS
#include <application.h>
#include "utilities/builders.h"
#include "utilities/widgets.h"

#include <imgui_node_editor.h>
#include <imgui_internal.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>

#include "components/NodeType.hpp"
#include "components/PinType.hpp"

#include "components/Pin.hpp"
#include "components/Node.hpp"
#include "components/Link.hpp"

#include "MyGraph.hpp"

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;


int Main(int argc, char** argv)
{
    MyGraph::MyGraph mygraph("MyGraph", argc, argv);

    if (mygraph.Create())
        return mygraph.Run();

    return 0;
}