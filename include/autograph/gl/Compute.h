#pragma once
#include <autograph/gl/Bind.h>

namespace ag {
namespace gl {

////////////////////////// draw

template <typename Shader, typename... Arguments>
void dispatchCompute(Shader &&shader, int numGroupsX, int numGroupsY, int numGroupsZ,
          Arguments &&... args) {
  StateGroup sg;
  sg.mask = StateGroupMask_AllCompute;
  // 1. bind program & draw states (~= pipeline state)
  shader(sg);
  // 2. bind args
  auto dummy = {0, (args(sg), 0)...};
  // 3. call render command
  // The render command is in charge of binding the state group to the pipeline
  bindStateGroup(sg);
  glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
  glMemoryBarrier(sg.barrierBits);
}

template <typename Shader, typename... Arguments>
void dispatchComputeOnImage2D(int width, int height, int groupSizeX, int groupSizeY, Shader &&shader, Arguments &&... args) {

  auto divRoundUp = [](int numToRound, int multiple) {
    return (numToRound + multiple - 1) / multiple;
  };

  StateGroup sg;
  sg.mask = StateGroupMask_AllCompute;
  // 1. bind program & draw states (~= pipeline state)
  shader(sg);
  // 2. bind args
  auto dummy = {0, (args(sg), 0)...};
  // 3. call render command
  // The render command is in charge of binding the state group to the pipeline
  bindStateGroup(sg);
  int numGroupsX = divRoundUp(width, groupSizeX);
  int numGroupsY = divRoundUp(width, groupSizeY);
  int numGroupsZ = 1;
  glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ); 
  glMemoryBarrier(sg.barrierBits);
}

}
}
