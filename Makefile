.PHONY: all clean os hv sm test os-clean hv-clean sm-clean test-clean

# todo: OS コードを更新したときに fs.img が再作成されない
all: sm

sm: hv
	$(MAKE) -C sm

hv: os
	$(MAKE) -C hv

os:
	$(MAKE) -C os

test:
	$(MAKE) -C tests test

clean: os-clean hv-clean sm-clean test-clean

os-clean:
	$(MAKE) -C os clean

hv-clean:
	$(MAKE) -C hv clean

sm-clean:
	$(MAKE) -C sm clean

test-clean:
	$(MAKE) -C tests clean
