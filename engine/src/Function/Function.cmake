set(module_name "Function")

file(GLOB_RECURSE kaubo_${module_name}_files ${kaubo_src_dir}/${module_name}/*.cpp)
set(kaubo_common_files ${kaubo_common_files} ${kaubo_${module_name}_files})