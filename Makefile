# Makefile para aplicacion ADSTR_2019
# Los programas a construir:
SUBDIRS           := src/informe \
                     src/captura \

# Reglas para la construcción de los mismos:
all:
	for subDirectory in $(SUBDIRS); do \
		$(MAKE) -C $$subDirectory all; \
		if test $$? -ne 0; then exit 1; fi; \
	done

doc:
	doxygen Doxyfile

clean:
	for subDirectory in $(SUBDIRS); do \
		$(MAKE) -C $$subDirectory clean; \
	done

