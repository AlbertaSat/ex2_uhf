build:
	cp -f csp_autoconfig.h ../libcsp/include/csp
	cp -f csp_if_uhf.c ../libcsp/src/interfaces
	cp -f csp_if_uhf.h ../libcsp/include/csp/interfaces
	cp -f uhf.h ../libcsp/include/csp/drivers
	cp -r -f uhf ../libcsp/src/drivers
	gcc ../libcsp/src/interfaces/csp_if_uhf.c -o ../libcsp/src/interfaces/csp_if_uhf -I/home/stefan/absat/ex2_uhf/libcsp/include -L/home/stefan/absat/ex2_uhf/libcsp/build -lcsp -lpthread -lrt
