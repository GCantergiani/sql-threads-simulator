library:=libtest

tmptest:=./tmptest
test:=./test
tmpdebug:=./tmpdebug
debug:=./debug
tmp:=./tmp
src:=./src


objects_test:=$(tmptest)/escribe.o $(tmptest)/opera.o $(tmptest)/reader.o $(tmptest)/valida.o 
objects_debug:=$(tmpdebug)/escribe.o $(tmpdebug)/opera.o $(tmpdebug)/reader.o $(tmpdebug)/valida.o 
objects:=$(tmp)/escribe.o $(tmp)/opera.o $(tmp)/reader.o $(tmp)/valida.o 
sources:=$(src)/escribe.cc $(src)/opera.cc $(src)/reader.cc $(src)/valida.cc

cxxflags:= -g
cxx:=g++

includes:=-I./ -I./include
libs:=-L./ -L./lib

main: $(objects)
	$(cxx) $(includes) $(libs) $(objects) -o minisql minisql.cc -lpthread

$(tmp)/%.o: $(src)/%.cc 
	test -d $(tmp) || mkdir $(tmp)
	$(cxx) $(includes) -c -o $(tmp)/$(*F).o $(src)/$*.cc -lpthread -lpthread

$(tmptest)/%.o: $(src)/%.cc
	test -d $(tmptest) || mkdir $(tmptest)
	$(cxx) $(includes) -c -o $(tmptest)/$(*F).o $(src)/$*.cc $(cxxflags) -lpthread -DTESTING

test:  $(objects_test)
	$(cxx) $(includes) $(libs) $(objects_test) -o minisql minisql.cc $(cxxflags) -lpthread -DTESTING

test-lib:  $(objects)
	$(cxx) $(includes) $(libs) $(objects) -o $(library).so $(cxxflags) -lpthread -shared

$(tmpdebug)/%.o: $(src)/%.cc
	test -d $(tmpdebug) || mkdir $(tmpdebug)
	$(cxx) $(includes) -c -o $(tmpdebug)/$(*F).o $(src)/$*.cc $(cxxflags) -lpthread -DDEBUG

debug:  $(objects_debug)
	$(cxx) $(includes) $(libs) $(objects_debug) -o minisql minisql.cc $(cxxflags) -lpthread -DDEBUG

debug-lib:  $(objects)
	$(cxx) $(includes) $(libs) $(objects) -o $(library).so $(cxxflags) -lpthread -shared


clean:
	rm -rf $(tmp);
	rm -f minisql;
	rm -rf $(tmptest);
	rm -rf $(tmpdebug);