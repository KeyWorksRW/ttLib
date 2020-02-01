# Call with nmake.exe (MSVC) or a GNU make variation (mingw32-make.exe, gmake.exe, or make.exe)

# nmake               <-- this will build release target(s)
# nmake debug         <-- this will build debug target(s)
# nmake clean release <-- this will rebuild release target(s)
# nmake cleanD debug  <-- this will rebuild debug target(s)

# mingw32-make               <-- this will build release target(s)
# mingw32-make debug         <-- this will build release target(s)
# mingw32-make clean release <-- this will rebuild release target(s)
# mingw32-make cleanD debug  <-- this will rebuild debug target(s)

# \
!ifndef 0 # \
######### Following section is for nmake ######### \
#\
!ifndef cmplr # \
cmplr=msvc # \
!endif #\
#\
bdir=bld # \
BldScript=$(bdir)/$(cmplr)_rel.ninja # \
BldScriptD=$(bdir)/$(cmplr)_dbg.ninja # \
# \
!else
######### Following section is for GNU make and variants #########

ifndef cmplr
    cmplr=clang
endif

bdir=bld
BldScript=$(bdir)/$(cmplr)_rel.ninja
BldScriptD=$(bdir)/$(cmplr)_dbg.ninja

# \
!endif

########## release section ##########

release: $(BldScript) ttwx

ttwx:
	ninja -f $(BldScript)

########## debug section ##########

debug: $(BldScriptD) ttwxD

ttwxD:
	ninja -f $(BldScriptD)

########## localization ##########

locale:
	xgettext -k_ -kwxPLURAL -kwxTRANSLATE -k_XGET -i -c --omit-header -fxgen.lst

########## clean release section ##########

clean: $(BldScript)
	ninja -f $(BldScript) -t clean

########## clean debug section ##########

cleanD: $(BldScriptD)
	ninja -f $(BldScriptD) -t clean

########## special ninja diagnostics ##########

commandsD: $(BldScriptD)
	ninja -f $(BldScriptD) -t commands

commandsR: $(BldScript)
	ninja -f $(BldScript) -t commands

targetsD: $(BldScriptD)
	ninja -f $(BldScriptD) -t targets depth 2

targetsR: $(BldScript)
	ninja -f $(BldScript) -t targets depth 2

deps: $(BldScript)
	ninja -f $(BldScript) -t deps

########## Creates or updates ninja file any time .srcfiles.yaml changes ##########

$(BldScript): .srcfiles.yaml
	ttBld.exe -u$(cmplr)

$(BldScriptD): .srcfiles.yaml
	ttBld.exe -u$(cmplr)D
