OUT=REFRESH
OBJECTS=refresh.rel menuentry.rel 


include ../lib/kc85_app.make

obj/menuentry.rel:	src/menuentry.s
	sdasz80 -plosgff  $@ $<
