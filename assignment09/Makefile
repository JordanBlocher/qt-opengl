# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/assignment09:
	@mkdir -p temp
	@cd temp; qmake ../build/assignment09.pro; make

clean:
	rm -fr temp bin/assignment09

.PHONY:bin/assignment09 clean

