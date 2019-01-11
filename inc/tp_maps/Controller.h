#ifndef tp_maps_Controller_h
#define tp_maps_Controller_h

#include "tp_maps/Globals.h"

#include "json.hpp"

#include "glm/glm.hpp"

#include <functional>

namespace tp_maps
{
class Map;
struct MouseEvent;
struct KeyEvent;

//##################################################################################################
class TP_MAPS_SHARED_EXPORT Controller
{
  friend class Map;

public:
  //################################################################################################
  //! Construct a Controller for map
  /*!
  This will construct a Controller for map, it will replace the current controller if there
  is one. The Map will take ownership of this new controller. The Map will call
  updateMatrices() when it needs the matrices to be updated.
  */
  Controller(Map* map);

  //################################################################################################
  struct Matrices
  {
    glm::mat4 v{1.0f};
    glm::mat4 p{1.0f};

    glm::mat4 vp{1.0f};

    glm::vec3 cameraOriginNear{0.0f, 0.0f, 0.0f};
    glm::vec3 cameraOriginFar {0.0f, 0.0f, 1.0f};
  };

  //################################################################################################
  //! Returns the camera matrix for a given coordinate system
  /*!
  It is possible to have multiple coordinate systems in use on a single map. For example you always
  have the default coordinate system defined by the controller, and the screen coordinate system so
  that drawing can be performed in pixels.

  These should exist:
  <ul>
    <li><b>Default</b> - The default coordinate system defined by the controller.
    <li><b>Screen</b> - Drawing in pixel coords.
  </ul>

  \param coordinateSystem - The type of coordinate system to use.
  \return The requested matrix, or an identity.
  */
  glm::mat4 matrix(const tp_utils::StringID& coordinateSystem)const;

  //################################################################################################
  //! This is basically the same as matrix however it returns the view and projection separatly.
  Matrices matrices(const tp_utils::StringID& coordinateSystem)const;

  //################################################################################################
  struct Scissor
  {
    bool valid{false};
    int x{0};
    int y{0};
    int width{0};
    int height{0};
  };

  //################################################################################################
  Scissor scissor(const tp_utils::StringID& coordinateSystem)const;

  //################################################################################################
  virtual void enableScissor(const tp_utils::StringID& coordinateSystem);

  //################################################################################################
  virtual void disableScissor();

  //################################################################################################
  //! Save the state of the controller
  /*!
  This should be re-implemented to save the state of the controller, see the \link Serialization
  \endlink page for more detail of how this should be done.

  \returns The state of the controller serialized to a byte array
  */
  virtual nlohmann::json saveState()const=0;

  //################################################################################################
  //! Load the state of the controller
  /*!
  This should be re-implemented to load the state of the controller, see the \link Serialization
  \endlink page for more detail of how this should be done.

  \param byteArray - The state to load previously generated by saveState().
  */
  virtual void loadState(const nlohmann::json& j)=0;

  //################################################################################################
  void setMouseClickCallback(const std::function<void(const MouseEvent&)>& mouseClickCallback);

  //################################################################################################
  //! Returns the Map that this should control
  /*!
  This will always return a valid Map
  */
  Map* map()const;

protected:
  //################################################################################################
  virtual ~Controller();

  //################################################################################################
  void setMatrix(const tp_utils::StringID& coordinateSystem, const glm::mat4& matrix);

  //################################################################################################
  void setMatrices(const tp_utils::StringID& coordinateSystem, const Matrices& matrices);

  //################################################################################################
  void setScissor(const tp_utils::StringID& coordinateSystem, int x, int y, int width, int height);

  //################################################################################################
  //! Called by the map when it is resized
  virtual void mapResized(int w, int h)=0;

  //################################################################################################
  //! Called to update the matrices
  /*!
  This will be called by the Map when it needs the matrices to be updated, you should re-implement
  this to implement custom camera setups.
  */
  virtual void updateMatrices()=0;

  //################################################################################################
  virtual bool mouseEvent(const MouseEvent& event)=0;

  //################################################################################################
  virtual bool keyEvent(const KeyEvent& event);

  //################################################################################################
  //! Update the state of the animation
  virtual void animate(double timestampMS);

  //################################################################################################
  void callMouseClickCallback(const MouseEvent& event) const;

private:
  struct Private;
  Private* d;
  friend struct Private;
};

}

#endif
