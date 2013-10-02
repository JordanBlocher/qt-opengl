# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment06:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment06.pro; make

clean:
	rm -fr temp bin/assignment06

.PHONY:bin/assignment06 clean

