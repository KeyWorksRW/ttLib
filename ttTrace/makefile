# Call with nmake.exe (MSVC) or a GNU make variation (mingw32-make.exe, gmake.exe, or make.exe)

# mingw32-make release 		 <-- this will build release target(s)
# mingw32-make clean release <-- this will clean and rebuild release target(s)
# mingw32-make cleanD debug  <-- this will clean and rebuild debug target(s)

# nmake release 	  <-- this will build release target(s)
# nmake clean release <-- this will clean and rebuild release target(s)
# nmake cleanD debug  <-- this will clean and rebuild debug target(s)

# If the environment variable "b64" is set (or b64=1 is part of the make command line), then a '64' suffix is added to
# all ninja build script names. In addition, if you add a LIB64 environment variable that points to all 64-bit LIB paths,
# then your LIB environment will be changed to the contents of LIB64 before building.

# If you are using the MSVC compiler, then you should also set a PATH64 environment variable ahead of time to point to
# the 64-bit compiler/linker.

# Remove the '#' character in the following line to always build 64-bit targets

b64=1

# \
!ifndef 0 # \
# Following section is for nmake \
!ifdef b64 # \
bits=64 # \
!ifdef LIB64 # \
LIB=$(LIB64) # \
!endif # \
!ifdef PATH64 # \
PATH=$(PATH64) # \
!endif  # \
!else # \
bits=32 # \
!endif  # b64 \
!ifndef cmplr # \
cmplr=msvc # \
!endif

# \
!else
# Following section is for GNU make and variants

ifdef b64
	bits=64
	ifdef LIB64
		LIB=$(LIB64)
	endif
	ifdef PATH64
		PATH=$(PATH64)
	endif
else
	bits=32
endif

ifndef cmplr
	cmplr=clang
endif

# \
!endif

########## release section ##########

release: ttLib build/$(cmplr)Build$(bits).ninja ttTrace

ttLib:
	cd ../src/ & ninja -f build/$(cmplr)Build$(bits).ninja

build/$(cmplr)Build$(bits).ninja: .srcfiles
	MakeNinja -force

ttTrace:
	ninja -f build/$(cmplr)Build$(bits).ninja

########## debug section ##########

debug: ttLibD build/$(cmplr)Build$(bits)D.ninja ttTraceD

ttLibD:
	cd ../src/ & ninja -f build/$(cmplr)Build$(bits)D.ninja

build/$(cmplr)Build$(bits)D.ninja: .srcfiles
	MakeNinja -force

ttTraceD:
	ninja -f build/$(cmplr)Build$(bits)D.ninja

########## clean release section ##########

clean:
	ninja -f build/$(cmplr)Build$(bits).ninja -t clean

########## clean debug section ##########

cleanD:
	ninja -f build/$(cmplr)Build$(bits)D.ninja -t clean
