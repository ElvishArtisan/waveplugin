## automake.am
##
## (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.2 2009/08/10 00:57:37 cvs Exp $
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##
## Use automake to process this into a Makefile.in

asound_module_pcm_wave_LTLIBRARIES = libasound_module_pcm_wave.la

asound_module_pcm_wavedir = @prefix@/@WAVE_LIB_PATH@/alsa-lib

AM_CFLAGS = -Wall -g
AM_LDFLAGS = -module -avoid-version -export-dynamic

libasound_module_pcm_wave_la_SOURCES = waveplugin.c
libasound_module_pcm_wave_la_LIBADD = -lasound -lsndfile

rpm:	dist
	cp waveplugin-$(VERSION).tar.gz $(RPM_ROOT)/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux waveplugin.spec
	mv $(RPM_ROOT)/RPMS/$(ARCH)/waveplugin-$(VERSION)-@RPM_RELEASE@.$(ARCH).rpm $(top_srcdir)/
	mv $(RPM_ROOT)/SRPMS/waveplugin-$(VERSION)-@RPM_RELEASE@.src.rpm $(top_srcdir)/
	rm $(RPM_ROOT)/SOURCES/waveplugin-$(VERSION).tar.gz
	rm -rf $(RPM_ROOT)/BUILD/waveplugin-$(VERSION)

EXTRA_DIST = waveplugin.spec.in autogen.sh get_target.sh get_distro.sh\
             PACKAGE_VERSION

CLEANFILES = *~ *.tar.gz *.tgz *.mo *.rpm *.zip
DISTCLEANFILES = *.spec ltmain.sh config.guess config.sub
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz\
                       config.guess config.status config.sub install-sh\
                       libtool ltmain.sh missing mkinstalldirs depcomp
