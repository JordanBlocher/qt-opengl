# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment08:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment08.pro; make

clean:
	rm -fr temp bin/assignment08

.PHONY:bin/assignment08 clean

