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

#===============================================

add_executable(clearvoice_demo main.cpp)
target_compile_features(clearvoice_demo PUBLIC cxx_std_17)
target_include_directories(clearvoice_demo PUBLIC ${IMM_CV_HEADER})
target_link_libraries(clearvoice_demo PUBLIC ${IMM_CV_LIB})
