###########################################################
#  MAC0219 - Computação Paralela e Distribuída - 2018/01  #
#  Eugenio Augusto Jimenes                                #
#  Nº USP 7118981                                         #
#  MiniEP1 - Utilização das Memórias Caches               #
###########################################################

all: run
run: compile
	./teste 10000 1
	./teste 10000 2
compile: miniep1.c
	gcc miniep1.c -o teste
clean:
	rm teste
