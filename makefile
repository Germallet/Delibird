all: clean default

clean:
	$(MAKE) clean -C ./Utils/.
	$(MAKE) clean -C ./Broker/.
	$(MAKE) clean -C ./GameBoy/.
	$(MAKE) clean -C ./GameCard/.
	$(MAKE) clean -C ./Team/.

default:
	$(MAKE) -C ./Utils/.
	$(MAKE) -C ./Broker/.
	$(MAKE) -C ./GameBoy/.
	$(MAKE) -C ./GameCard/.
	$(MAKE) -C ./Team/.
