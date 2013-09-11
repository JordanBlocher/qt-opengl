# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment02:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment02.pro; make

clean:
	rm -fr temp bin/assignment02

.PHONY:bin/assignment02 clean

