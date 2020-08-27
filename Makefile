##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.6.0] date: [Fri Jan 24 14:15:57 GMT 2020] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = 


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = 

#######################################
# paths
#######################################
# doc/doxygen path
DOC_DIR = doc
DOXY_WARN_LOGFILE = doxy_warn_log_file.txt

######################################
# source
######################################
# C sources

# ASM sources
ASM_SOURCES =


#######################################
# binaries
#######################################
PREFIX =
 
#######################################
# CFLAGS
#######################################

# mcu
MCU = 

# macros for gcc

# C defines
C_DEFS = 

# C includes
C_INCLUDES =  \
-I.

# compile gcc flags

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################

# generate doc (doxygen framework), configure some dynamic parameters from Makefile
doc: Doxyfile
	(cat $< ; \
echo $(CFLAGS) | xargs -n1 echo | grep "^-D" | sed 's/-D//' | xargs echo PREDEFINED= ; \
echo OUTPUT_DIRECTORY=$(DOC_DIR) ; \
echo EXCLUDE=$(DOC_DIR) ; \
echo INCLUDE_PATH=$(subst -I,,$(C_INCLUDES)) ; \
echo WARN_LOGFILE=$(DOXY_WARN_LOGFILE)) \
| doxygen -
	(git log -n 1 > $(DOC_DIR)/doc_version.txt)
	
#######################################
# clean up
#######################################
doc_clean:
	-rm -fR $(DOC_DIR)
	-rm -f  $(DOXY_WARN_LOGFILE)

#######################################
# dependencies
#######################################

.PHONY: doc doc_clean

# *** EOF ***