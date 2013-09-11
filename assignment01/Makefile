# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment00:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment01.pro; make

clean:
	rm -fr temp bin/assignment01

.PHONY:bin/assignment01 clean

