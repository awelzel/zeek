########################################################################
## Benchmarks

macro(ADD_BENCH_TARGET _name)
    set(_bench_target ${_name}-bench)
    set(_bench_source ${_name}-bench.cc)

    add_executable(${_bench_target} ${_bench_source} ${ARGN})

    target_link_libraries(${_bench_target} zeek_bench_shared)
    target_link_libraries(${_bench_target} ${zeekdeps} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endmacro ()

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(zeek_bench_shared SHARED
            $<TARGET_OBJECTS:zeek_objs>
            ${bro_SUBDIR_LIBS}
            ${bro_PLUGIN_LIBS}
)

set(zeek_bench_shared_deps)
foreach(_dep ${zeekdeps} )
        set(zeek_bench_shared_deps ${zeek_fuzzer_shared_deps} ${_dep})
endforeach ()

target_link_libraries(zeek_bench_shared
                      ${zeek_bench_shared_deps}
                      ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})


add_bench_target("map")
