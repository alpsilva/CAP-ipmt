DIR := ../bin


ipmt: Main.cpp
	mkdir -p $(DIR)
	g++ Main.cpp -O3 -o $(DIR)/ipmt
	@cd ..;  sudo ln -s $$(pwd)/bin/ipmt /usr/local/bin/ipmt; chmod +x $$(pwd)/bin/ipmt
clean:
	rm $(DIR)/ipmt
	sudo rm /usr/local/bin/ipmt