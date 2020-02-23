# Call with nmake.exe (MSVC) or a GNU make variation (mingw32-make.exe, gmake.exe, or make.exe)

# mingw32-make release 		 <-- this will build release target(s)
# mingw32-make clean release <-- this will clean and rebuild release target(s)
# mingw32-make cleanD debug  <-- this will clean and rebuild debug target(s)

# nmake release 	  <-- this will build release target(s)
# nmake clean release <-- this will clean and rebuild release target(s)
# nmake cleanD debug  <-- this will clean and rebuild debug target(s)

# If the environment variable "b32" is set (or b32=1 is part of the make command line), then a '32' suffix is added to
# all ninja build script names instead of the normal '64'. In addition, if you add a LIB32 environment variable that
# points to all 32-bit LIB paths, then your LIB environment will be temporarily changed to LIB32 before building.

# If you are using the MSVC compiler, then you should also set a PATH32 environment variable ahead of time to point to
# the 32-bit version of the compiler and linker.

# Remove the '#' character in the following line to always build 32-bit targets

#b32=1

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
#\
bdir=bld # \
BldScript=$(bdir)/$(cmplr)_rel.ninja # \
BldScriptD=$(bdir)/$(cmplr)_dbg.ninja # \
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

bdir=bld
BldScript=$(bdir)/$(cmplr)_rel.ninja
BldScriptD=$(bdir)/$(cmplr)_dbg.ninja

# \
!endif

########## release section ##########

release: ttLib ttLibwin

ttLib:
	cd src & ttBld -u$(cmplr) & ninja -f $(BldScript)

ttLibwin:
	cd winsrc & ttBld -u$(cmplr) & ninja -f $(BldScript)

########## debug section ##########

debug: ttLibD ttLibwinD

ttLibD:
	cd src & ttBld -u$(cmplr)D & ninja -f $(BldScriptD)

ttLibwinD:
	cd winsrc & ttBld -u$(cmplr)D & ninja -f $(BldScriptD)

########## clean release section ##########

clean:
	cd src & ninja -f $(BldScript) -t clean & cd ../winsrc & ninja -f $(BldScript) -t clean

########## clean debug section ##########

cleanD:
	cd src & ninja -f $(BldScriptD) -t clean & cd ../winsrc & ninja -f $(BldScriptD) -t clean
