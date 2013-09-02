# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
lib/common.so:
	@mkdir -p temp
	@cd temp; qmake ../build/common.pro; make

clean:
	rm -fr temp lib/*

.PHONY:bin/common clean

