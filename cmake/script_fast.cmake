# Ĭ������£���FAST����б��룬��3rdpartyĿ¼Ѱ��
# ------------------------------------------------------------------------
IF(EXISTS "${OPENMVO_LIBS_ROOT}/3rdparty/fast")
	SET( CMAKE_OPENMVO_HAS_FAST 1)
ELSE(EXISTS "${OPENMVO_LIBS_ROOT}/3rdparty/fast")
	SET( CMAKE_OPENMVO_HAS_FAST 0)
ENDIF(EXISTS "${OPENMVO_LIBS_ROOT}/3rdparty/fast")

OPTION(DISABLE_FAST "Disable the fast library" "OFF")
MARK_AS_ADVANCED(DISABLE_FAST)
IF(DISABLE_FAST)
	SET(CMAKE_OPENMVO_HAS_FAST 0)
ENDIF(DISABLE_FAST)
