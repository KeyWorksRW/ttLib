# Call with nmake.exe (MSVC) or a GNU make variation (mingw32-make.exe, gmake.exe, or make.exe)

# mingw32-make release       <-- this will build release target(s)
# mingw32-make clean release <-- this will clean and rebuild release target(s)
# mingw32-make cleanD debug  <-- this will clean and rebuild debug target(s)

# nmake release       <-- this will build release target(s)
# nmake clean release <-- this will clean and rebuild release target(s)
# nmake cleanD debug  <-- this will clean and rebuild debug target(s)

# If the environment variable "b32" is set (or b32=1 is part of the nmake/make command line), then a '32' suffix is added
# to all ninja build script names instead of the normal '64'. In addition, if you have a LIB32 environment variable that
# points to all 32-bit LIB paths, then your LIB environment will be temporarily changed to LIB32 before building.

# If you are using the MSVC compiler, then you can also set a PATH32 environment variable so that you can build 32-bit
# targets even if your current PATH is pointing to the 64-bit compiler/linker

# Remove the '#' character in the following line to always build 32-bit targets

#b32=1

# \
!ifndef 0 # \
######### Following section is for nmake ######### \
#\
!ifdef b32 # \
bits=32 # \
!ifdef LIB32 # \
LIB=$(LIB32) # \
!endif # \
!ifdef PATH32 # \
PATH=$(PATH32) # \
!endif  # \
#\
!else # \
bits=64 # \
!endif  # b32 \
!ifndef cmplr # \
cmplr=msvc # \
!endif #\
#\
bdir=bld # \
BldScript=$(bdir)/$(cmplr)Build$(bits).ninja # \
BldScriptD=$(bdir)/$(cmplr)Build$(bits)D.ninja # \
# \
!else
######### Following section is for GNU make and variants #########

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
BldScript=$(bdir)/$(cmplr)Build$(bits).ninja
BldScriptD=$(bdir)/$(cmplr)Build$(bits)D.ninja

# \
!endif

########## release section ##########

release: ttLib

ttLib:
	ninja -f $(BldScript)

########## debug section ##########

debug: ttLibD

ttLibD:
	ninja -f $(BldScriptD)

########## localization ##########

locale:
	xgettext -k_ -kwxGetTranslation -kwxPLURAL -k_XGET -kTRANSLATE -i -c --omit-header -fxgen.lst

########## clean release section ##########

clean:
	ninja -f $(BldScript) -t clean

########## clean debug section ##########

cleanD:
	ninja -f $(BldScriptD) -t clean

########## special ninja diagnostics ##########

commandsD:
	ninja -f $(BldScriptD) -t commands

commandsR:
	ninja -f $(BldScript) -t commands

targetsD:
	ninja -f $(BldScriptD) -t targets depth 2

targetsR:
	ninja -f $(BldScript) -t targets depth 2

deps:
	ninja -f $(BldScript) -t deps
