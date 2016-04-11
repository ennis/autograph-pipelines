
# vulkan
if (VULKAN_FOUND)
add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE ${VULKAN_INCLUDE_DIR})
target_link_libraries(vulkan INTERFACE ${VULKAN_LIBRARY})
endif()


# GSL (guideline support library)
add_library(gsl INTERFACE)
target_include_directories(gsl INTERFACE GSL/)