# Makefile for ubiquity-slideshow-ubuntu

SOURCE=.
BUILD=$(SOURCE)/build
SOURCESLIDES=$(SOURCE)/slideshows

all: clean build_os-xug translations

build_init:
	mkdir -p $(BUILD)

build_os-xug: build_init
	cp -rL $(SOURCESLIDES)/os-xug $(BUILD)

translations:
	python generate-local-slides.py os-xug

.PHONY : clean

clean:
	-rm -rf $(BUILD)
