# 작성자: 윤정도
# 써드파티 라이브러리 빌드를 위한 용도

cmake_minimum_required(VERSION 3.25)

project(UltimateHarmony)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(BUILD_SHARED_LIBS off)              # 정적 라이브러리 빌드용
set(BENCHMARK_ENABLE_TESTING off)       # benchmark/test는 빌드 제외하도록함

add_subdirectory(ThirdParty/GoogleBenchmark)