if(USE_GTEST)
    find_package(GTest REQUIRED)
    set(Google_Tests_LIBS ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})
endif()
