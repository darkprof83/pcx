set( pcx_SOURCES ReaderWriterPCX.cpp )
set( CMAKE_SHARED_LIBRARY_PREFIX "" )
add_library( osgdb_pcx SHARED ${pcx_SOURCES} )
target_link_libraries (osgdb_pcx 
  ${Boost_LIBRARIES}
  #${OSGVIEWER_LIBRARY}
  ${OSG_LIBRARIES}
  #${OSGINTROSPECTION_LIBRARY}
  #${OSGMANIPULATOR_LIBRARY}
  #${OSGSHADOW_LIBRARY}
  #${OSGSIM_LIBRARY}
  #${OSGTERRAIN_LIBRARY}
  #${OSGFX_LIBRARY}
  #${OSGPARTICLE_LIBRARY}
  #${OSGTEXT_LIBRARY}
  #${OSGGA_LIBRARY}
  #${OSGUTIL_LIBRARY} 
  ${OSGDB_LIBRARY} 
  #${OSGVIEWER_LIBRARY} 
  #${OPENTHREADS_LIBRARY}
)
