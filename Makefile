.PHONY: all clean os hv os-clean hv-clean

all: hv

hv: os
	$(MAKE) -C hv

os:
	$(MAKE) -C os

clean: os-clean hv-clean

os-clean:
	$(MAKE) -C os clean

hv-clean:
	$(MAKE) -C hv clean
