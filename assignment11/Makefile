# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment11:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment11.pro; make

clean:
	rm -fr temp bin/assignment11

.PHONY:bin/assignment11 clean

