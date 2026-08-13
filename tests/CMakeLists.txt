include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(stu_tests test_duration.cpp test_instant.cpp)
target_link_libraries(stu_tests PRIVATE stu Catch2::Catch2WithMain)

if(STU_SINGLE_HEADER)
    target_compile_definitions(stu_tests PRIVATE STU_SINGLE_HEADER_TEST)
endif()

include(CTest)
include(Catch)
catch_discover_tests(stu_tests)
