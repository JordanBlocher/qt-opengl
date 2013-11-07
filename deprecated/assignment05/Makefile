# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment05:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment05.pro; make

clean:
	rm -fr temp bin/assignment05

.PHONY:bin/assignment05 clean

