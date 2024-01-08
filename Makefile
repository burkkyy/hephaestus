default: all

.PHONY: all clean
all:
	@$(MAKE) --no-print-directory -C engine/
	@$(MAKE) --no-print-directory -C testapp/

clean:
	-rm -r engine/build/
	-rm -r testapp/build/

