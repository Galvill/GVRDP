function(set_compiler_warnings target)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
            -Wall -Wextra -Wpedantic
            -Wshadow -Wnon-virtual-dtor
            -Wcast-align -Wunused
            -Woverloaded-virtual
            -Wconversion -Wsign-conversion
            -Wnull-dereference
            -Wformat=2
        >
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4 /permissive-
        >
    )
endfunction()
