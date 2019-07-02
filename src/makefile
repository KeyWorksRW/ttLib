# Call with nmake.exe (MSVC) or a GNU make variation (mingw32-make.exe, gmake.exe, or make.exe)

# mingw32-make release 		 <-- this will build release target(s)
# mingw32-make clean release <-- this will clean and rebuild release target(s)
# mingw32-make cleanD debug  <-- this will clean and rebuild debug target(s)

# nmake release 	  <-- this will build release target(s)
# nmake clean release <-- this will clean and rebuild release target(s)
# nmake cleanD debug  <-- this will clean and rebuild debug target(s)

# By default, a 64 is appended to all .ninja script filenames to build a 64-bit target. However,  If the environment
# variable "b32" is set (or b32=1 is part of the make command line), then a '32' suffix is added instead to all ninja
# build script names. In addition, if you add a LIB32 environment variable that points to all 32-bit LIB paths, then your
# LIB environment will be changed to the contents of LIB32 before building.

# If you are using the MSVC compiler, then you should also set a PATH32 environment variable ahead of time to point to
# the 32-bit compiler/linker. Not that this is not needed for the CLANG compiler

# Remove the '#' character in the following line to always build 32-bit targets

b32=1

# \
!ifndef 0 # \
# Following section is for nmake \
!ifdef b32 # \
bits=32 # \
!ifdef LIB32 # \
LIB=$(LIB32) # \
!endif # \
!ifdef PATH32 # \
PATH=$(PATH32) # \
!endif  # \
!else # \
bits=64 # \
!endif  # b32 \
!ifndef cmplr # \
cmplr=msvc # \
!endif

# \
!else
# Following section is for GNU make and variants

ifdef b32
	bits=32
	ifdef LIB32
		LIB=$(LIB32)
	endif
	ifdef PATH32
		PATH=$(PATH32)
	endif
else
	bits=64
endif

ifndef cmplr
	cmplr=clang
endif

# \
!endif

########## release section ##########

release: ttLib

ttLib:
	ninja -f build/$(cmplr)Build$(bits).ninja

########## debug section ##########

debug: ttLibD

ttLibD:
	ninja -f build/$(cmplr)Build$(bits)D.ninja

########## clean release section ##########

clean:
	ninja -f build/$(cmplr)Build$(bits).ninja -t clean

########## clean debug section ##########

cleanD:
	ninja -f build/$(cmplr)Build$(bits)D.ninja -t clean
