# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment07:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment07.pro; make

clean:
	rm -fr temp bin/assignment07

.PHONY:bin/assignment07 clean

