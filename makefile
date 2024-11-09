CC=gcc
CORE_DEPENDENCIES= ./core/debug/debug.c ./core/zone/zone.c ./core/string/string.c ./core/array/array.c ./core/hashmap/hashmap.c ./core/lexer/lexer.c ./core/parser/parser.c

test_zone:
	$(CC) $(CORE_DEPENDENCIES) ./core/zone/zone_test.c && ./a.out
	@rm -f *.out

test_string:
	$(CC) $(CORE_DEPENDENCIES) ./core/string/string_test.c && ./a.out
	@rm -f *.out

test_array:
	$(CC)  $(CORE_DEPENDENCIES) ./core/array/array_test.c && ./a.out
	@rm -f *.out

test_parser:
	$(CC) $(CORE_DEPENDENCIES) ./core/parser/parser_test.c && ./a.out
	@rm -f *.out

test_lexer:
	$(CC) $(CORE_DEPENDENCIES) ./core/lexer/lexer_test.c && ./a.out
	@rm -f *.out

test_hashmap:
	$(CC) $(CORE_DEPENDENCIES) ./core/hashmap/hashmap_test.c && ./a.out
	@rm -f *.out

c_print:
	$(CC) $(CORE_DEPENDENCIES) ./core/ast/ast.c ./core/c_print/c_print.c && ./a.out
	@rm -f *.out

vscode:
	@rm -rf ~/.vscode/extensions/deet-vscode
	@rm -rf ~/.vscode-fb/extensions/deet-vscode
	@cp -rp ./deet-vscode ~/.vscode/extensions
	@cp -rp ./deet-vscode ~/.vscode-fb/extensions
	@echo "Installation finished"

test:
	@make test_zone
	@make test_string
	@make test_array
	@make test_parser
	@make test_lexer
	@make test_hashmap
