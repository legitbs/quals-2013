.PHONY: views install submodule

all: install

run: install
	./babysfirst

install: babysfirst submodule views

babysfirst:
	@GOPATH=`pwd` go install

views: views/index.html views/success.html

views/%.html: views/%.haml
	haml $< $@

submodule:
	git submodule update --init
