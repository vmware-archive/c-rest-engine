# Copyright (c) VMware Inc.  All rights Reserved.

#
# This Makefile is used for both gobuild and local builds
#

GOBUILD_AUTO_COMPONENTS = 1

GOBUILD_TARGET ?= trident

ifeq ($(OS),Windows_NT)
  #
  #  # Windows...
  # This Makefile is used by gobuild harness
  #   #

  SRCROOT := ..
  MAKEROOT=$(SRCROOT)/support/make
  include $(MAKEROOT)/makedefs.mk

  $(info SRCROOT: [$(SRCROOT)])
  $(info BUILDROOT: [$(BUILDROOT)])
  $(info PUBLISH_DIR: [$(PUBLISH_DIR)])

  all: gobuild-deps-copy
	win_build.cmd SIGN
	$(CP) build.log $(BUILDLOG_DIR)
	$(MKDIR) -p $(PUBLISH_DIR)/win64/Debug
	$(MKDIR) -p $(PUBLISH_DIR)/win64/Release
	$(MKDIR) -p $(PUBLISH_DIR)/win64/include
	$(CP) -frv x64/Debug/*.dll $(PUBLISH_DIR)/win64/Debug
	$(CP) -frv x64/Release/*.dll $(PUBLISH_DIR)/win64/Release
	$(CP) -frv x64/Debug/*.exe $(PUBLISH_DIR)/win64/Debug
	$(CP) -frv x64/Release/*.exe $(PUBLISH_DIR)/win64/Release
	$(CP) -frv include/public/*.h $(PUBLISH_DIR)/win64/include

  #include $(MAKEROOT)/makeimpl.mk

  ifdef GOBUILD_TARGET
    ifndef GOBUILD_SPECINFO_MK
      GOBUILD_SPECINFO_MK = 1
      .PHONY: $(GOBUILD_SPECINFO_MK)
      $(GOBUILD_SPECINFO_MK):

    endif

    .PHONY: gobuild-deps-copy deps

    deps: $(GOBUILD_SPECINFO_MK) gobuild-deps-copy

    gobuild-deps-copy: $(GOBUILD_SPECINFO_MK)
   endif

else

GOBUILD_AUTO_COMPONENTS_HOSTTYPE = linux64

SRCROOT=.
MAKEROOT=$(SRCROOT)/support/make
include $(MAKEROOT)/makedefs.mk

MAKEFILENAME = Makefile.vmware
DIRS = build

scrub : clean
	$(RM) -rf obj gobuild

CLEANMORE = \
    aclocal.m4     \
    autom4te.cache \
    Makefile.in    \
    config.guess   \
    config.sub     \
    configure      \
    depcomp        \
    include/config.h.in* \
    install-sh     \
    ltmain.sh      \
    missing

include $(MAKEROOT)/makeimpl.mk

clean:
	@for i in `find $(CURDIR) -name Makefile.in`; do \
		$(RM) -f $$i; \
	done
endif
