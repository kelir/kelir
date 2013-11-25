#ifndef ENUMS_HPP
#define ENUMS_HPP

typedef enum {
  TOOL_NONE = 0,
  TOOL_HAND,
  TOOL_ERASER,
  TOOL_PENCIL,
  TOOL_LINE,

  TOOLS_COUNT
} ToolsEnums;

typedef enum {
  LAYER_VECTOR = 0,
  LAYER_BITMAP,
  LAYER_CAMERA,
  LAYER_GROUP,

  LAYER_COUNT
} LayerEnum;

#endif /* ENUMS_HPP */
