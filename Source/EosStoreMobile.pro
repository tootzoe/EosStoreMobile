




#TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#
# below one line project name need to be lowercaes
#### Module 1
PRJMODULE1  = EosStoreMobile
DEFINES += "EOSSTOREMOBILE_API=__declspec(dllexport)"
#
INCLUDEPATH += ../Intermediate/Build/Win64/UnrealEditor/Inc/$$PRJMODULE1/UHT
#INCLUDEPATH += ../Intermediate/Build/Mac/UnrealEditor/Inc/$$PRJMODULE1/UHT
INCLUDEPATH += $$PRJMODULE1 $$PRJMODULE1/Public $$PRJMODULE1/Private
#### Module 2
# PRJMODULE2  = StrategyGameLoadingScreen
# DEFINES += "STRATEGYGAMELOADINGSCREEN_API=__declspec(dllexport)"
# #
# INCLUDEPATH += ../Intermediate/Build/Win64/UnrealEditor/Inc/$$PRJMODULE2/UHT
# INCLUDEPATH += $$PRJMODULE2 $$PRJMODULE2/Public $$PRJMODULE2/Private
####


#
# this is true during development with unreal-editor...

DEFINES += "WITH_EDITORONLY_DATA=1"

## this project only
DEFINES += PLATFORM_ANDROID
##

#INCLUDEPATH += ../Plugins/NNEPostProcessing/Source/NNEPostProcessing/Public
# we should follow UE project struct to include files, start from prj.Build.cs folder
#
#  The Thirdparty libs
#
#
#
include(defs.pri)
include(inc.pri)
#
## this project only
## INCLUDEPATH += $$UESRCROOT/Runtime/Renderer/Private

##
#### PLUGINSROOT 1
PLUGINSROOT = ../Plugins
###
###
#### Start Plugins Module 1 Start
# PLUGINNAME1  = GlobalVMResolver
# PLUGINMODULE1  =  MatchoGlobalVMResolver
# DEFINES += "MATCHOGLOBALVMRESOLVER_API=__declspec(dllexport)"
# #
# INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME1/Intermediate/Build/Win64/UnrealEditor/Inc/$$PLUGINMODULE1/UHT
# INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME1/Source/$$PLUGINMODULE1
# INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME1/Source/$$PLUGINMODULE1/Public
# INCLUDEPATH += $$PLUGINSROOT/$$PLUGINNAME1/Source/$$PLUGINMODULE1/Private
########## End Plugins Module 1 End



##
#
#

HEADERS += \
	EosStoreMobile/EosStoreMobile.h \
	EosStoreMobile/StoreEntitlement.h \
	EosStoreMobile/StoreOffer.h \
	EosStoreMobile/StoreOwnedItem.h \
	EosStoreMobile/TootEosPC.h \
	EosStoreMobile/TootLog.h

SOURCES += \
	EosStoreMobile/EosStoreMobile.cpp \
	EosStoreMobile/StoreEntitlement.cpp \
	EosStoreMobile/StoreOffer.cpp \
	EosStoreMobile/StoreOwnedItem.cpp \
	EosStoreMobile/TootEosPC.cpp \
	EosStoreMobile/TootLog.cpp

DISTFILES += \
	EosStoreMobile.Target.cs \
	EosStoreMobile/EosStoreMobile.Build.cs \
	EosStoreMobileEditor.Target.cs
