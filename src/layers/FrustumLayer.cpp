#include "tp_maps/layers/FrustumLayer.h"
#include "tp_maps/shaders/LineShader.h"
#include "tp_maps/Map.h"
#include "tp_maps/RenderInfo.h"
#include "tp_maps/Controller.h"

#include <vector>

namespace tp_maps
{
namespace
{
struct LinesDetails_lt
{
  LineShader::VertexBuffer* vertexBuffer{nullptr};
  glm::vec4 color{0.0f, 0.0f, 0.0f, 1.0f};
};
}

//##################################################################################################
struct FrustumLayer::Private
{
  TP_NONCOPYABLE(Private);
  FrustumLayer* q;

  //Processed geometry ready for rendering
  bool updateVertexBuffer{true};
  std::vector<LinesDetails_lt> processedGeometry;

  glm::mat4 cameraMatrix{glm::mat4(1)};
  glm::mat4 inverseCameraMatrix{glm::mat4(1)};

  bool renderFrustumBorder{true};
  bool renderRays{false};

  glm::vec4 frustumBorderColor{1.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 raysColor{0.0f, 1.0f, 0.0f, 1.0f};

  //################################################################################################
  Private(FrustumLayer* q_):
    q(q_)
  {

  }

  //################################################################################################
  ~Private()
  {
    deleteVertexBuffers();
  }

  //################################################################################################
  void deleteVertexBuffers()
  {
    for(const auto& details : processedGeometry)
      delete details.vertexBuffer;

    processedGeometry.clear();
  }
};

//##################################################################################################
FrustumLayer::FrustumLayer():
  d(new Private(this))
{
}

//##################################################################################################
FrustumLayer::~FrustumLayer()
{
  delete d;
}

//##################################################################################################
void FrustumLayer::setCameraMatrix(const glm::mat4& matrix)
{
  d->cameraMatrix = matrix;
  d->inverseCameraMatrix = glm::inverse(matrix);
  d->updateVertexBuffer = true;
  update();
}

//##################################################################################################
bool FrustumLayer::renderFrustumBorder() const
{
  return d->renderFrustumBorder;
}

//##################################################################################################
void FrustumLayer::setRenderFrustumBorder(bool renderFrustumBorder)
{
  d->renderFrustumBorder = renderFrustumBorder;
  d->updateVertexBuffer = true;
  update();
}

//##################################################################################################
bool FrustumLayer::renderRays() const
{
  return d->renderRays;
}

//##################################################################################################
void FrustumLayer::setRenderRays(bool renderRays)
{
  d->renderRays = renderRays;
  d->updateVertexBuffer = true;
  update();
}

//##################################################################################################
glm::vec3 FrustumLayer::frustumBorderColor() const
{
  return d->frustumBorderColor;
}

//##################################################################################################
void FrustumLayer::setFrustumBorderColor(const glm::vec4& frustumBorderColor)
{
  d->frustumBorderColor = frustumBorderColor;
  d->updateVertexBuffer = true;
  update();
}

//##################################################################################################
glm::vec3 FrustumLayer::raysColor() const
{
  return d->raysColor;
}

//##################################################################################################
void FrustumLayer::setRaysColor(const glm::vec4& raysColor)
{
  d->raysColor = raysColor;
  d->updateVertexBuffer = true;
  update();
}

//##################################################################################################
void FrustumLayer::render(RenderInfo& renderInfo)
{
  if(renderInfo.pass != RenderPass::Normal)
    return;

  auto shader = map()->getShader<LineShader>();
  if(shader->error())
    return;

  if(d->updateVertexBuffer)
  {
    d->deleteVertexBuffers();
    d->updateVertexBuffer=false;

    auto addLine = [this](std::vector<glm::vec3>& vertices, const glm::vec3& start, const glm::vec3& end)
    {
      auto addVert = [this, &vertices](const glm::vec3& vert)
      {
        glm::vec4 tmp = glm::vec4(vert.x, vert.y, vert.z, 1.0f);

        glm::vec4 obj = d->inverseCameraMatrix * tmp;
        obj /= obj.w;
        vertices.emplace_back(obj);
      };

      addVert(start);
      addVert(end);
    };

    if(d->renderRays)
    {
      std::vector<glm::vec3> vertices;
      for(float x=-1; x<=1.01f; x+=0.1f)
      {
        for(float y=-1; y<=1.01f; y+=0.1f)
        {
          addLine(vertices, {x, y, 0.0f}, {x, y, 1.0f});
        }
      }

      LinesDetails_lt details;
      details.vertexBuffer = shader->generateVertexBuffer(map(), vertices);
      details.color = d->raysColor;
      d->processedGeometry.push_back(details);
    }

    if(d->renderFrustumBorder)
    {
      std::vector<glm::vec3> vertices;

      // Near quad
      addLine(vertices, {-1.0f, -1.0f,-1.0f}, { 1.0f, -1.0f,-1.0f});
      addLine(vertices, { 1.0f, -1.0f,-1.0f}, { 1.0f,  1.0f,-1.0f});
      addLine(vertices, { 1.0f,  1.0f,-1.0f}, {-1.0f,  1.0f,-1.0f});
      addLine(vertices, {-1.0f,  1.0f,-1.0f}, {-1.0f, -1.0f,-1.0f});

      // Far quad
      addLine(vertices, {-1.0f, -1.0f, 1.0f}, { 1.0f, -1.0f, 1.0f});
      addLine(vertices, { 1.0f, -1.0f, 1.0f}, { 1.0f,  1.0f, 1.0f});
      addLine(vertices, { 1.0f,  1.0f, 1.0f}, {-1.0f,  1.0f, 1.0f});
      addLine(vertices, {-1.0f,  1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f});

      // Corner lines
      addLine(vertices, {-1.0f, -1.0f,-1.0f}, {-1.0f, -1.0f, 1.0f});
      addLine(vertices, { 1.0f, -1.0f,-1.0f}, { 1.0f, -1.0f, 1.0f});
      addLine(vertices, { 1.0f,  1.0f,-1.0f}, { 1.0f,  1.0f, 1.0f});
      addLine(vertices, {-1.0f,  1.0f,-1.0f}, {-1.0f,  1.0f, 1.0f});

      LinesDetails_lt details;
      details.vertexBuffer = shader->generateVertexBuffer(map(), vertices);
      details.color = d->frustumBorderColor;
      d->processedGeometry.push_back(details);
    }
  }

  shader->use();
  shader->setMatrix(map()->controller()->matrix(coordinateSystem()));
  shader->setLineWidth(1.0f);

  for(const LinesDetails_lt& line : d->processedGeometry)
  {
    shader->setColor(line.color);
    shader->drawLines(GL_LINES, line.vertexBuffer);
  }
}

//##################################################################################################
void FrustumLayer::invalidateBuffers()
{
  d->deleteVertexBuffers();
  d->updateVertexBuffer=true;
}

}
