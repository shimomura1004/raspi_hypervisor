.PHONY: all clean os hv sm os-clean hv-clean sm-clean

# todo: OS コードを更新したときに fs.img が再作成されない
all: sm

sm: hv
	$(MAKE) -C sm

hv: os
	$(MAKE) -C hv

os:
	$(MAKE) -C os

clean: os-clean hv-clean sm-clean

os-clean:
	$(MAKE) -C os clean

hv-clean:
	$(MAKE) -C hv clean

sm-clean:
	$(MAKE) -C sm clean
