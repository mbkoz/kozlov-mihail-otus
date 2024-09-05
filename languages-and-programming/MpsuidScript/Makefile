DBGDIR := _debug
JSDIR := _js

GRAMMAR := Chat
RULE := chat

$(DBGDIR):
	mkdir -p $(DBGDIR)

$(JSDIR):
	mkdir -p $(JSDIR)

$(DBGDIR)/$(GRAMMAR)Parser.java: $(GRAMMAR).g4 | $(DBGDIR)
	cp $(GRAMMAR).g4 ./$(DBGDIR)/
	cd $(DBGDIR)/ && java -Xmx500M -cp "/usr/local/lib/antlr-4.9.3-complete.jar:$$CLASSPATH" org.antlr.v4.Tool $(GRAMMAR).g4
	cd $(DBGDIR)/ && javac Chat*.java


$(JSDIR)/$(GRAMMAR)Parser.js: $(GRAMMAR).g4 *.js | $(JSDIR)
	cp $(GRAMMAR).g4 ./$(JSDIR)/
	cp *.js ./$(JSDIR)/
	cp package.json ./$(JSDIR)/
	cd $(JSDIR)/ && npm install antlr4
	cd $(JSDIR)/ && java -Xmx500M -cp "/usr/local/lib/antlr-4.9.3-complete.jar:$$CLASSPATH" org.antlr.v4.Tool -Dlanguage=JavaScript $(GRAMMAR).g4


.PHONY: test_rule
test_rule: $(DBGDIR)/$(GRAMMAR)Parser.java
	cd $(DBGDIR)/ && java -Xmx500M -cp "/usr/local/lib/antlr-4.9.3-complete.jar:$$CLASSPATH" org.antlr.v4.gui.TestRig $(GRAMMAR) $(RULE) -gui -tokens



.PHONY: clean
clean:
	rm -rf $(DBGDIR) 2> /dev/null
	rm -rf $(JSDIR) 2> /dev/null
	rm -rf .antlr 2> /dev/null

