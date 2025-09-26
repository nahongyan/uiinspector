set(gammaray_kitemmodels_srcs
  klinkitemselectionmodel.cpp
  kmodelindexproxymapper.cpp
  kdescendantsproxymodel.cpp
  kitemmodels_debug.cpp
)

add_library(gammaray_kitemmodels STATIC ${gammaray_kitemmodels_srcs})

target_link_libraries(gammaray_kitemmodels PUBLIC Qt::Core)

target_compile_definitions(gammaray_kitemmodels PUBLIC KITEMMODELS_STATICLIB=1)

if(GAMMARAY_DEFAULT_LIBRARY_PROPERTIES)
  set_target_properties(gammaray_kitemmodels PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES})
endif()
set_target_properties(gammaray_kitemmodels PROPERTIES
  OUTPUT_NAME gammaray_kitemmodels
  DEFINE_SYMBOL MAKE_KITEMMODELS_LIB
)

