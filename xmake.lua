set_project("cmdparser")
set_version("0.1.0")

add_rules("mode.debug", "mode.release", "plugin.compile_commands.autoupdate", {outputdir = "build"})
set_languages("c11")
set_warnings("all")

if is_plat("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
end

target("cmdparser")
    set_kind("static")
    add_files("cmdparser.c")
    add_headerfiles("cmdparser.h")
    add_includedirs(".", {public = true})

target("demo_simple")
    set_kind("binary")
    set_default(false)
    add_deps("cmdparser")
    add_files("demo_simple/main.c")

target("demo_git")
    set_kind("binary")
    set_default(false)
    add_deps("cmdparser")
    add_files("demo_git/*.c")

target("cmdparser_test")
    set_kind("binary")
    set_default(false)
    add_deps("cmdparser")
    add_files("test/*.c")
    add_includedirs("test")
    add_tests("default")
    if not is_plat("windows") then
        add_defines("_GNU_SOURCE")
    end
