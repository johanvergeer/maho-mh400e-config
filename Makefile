PYTHON=python3
VENV_DIR=.venv
PIP=$(VENV_DIR)/bin/pip
PYTEST=$(VENV_DIR)/bin/pytest

CC = gcc
CFLAGS = -fPIC -Wall -Werror
LDFLAGS = -shared

LOGIC_SRC = components/src/calculate/calculate_logic.c
LOGIC_SO = components/src/calculate/calculate.so

COMPONENTS = components/src/calculate/calculate.comp components/src/spindle/mh400e_spindle.comp

# Check en bouw logic.so
$(LOGIC_SO): $(LOGIC_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Default help command
.PHONY: help
help: ## Toon dit help-overzicht
	@echo ""
	@echo "📦  Beschikbare commando's:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'
	@echo ""

.PHONY: venv
venv: ## Maak virtuele omgeving en installeer pytest als die nog niet bestaat
	@if [ ! -d $(VENV_DIR) ]; then \
		$(PYTHON) -m venv $(VENV_DIR); \
	fi
	@$(PIP) install --upgrade pip > /dev/null
	@$(PIP) show pytest > /dev/null 2>&1 || $(PIP) install pytest

.PHONY: test
test: venv $(LOGIC_SO) ## Compileer logica en draai tests via pytest in de venv
	$(PYTEST) components/tests

.PHONY: install
install: ## Compileer en installeer alle .comp bestanden met comp --install
	@for comp in $(COMPONENTS); do \
		echo "Compiling $$comp..."; \
		sudo halcompile --install $$comp; \
	done

.PHONY: clean
clean: ## Verwijder build artifacts (logic.so)
	rm -f $(LOGIC_SO)
