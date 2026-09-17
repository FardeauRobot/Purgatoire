NAME     = main
CXX      = c++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

PY       = python3
SIM      = simulator/sim.py
OUT      = simulator/out
PORT     = 8765

SEED    ?= 1
N       ?= 20
LEAGUE  ?= 3
OPP     ?=

OPP_FLAG = $(if $(OPP),--opponent "$(OPP)")

all: $(NAME)

$(NAME): main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test: test-l1 test-l2 test-l3

test-l1 test-l2 test-l3: test-l%: $(NAME)
	@mkdir -p $(OUT)
	@$(PY) $(SIM) --league $* --bot ./$(NAME) --seed $(SEED) --out $(OUT)/replay-l$*.json $(OPP_FLAG)

strict: $(NAME)
	@mkdir -p $(OUT)
	@for l in 1 2 3; do \
		$(PY) $(SIM) --league $$l --bot ./$(NAME) --seed $(SEED) --strict-timing --out $(OUT)/replay-l$$l.json $(OPP_FLAG); \
	done

selfplay: $(NAME)
	@mkdir -p $(OUT)
	@$(PY) $(SIM) --league 3 --bot ./$(NAME) --opponent ./$(NAME) --seed $(SEED) --out $(OUT)/replay-self.json

series: $(NAME)
	@$(PY) $(SIM) --league $(LEAGUE) --bot ./$(NAME) --games $(N) --seed $(SEED) $(OPP_FLAG)

view:
	@echo "Viewer: http://localhost:$(PORT)/viewer/"
	@$(PY) -m http.server $(PORT)

clean:
	rm -f $(NAME) $(OUT)/*.json

help:
	@echo "make              build ./main"
	@echo "make test         run leagues 1-3 once (SEED=1), write replays"
	@echo "make test-lN      one league (N = 1, 2, 3)"
	@echo "make strict       same, but a slow turn is a timeout loss"
	@echo "make selfplay     league 3, ./main vs ./main"
	@echo "make series       N games from SEED, win rate  (LEAGUE=3 N=20 OPP=...)"
	@echo "make view         serve the replay viewer on port $(PORT)"
	@echo "make clean        remove ./main and replays"
	@echo "Overrides: SEED=7  OPP='./old_bot'  LEAGUE=2  N=50"

.PHONY: all test test-l1 test-l2 test-l3 strict selfplay series view clean help
