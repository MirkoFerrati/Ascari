#ifdef ISVIEWER

#include "formation_control_viewer.h"


formation_control_viewer::formation_control_viewer(){}

formation_control_viewer::~formation_control_viewer(){}

void formation_control_viewer::init()
{
  assert(father);
  Viewer* temp_father=reinterpret_cast<Viewer*>(father);
  temp_father->setScalingAndTranslateFactor(200, -200, 200, -200);
}

#endif
