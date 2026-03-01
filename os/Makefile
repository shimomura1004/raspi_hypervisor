SUBDIRS = ./echo ./raspios ./vmm

all: $(SUBDIRS)
clean: $(SUBDIRS)

$(SUBDIRS): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

FORCE:
