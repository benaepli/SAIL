add_rules("mode.debug", "mode.release")

add_requires("fmt", "magic_enum", "unordered_dense v3.0.0")

set_project("SAIL")
set_version("0.1.0")
set_languages("c++20")

target("SAIL_lib")
    set_kind("static")

    add_files("src/**.cpp")
    add_headerfiles("include/**.h")
    add_includedirs("include")
    
    add_packages("fmt", "magic_enum", "unordered_dense")

    if is_mode("debug") then
        add_defines("SAIL_DEBUG")
    end

    if is_kind("shared") then
        add_defines("SAIL_EXPORT=__declspec(dllexport)")
    else
        add_defines("SAIL_EXPORT=")
    end

target("SAIL")
    set_kind("binary")
    add_deps("SAIL_lib")
    add_files("exec/**.cpp")
    add_includedirs("include")
    add_links("SAIL_lib")

includes("tests/xmake.lua")