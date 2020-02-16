# Builds all versions of the 64-bit libraries

all: ttlibwinC ttlibwinD ttlibwinR ttlibC ttlibD ttlibR ttlibwxC ttlibwxD ttlibwxR results

ttlibwinC:
	cd winsrc & ttBld.exe -uclangD & ninja -f bld/clang_dbg.ninja

ttlibwinD:
	cd winsrc & ttBld.exe -umsvcD & ninja -f bld/msvc_dbg.ninja

ttlibwinR:
	cd winsrc & ttBld.exe -umsvc & ninja -f bld/msvc_rel.ninja

ttlibC:
	cd src & ttBld.exe -uclangD & ninja -f bld/clang_dbg.ninja

ttlibD:
	cd src & ttBld.exe -umsvcD & ninja -f bld/msvc_dbg.ninja

ttlibR:
	cd src & ttBld.exe -umsvc & ninja -f bld/msvc_rel.ninja

ttlibwxC:
	cd wxsrc & ttBld.exe -uclangD & ninja -f bld/clang_dbg.ninja

ttlibwxD:
	cd wxsrc & ttBld.exe -umsvcD & ninja -f bld/msvc_dbg.ninja

ttlibwxR:
	cd wxsrc & ttBld.exe -umsvc & ninja -f bld/msvc_rel.ninja

results:
	dir ..\lib\ttlib*.lib
