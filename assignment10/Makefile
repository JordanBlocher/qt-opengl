# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment10:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment10.pro; make

clean:
	rm -fr temp bin/assignment10

.PHONY:bin/assignment10 clean

