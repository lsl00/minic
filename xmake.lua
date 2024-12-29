add_rules("mode.release","mode.debug")
target("minic")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")
    set_languages("c++20")

    if is_os("macosx") then 
        add_linkdirs("/opt/homebrew/lib")
        add_includedirs("/opt/homebrew/include")
    end
    
    add_links("LLVM")