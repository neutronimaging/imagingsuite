## Process this file with automake to produce Makefile.in
#====================================================================
#
# $Id$
#
DOC_SRC=NXtranslate.docbook overview.docbook \
		translation.docbook retrievers.docbook

nxtranslatedocdir		= $(NXDOCDIR)/NXtranslate
nxtranslatedoc_DATA		= $(DOC_OUTPUT)
EXTRA_DIST 			= $(DOC_OUTPUT) $(DOC_SRC)

if HAVE_DOCBOOK
DOC_OUTPUT=NXtranslate.pdf NXtranslate.txt
NXtranslate.pdf: $(DOC_SRC)
	$(DOCBOOK2PDF) NXtranslate.docbook

NXtranslate.txt: $(DOC_SRC)
	$(DOCBOOK2TXT) NXtranslate.docbook
endif
