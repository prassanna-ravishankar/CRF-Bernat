#add_executable(example-grid-potts grid_potts.cxx ${headers})
#add_executable(example-recognition recognition.cxx ${headers})
#add_executable(example-segmentation interpixel_boundary_segmentation.cxx ${headers})



add_executable(example-Read Read.cpp ${headers})

if(WIN32 OR APPLE)

else()
  find_library(RT_LIBRARY rt)
  #target_link_libraries(example-grid-potts rt)
  #target_link_libraries(example-recognition rt)
  target_link_libraries(example-Read rt)
endif()

