
JAVA_COMPILER = javac
JAVA_RUNTIME = java
MAIN_CLASS = java_server.Server

all: compile run

compile:
	mvn clean package

run:
	java -jar target/java-server-1.0-SNAPSHOT.jar

clean:
	rm -rf *.class
