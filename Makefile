default: all

.PHONY: testapp
testapp:
	@$(MAKE) --no-print-directory -C engine/ dev
	@$(MAKE) --no-print-directory -C testapp/
	./testapp/build/app.bin

.PHONY: all
all:
	@$(MAKE) --no-print-directory -C engine/
	@$(MAKE) --no-print-directory -C testapp/

.PHONY: clean
clean:
	-rm -r engine/build/
	-rm -r testapp/build/

