ssh:
	ssh linadm@192.168.100.1	

fusessh:
	sshfs linadm@192.168.100.1:/home/linadm/Codes/mobrob ~/fusessh

unfuse:
	fusermount -u ~/fusessh 

copy:
	cp -r ~/github/mobrob/CC/src ~/github/mobrob/CC/include ~/fusessh/CC