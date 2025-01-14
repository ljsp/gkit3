# GNU Make solution makefile autogenerated by Premake
# Type "make help" for usage help

ifndef config
  config=debug
endif
export config

PROJECTS := tp1 tp2 tp2_ex1 tp2_ex2 tp2_ex3

.PHONY: all clean help $(PROJECTS)

all: $(PROJECTS)

tp1: 
	@echo "==== Building tp1 ($(config)) ===="
	@${MAKE} --no-print-directory -C . -f tp1.make

tp2: 
	@echo "==== Building tp2 ($(config)) ===="
	@${MAKE} --no-print-directory -C . -f tp2.make

tp2_ex1: 
	@echo "==== Building tp2_ex1 ($(config)) ===="
	@${MAKE} --no-print-directory -C . -f tp2_ex1.make

tp2_ex2: 
	@echo "==== Building tp2_ex2 ($(config)) ===="
	@${MAKE} --no-print-directory -C . -f tp2_ex2.make

tp2_ex3: 
	@echo "==== Building tp2_ex3 ($(config)) ===="
	@${MAKE} --no-print-directory -C . -f tp2_ex3.make

clean:
	@${MAKE} --no-print-directory -C . -f tp1.make clean
	@${MAKE} --no-print-directory -C . -f tp2.make clean
	@${MAKE} --no-print-directory -C . -f tp2_ex1.make clean
	@${MAKE} --no-print-directory -C . -f tp2_ex2.make clean
	@${MAKE} --no-print-directory -C . -f tp2_ex3.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "   debug"
	@echo "   release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   tp1"
	@echo "   tp2"
	@echo "   tp2_ex1"
	@echo "   tp2_ex2"
	@echo "   tp2_ex3"
	@echo ""
	@echo "For more information, see http://industriousone.com/premake/quick-start"
