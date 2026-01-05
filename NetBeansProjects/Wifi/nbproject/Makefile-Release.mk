#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=arm-oe-linux-gnueabi-gcc
CCC=arm-oe-linux-gnueabi-g++
CXX=arm-oe-linux-gnueabi-g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=OE-Gnueabi_1-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include NbMakefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/appmain.o


# C Compiler Flags
CFLAGS="-I${SDKV1INC}" -fsigned-char -Wundef -Wstrict-prototypes -Wno-trigraphs -Wimplicit -Wformat 

# CC Compiler Flags
CCFLAGS="-I${SDKV1INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 
CXXFLAGS="-I${SDKV1INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread -ldl -lctosapi -lz -lfreetype -lcaemvl2 -lcaemvl2ap -lcaclmdl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/V1/App/Wifi.exe

dist/V1/App/Wifi.exe: ${OBJECTFILES}
	${MKDIR} -p dist/V1/App
	arm-oe-linux-gnueabi-g++ -o dist/V1/App/Wifi ${OBJECTFILES} ${LDLIBSOPTIONS} -L . "-L${SDKV1LIB}" "-L${SDKV1LIBN}"

${OBJECTDIR}/appmain.o: appmain.c
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA1000/include -o ${OBJECTDIR}/appmain.o appmain.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
