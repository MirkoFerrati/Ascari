#ifndef FORMATION_CONTROL_VIEWER_H
#define FORMATION_CONTROL_VIEWER_H
#include "../plugins/abstract_viewer_plugin.h"

#ifdef ISVIEWER

#include "yaml_parser.h"
#include <boost/circular_buffer.hpp>
#include "../viewer/gui/viewer.h"


class formation_control_viewer : public abstract_viewer_plugin
{
public:
    formation_control_viewer();
    ~formation_control_viewer();  
};

#endif
#endif // FORMATION_CONTROL_VIEWER_H
