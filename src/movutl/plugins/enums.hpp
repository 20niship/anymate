#pragma once
namespace mu::plugin {
enum class MU_Err { NONE = 0, OUT_OF_MEMORY, INTERNL_STRUCT_DAmAGED, INVALID_INDEX, _UNRECOGNIZED_PARAM_TYPE, INVALID_CALLBACK, BAD_CALLBACK_PARAM, INTERRUPT_CANCEL, CANNOT_PARSE_KEYFRAME_TEXT };

enum class PluginOutFlag : long long {
  None                            = 0L,
  KEEP_RESOURCE_OPEN              = 1L << 0,
  WIDE_TIME_INPUT                 = 1L << 1,  // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  NON_PARAM_VARY                  = 1L << 2,  // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  SEQUENCE_DATA_NEEDS_FLATTENING  = 1L << 4,  // PF_Cmd_GLOBAL_SETUP
  I_DO_DIALOG                     = 1L << 5,  // PF_Cmd_GLOBAL_SETUP
  USE_OUTPUT_EXTENT               = 1L << 6,  // PF_Cmd_GLOBAL_SETUP
  SEND_DO_DIALOG                  = 1L << 7,  // PF_Cmd_SEQUENCE_SETUP
  DISPLAY_ERROR_MESSAGE           = 1L << 8,  // all PF_Cmds
  I_EXPAND_BUFFER                 = 1L << 9,  // PF_Cmd_GLOBAL_SETUP
  PIX_INDEPENDENT                 = 1L << 10, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  I_WRITE_INPUT_BUFFER            = 1L << 11, // PF_Cmd_GLOBAL_SETUP
  I_SHRINK_BUFFER                 = 1L << 12, // PF_Cmd_GLOBAL_SETUP
  WORKS_IN_PLACE                  = 1L << 13, // PF_Cmd_GLOBAL_SETUP
  CUSTOM_UI                       = 1L << 15, // PF_Cmd_GLOBAL_SETUP
  REFRESH_UI                      = 1L << 17, // PF_Cmd_EVENT, PF_Cmd_RENDER, PF_Cmd_DO_DIALOG
  NOP_RENDER                      = 1L << 18, // PF_Cmd_GLOBAL_SETUP
  I_USE_SHUTTER_ANGLE             = 1L << 19, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  I_USE_AUDIO                     = 1L << 20, // PF_Cmd_GLOBAL_SETUP
  I_AM_OBSOLETE                   = 1L << 21, // PF_Cmd_GLOBAL_SETUP
  FORCE_RERENDER                  = 1L << 22, // PF_Cmd_EVENT, PF_Cmd_USER_CHANGED_PARAM, PF_Cmd_UPDATE_PARAMS_UI
  PiPL_OVERRIDES_OUTDATA_OUTFLAGS = 1L << 23, // PiPL-only-flag
  I_HAVE_EXTERNAL_DEPENDENCIES    = 1L << 24, // PF_Cmd_GLOBAL_SETUP
  DEEP_COLOR_AWARE                = 1L << 25, // PF_Cmd_GLOBAL_SETUP
  SEND_UPDATE_PARAMS_UI           = 1L << 26, // PF_Cmd_GLOBAL_SETUP

  // audio flags (PF_OutFlag_AUDIO_EFFECT_TOO or PF_OutFlag_AUDIO_EFFECT_ONLY required for audio effects)
  AUDIO_FLOAT_ONLY   = 1L << 27, // PF_Cmd_GLOBAL_SETUP
  AUDIO_IIR          = 1L << 28, // PF_Cmd_GLOBAL_SETUP
  I_SYNTHESIZE_AUDIO = 1L << 29, // PF_Cmd_GLOBAL_SETUP
  AUDIO_EFFECT_TOO   = 1L << 30, // PF_Cmd_GLOBAL_SETUP
  AUDIO_EFFECT_ONLY  = 1L << 31, // PF_Cmd_GLOBAL_SETUP

  // Flag 2
  SUPPORTS_QUERY_DYNAMIC_FLAGS          = 1L << 32, // PF_Cmd_GLOBAL_SETUP
  I_USE_3D_CAMERA                       = 1L << 33, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  I_USE_3D_LIGHTS                       = 1L << 34, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  PARAM_GROUP_START_COLLAPSED_FLAG      = 1L << 35, // PF_Cmd_GLOBAL_SETUP
  I_AM_THREADSAFE                       = 1L << 36, // PF_Cmd_GLOBAL_SETUP (unused)
  CAN_COMBINE_WITH_DESTINATION          = 1L << 37, // Premiere only (as of AE 6.0)
  DOESNT_NEED_EMPTY_PIXELS              = 1L << 38, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  REVEALS_ZERO_ALPHA                    = 1L << 39, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  PRESERVES_FULLY_OPAQUE_PIXELS         = 1L << 40, // Premiere only (as of AE 6.0)
  SUPPORTS_SMART_RENDER                 = 1L << 42, // PF_Cmd_GLOBAL_SETUP
  FLOAT_COLOR_AWARE                     = 1L << 44, // PF_Cmd_GLOBAL_SETUP, may require PF_OutFlag2_SUPPORTS_SMART_RENDER
  I_USE_COLORSPACE_ENUMERATION          = 1L << 45, // PF_Cmd_GLOBAL_SETUP, not implemented in AE7 (may be impl in Premiere Pro)
  I_AM_DEPRECATED                       = 1L << 46, // PF_Cmd_GLOBAL_SETUP
  DO_NOT_CLONE_SEQUENCE_DATA_FOR_RENDER = 1L << 47, // PF_Cmd_GLOBAL_SETUP, Premiere only, CS4.1 and later
  AUTOMATIC_WIDE_TIME_INPUT             = 1L << 49, // PF_Cmd_GLOBAL_SETUP, falls back to PF_OutFlag_WIDE_TIME_INPUT if not PF_OutFlag2_SUPPORTS_SMART_RENDER
  I_USE_TIMECODE                        = 1L << 50, // PF_Cmd_GLOBAL_SETUP
  DEPENDS_ON_UNREFERENCED_MASKS         = 1L << 51, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  OUTPUT_IS_WATERMARKED                 = 1L << 52, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_QUERY_DYNAMIC_FLAGS
  I_MIX_GUID_DEPENDENCIES               = 1L << 53, // PF_Cmd_GLOBAL_SETUP
  AE13_5_THREADSAFE                     = 1L << 54, // PF_Cmd_GLOBAL_SETUP (unused)
  SUPPORTS_GET_FLATTENED_SEQUENCE_DATA  = 1L << 55, // PF_Cmd_GLOBAL_SETUP, support required if both PF_OutFlag_SEQUENCE_DATA_NEEDS_FLATTENING and PF_OutFlag2_SUPPORTS_THREADED_RENDERING is set
  CUSTOM_UI_ASYNC_MANAGER               = 1L << 56, // PF_Cmd_GLOBAL_SETUP
  SUPPORTS_GPU_RENDER_F32               = 1L << 57, // PF_Cmd_GLOBAL_SETUP, PF_Cmd_GPU_DEVICE_SETUP. Must also set PF_RenderOutputFlag_GPU_RENDER_POSSIBLE at pre-render to enable GPU rendering.
  SUPPORTS_THREADED_RENDERING           = 1L << 58, // PF_Cmd_GLOBAL_SETUP
  MUTABLE_RENDER_SEQUENCE_DATA_SLOWER   = 1L << 59  // PF_Cmd_GLOBAL_SETUP
};


} // namespace mu::plugin
