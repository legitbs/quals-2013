.PHONY: views install submodule

all: install

run: install
	./badmedicine

install: badmedicine views key iv

badmedicine:
	@GOPATH=`pwd` go install

views: views/index.html views/success.html

views/%.html: views/%.haml
	haml $< $@

key:
	dd if=/dev/random of=key bs=32 count=1

iv:
	dd if=/dev/random of=iv bs=16 count=1

clean:
	rm -rf pkg
	rm -f badmedicine
