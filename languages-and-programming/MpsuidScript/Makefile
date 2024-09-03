DBGDIR = _debug

GRAMMAR = Chat
RULE = chat

$(DBGDIR):
	mkdir -p $(DBGDIR)

$(DBGDIR)/$(GRAMMAR)Parser.java: $(GRAMMAR).g4 | $(DBGDIR)
	cp $(GRAMMAR).g4 ./$(DBGDIR)/
	cd $(DBGDIR)/ && java -Xmx500M -cp "/usr/local/lib/antlr-4.9.3-complete.jar:$CLASSPATH" org.antlr.v4.Tool $(GRAMMAR).g4
	cd $(DBGDIR)/ && javac Chat*.java


.PHONY: test_rule
test_rule: $(DBGDIR)/$(GRAMMAR)Parser.java
	cd $(DBGDIR)/ && java -Xmx500M -cp "/usr/local/lib/antlr-4.9.3-complete.jar:$CLASSPATH" org.antlr.v4.gui.TestRig $(GRAMMAR) $(RULE)



.PHONY: clean
clean:
	rm -rf $(DBGDIR) 2> /dev/null
