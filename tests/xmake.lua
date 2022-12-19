
add_requires("catch2")

target("SAIL_test")
    set_kind("binary")
    add_deps("SAIL_lib")
    add_files("tests/**.cpp")
    add_includedirs("include")
    add_links("SAIL_lib")
    add_packages("catch2")