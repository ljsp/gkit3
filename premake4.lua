dofile "gkit.lua"

 -- description des projets		 
projects = {
	"tp1",
	"tp2",
    "tp2_ex1",
    "tp2_ex2",
    "tp2_ex3"
}

for i, name in ipairs(projects) do
    project(name)
        language "C++"
        kind "ConsoleApp"
        targetdir "bin"
        files ( gkit_files )
        files { "projets/" .. name .. ".cpp" }
end
