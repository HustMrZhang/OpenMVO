#  ��ҪΪ��ȷ����ǰCMakeLists.txt�Ѿ���ӵ���ROOT��CMakeLists.txt�У���ֹ��Դ�������Ӱ��
#  Usage:  INCLUDE(AssureCMakeRootFile)
#

IF(NOT OPENMVO_SOURCE_DIR)
	MESSAGE(FATAL_ERROR "ERROR: Do not use this directory as 'source directory' in CMake, but the ROOT directory of the OPENMVO source tree.")
ENDIF(NOT OPENMVO_SOURCE_DIR)


