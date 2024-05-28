message(STATUS "Configuring ClearVoice Demo...")
cmake_minimum_required(VERSION 3.20)
project("Immersitech ClearVoice Demo" LANGUAGES C CXX)

#============== Set these =====================

set(IMM_CV_HEADER 
    "<full/path/to/immersitech_clearvoice.h/folder>" #path should NOT include the header file itself
)

set(IMM_CV_LIB 
    "<full/path/to/imm_clearvoice/library>" #path should include the library file
)

set(INTEL_IPP_INCLUDE 
    "<full/path/to/intel/ipp/include/folder>" #path should NOT include the header file itself
)

set(INTEL_IPP_LIBS 
    "<full/path/to/intel/ipp/libippcore.a>"
    "<full/path/to/intel/ipp/libippi.a>"
    "<full/path/to/intel/ipp/libipps.a>"
    "<full/path/to/intel/ipp/libippvm.a>" #paths should include the library file
)

#===============================================

set(COMPILE_DEFS "")

if (APPLE)
    EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
    if(${ARCHITECTURE} STREQUAL "arm64")
        set(COMPILE_DEFS "-framework Accelerate")
    endif()
endif()


add_executable(clearvoice_demo main.cpp)
target_compile_features(clearvoice_demo PUBLIC cxx_std_17)
target_include_directories(clearvoice_demo PUBLIC ${IMM_CV_HEADER})
target_include_directories(clearvoice_demo PUBLIC ${INTEL_IPP_INCLUDE})
target_compile_definitions(clearvoice_demo PUBLIC ${COMPILE_DEFS})
target_link_libraries(clearvoice_demo PUBLIC ${IMM_CV_LIB})

# Only link Intel Libs to Intel Mac or Windows
if (APPLE)
    if(${ARCHITECTURE} STREQUAL "x86_64")
        target_link_libraries(clearvoice_demo PUBLIC ${INTEL_IPP_LIBS})
    endif()
elseif(WIN32)
    target_link_libraries(clearvoice_demo PUBLIC ${INTEL_IPP_LIBS})
endif()
