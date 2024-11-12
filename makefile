CC=gcc
CORE_DEPENDENCIES= \
	./core/debug/debug.c \
	./core/zone/zone.c \
	./core/string/string.c \
	./core/array/array.c \
	./core/hashmap/hashmap.c \
	./core/lexer/lexer.c \
	./core/parser/parser.c \
	./core/c_print/c_print.c \
	./core/c_print/ctype.c \
	./core/ast/ast.c

zone:
	@make zone_test

zone_test:
	$(CC) $(CORE_DEPENDENCIES) ./core/zone/zone_test.c && ./a.out
	@rm -f *.out

string:
	@make string_test

string_test:
	$(CC) $(CORE_DEPENDENCIES) ./core/string/string_test.c && ./a.out
	@rm -f *.out

array:
	@make array_test

array_test:
	$(CC)  $(CORE_DEPENDENCIES) ./core/array/array_test.c && ./a.out
	@rm -f *.out

parser:
	@make parser_test

parser_test:
	$(CC) $(CORE_DEPENDENCIES) ./core/parser/parser_test.c && ./a.out
	@rm -f *.out

lexer:
	@make lexer_test

lexer_test:
	$(CC) $(CORE_DEPENDENCIES) ./core/lexer/lexer_test.c && ./a.out
	@rm -f *.out

hashmap:
	@make hashmap_test

hashmap_test:
	$(CC) $(CORE_DEPENDENCIES) ./core/hashmap/hashmap_test.c && ./a.out
	@rm -f *.out

c_print:
	@make c_print_test

c_print_test:
	@$(CC) $(CORE_DEPENDENCIES) ./core/c_print/c_print_test.c && ./a.out
	@rm -f *.out

#  shell command: fswatch -e ".*" -i "\\.c$" -i "\\.h$" . | xargs -n1 -I {} sh -c 'make "$(basename "{}" | sed "s/\\.[ch]$//")"'
watch:
	@fswatch -e ".*" -i "\\.c$$" -i "\\.h$$" . | xargs -n1 -I {} sh -c 'make "$$(basename "{}" | sed "s/\\.[ch]$$//")"'

vscode:
	@rm -rf ~/.vscode/extensions/deet-vscode
	@rm -rf ~/.vscode-fb/extensions/deet-vscode
	@cp -rp ./deet-vscode ~/.vscode/extensions
	@cp -rp ./deet-vscode ~/.vscode-fb/extensions
	@echo "Installation finished"

test:
	@make zone_test
	@make string_test
	@make array_test
	@make parser_test
	@make lexer_test
	@make hashmap_test
	@make c_print_test
