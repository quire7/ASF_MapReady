CFLAGS += $(HDF5_CFLAGS)
CFLAGS += $(GEOTIFF_CFLAGS)
CFLAGS += $(HDF5_CFLAGS)
# Makefile for		: elev 
# Module Author		: Mike Shindle

include ../../make_support/system_rules

LIBS  = \
	$(LIBDIR)/libasf_insar.a \
	$(LIBDIR)/asf_meta.a \
	$(GSL_LIBS) \
	$(LIBDIR)/libasf_proj.a \
	$(LIBDIR)/asf.a \
	$(PROJ_LIBS) \
	$(XML_LIBS) \
	-lm
OBJS  = elev.o

all: elev clean

elev: $(OBJS)
	$(CC) $(CFLAGS) -o elev $(OBJS) $(LIBS) $(LDFLAGS)
	mv elev$(BIN_POSTFIX) $(BINDIR)
	cp elev.1 ../../man/man1

clean:
	rm *.o
