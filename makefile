CC=g++
#编译选项
OPT=-Wall

#源文件目录
SRCDIR=src_bf
#生成可执行文件目录
OBJDIR=build
#数据文件夹
DATDIR=data

#串行程序名，源文件应为${serial}.cpp
serial=gauss
#并行程序名，源文件应为${parellel}.cpp
parellel=gauss_mt
#随机数生成器名，源文件${gen}.cpp
gen=generator

#数据规模(gen x gen的行列式)
gen_size=6
#数据范围(0~max-1的整数)
gen_max=3
#产生的数据文件名
gen_out=input.txt

default: prepare gen serial parellel
	@echo 'Done!'
	@echo -e 'use "make generate" to generate data :)\n'
	@sleep 0.3

prepare:
	@echo -n 'make directories needed......'
	@mkdir -p ${OBJDIR}
	@mkdir -p ${DATDIR}
	@sleep 0.3
	@echo 'OK!'

gen: ${SRCDIR}/${gen}.cpp
	@echo -n 'Compiling '${gen}.cpp......
	@${CC} ${OPT} -o ${OBJDIR}/${gen} ${SRCDIR}/${gen}.cpp
	@sleep 0.3
	@echo 'OK!'

serial: ${SRCDIR}/${serial}.cpp
	@echo -n 'Compiling '${serial}.cpp......
	@${CC} ${OPT} -o ${OBJDIR}/${serial} ${SRCDIR}/${serial}.cpp
	@sleep 0.3
	@echo 'OK!'

parellel: ${SRCDIR}/${parellel}.cpp
	@echo -n 'Compiling '${parellel}.cpp......
	@${CC} ${OPT} -pthread -o ${OBJDIR}/${parellel} ${SRCDIR}/${parellel}.cpp
	@sleep 0.3
	@echo 'OK!'

generate: ${OBJDIR}/${gen}
	@echo -n 'Preparing data......'
	@${OBJDIR}/${gen} ${gen_max} ${gen_size} > ${DATDIR}/${gen_out}
	@echo -e 'OK!\n'
	@sleep 0.3

test:
	@echo -e 'Serial<<<'
	./${OBJDIR}/${serial} ${DATADIR}/${input}
	@echo -e 'Parellel<<<'
	./${OBJDIR}/${parellel} ${DATADIR}/${input}
	@echo -e '---------------------'

clean:
	rm -rf *.out
	rm -rf data
	rm -rf build
