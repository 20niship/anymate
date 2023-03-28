
set(COMPILED_MODULE_LIST "")
set(NOT_COMPILED_MODULE_LIST "")

macro(mu_add_module module_name)
  set(hoge "aa")
endmacro()

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()


