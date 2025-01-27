
JAVA_COMPILER = javac
JAVA_RUNTIME = java
MAIN_CLASS = java_server.Server

all: compile run

compile:
	$(JAVA_COMPILER) -d . *.java

run:
	$(JAVA_RUNTIME) $(MAIN_CLASS)

clean:
	rm -rf *.class
