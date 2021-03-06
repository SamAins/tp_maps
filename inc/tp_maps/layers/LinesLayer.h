#ifndef tp_maps_LinesLayer_h
#define tp_maps_LinesLayer_h

#include "tp_maps/Layer.h"
#include "tp_maps/shaders/MaterialShader.h"

#include "glm/glm.hpp"

namespace tp_maps
{
class LinesLayer;
class Texture;

//##################################################################################################
struct Lines
{
  std::vector<glm::vec3> lines;
  glm::vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
  GLenum mode{GL_LINE_LOOP};
};

//##################################################################################################
class TP_MAPS_SHARED_EXPORT LinesLayer: public Layer
{
public:
  //################################################################################################
  LinesLayer();

  //################################################################################################
  ~LinesLayer()override;

  //################################################################################################
  const std::vector<Lines>& lines()const;

  //################################################################################################
  void setLines(const std::vector<Lines>& lines);

  //################################################################################################
  float lineWidth()const;

  //################################################################################################
  void setLineWidth(float lineWidth);

protected:
  //################################################################################################
  void render(RenderInfo& renderInfo) override;

  //################################################################################################
  void invalidateBuffers() override;

private:
  struct Private;
  Private* d;
  friend struct Private;
};

}

#endif
