#!/usr/bin/env bash
# =============================================================================
#  leakcheck.sh — cross-platform memory-leak runner for C++ binaries
# -----------------------------------------------------------------------------
#  Auto-detects the OS and dispatches to the right tool:
#     macOS  -> leaks   (built into the system, no install needed)
#     Linux  -> valgrind (install via your distro package manager)
#
#  Three modes, increasing in noise and pedagogical value:
#     -b / --basic    quick "is there a leak yes/no" with backtraces
#     -v / --verbose  + tracks origins of uninitialised reads, scribbles
#                       freed memory so use-after-free crashes loudly
#     -s / --strict   + flags every reachable byte, even ones the OS would
#                       reclaim at exit anyway. Useful to learn what counts
#                       as a "real" leak vs. "the runtime cleans this up".
#
#  Usage:
#     ./leakcheck.sh [MODE] <binary> [args...]
#     ./leakcheck.sh ./zombie
#     ./leakcheck.sh -v ./zombie arg1 arg2
#     ./leakcheck.sh --strict ./zombie
# =============================================================================

set -eu

# ---- colours (only if stdout is a TTY) --------------------------------------
if [ -t 1 ]; then
    RED=$'\033[1;31m'
    GREEN=$'\033[1;32m'
    YELLOW=$'\033[1;33m'
    CYAN=$'\033[1;36m'
    DIM=$'\033[2m'
    RESET=$'\033[0m'
else
    RED=""; GREEN=""; YELLOW=""; CYAN=""; DIM=""; RESET=""
fi

usage() {
    cat <<EOF
${CYAN}leakcheck.sh${RESET} — cross-platform memory-leak runner

${YELLOW}USAGE${RESET}
  $0 [MODE] <binary> [args...]

${YELLOW}MODES${RESET}
  -b, --basic     basic check (default): leaks/valgrind summary + backtraces
  -v, --verbose   adds origin tracking + scribble (catches use-after-free)
  -s, --strict    everything, including 'still reachable' bytes
  -h, --help      show this help

${YELLOW}OS DISPATCH${RESET}
  macOS  -> ${GREEN}leaks --atExit${RESET}   (env: MallocStackLogging, MallocScribble, ...)
  Linux  -> ${GREEN}valgrind${RESET}         (--leak-check=full, --track-origins, ...)

${YELLOW}EXAMPLES${RESET}
  $0 ./zombie
  $0 -v ./zombie
  $0 --strict ./PhoneBook
EOF
}

# ---- arg parsing ------------------------------------------------------------
MODE="basic"
case "${1:-}" in
    -h|--help)    usage; exit 0 ;;
    -b|--basic)   MODE="basic";   shift ;;
    -v|--verbose) MODE="verbose"; shift ;;
    -s|--strict)  MODE="strict";  shift ;;
    -*)           echo "${RED}error:${RESET} unknown flag '$1'" >&2; usage; exit 2 ;;
esac

BIN="${1:-}"
[ -n "$BIN" ] || { echo "${RED}error:${RESET} no binary specified" >&2; usage; exit 2; }
shift || true

if [ ! -x "$BIN" ]; then
    echo "${RED}error:${RESET} '$BIN' is not executable (build it first?)" >&2
    exit 2
fi

OS="$(uname -s)"

# ---- header ----------------------------------------------------------------
echo "${CYAN}┌─ leakcheck ────────────────────────────────────────${RESET}"
echo "${CYAN}│${RESET} os     : ${YELLOW}${OS}${RESET}"
echo "${CYAN}│${RESET} mode   : ${YELLOW}${MODE}${RESET}"
echo "${CYAN}│${RESET} binary : ${YELLOW}${BIN}${RESET} ${DIM}$*${RESET}"
echo "${CYAN}└────────────────────────────────────────────────────${RESET}"
echo

# ---- dispatch --------------------------------------------------------------
case "$OS" in
  Darwin)
    if ! command -v leaks >/dev/null 2>&1; then
        echo "${RED}error:${RESET} 'leaks' not found." >&2
        echo "       install Xcode Command Line Tools: ${GREEN}xcode-select --install${RESET}" >&2
        exit 127
    fi
    case "$MODE" in
      basic)
        # MallocStackLogging   : record allocation backtraces so 'leaks' can
        #                        point at the exact line that allocated the
        #                        leaked block (otherwise you only get an addr).
        exec env MallocStackLogging=1 \
            leaks --atExit -- "$BIN" "$@"
        ;;
      verbose)
        # MallocScribble       : fill freed memory with 0x55. A use-after-free
        #                        then reads recognisable garbage instead of
        #                        whatever happened to be there.
        # MallocPreScribble    : fill fresh allocations with 0xAA. Surfaces
        #                        code that assumes new memory is zeroed.
        # MallocGuardEdges     : protected pages around large allocations,
        #                        so over/under-runs trap immediately.
        exec env MallocStackLogging=1 \
                 MallocScribble=1 \
                 MallocPreScribble=1 \
                 MallocGuardEdges=1 \
            leaks --atExit -- "$BIN" "$@"
        ;;
      strict)
        # MallocStackLoggingNoCompact : also keeps stacks of FREED blocks,
        #                               so leaks can describe a use-after-free
        #                               in terms of "this block was freed here".
        # MallocCheckHeapStart=1
        # MallocCheckHeapEach=100     : run a heap-consistency check every
        #                               100 mallocs. Catches corruption early.
        exec env MallocStackLogging=1 \
                 MallocStackLoggingNoCompact=1 \
                 MallocScribble=1 \
                 MallocPreScribble=1 \
                 MallocGuardEdges=1 \
                 MallocCheckHeapStart=1 \
                 MallocCheckHeapEach=100 \
            leaks --atExit --groupByType --list -- "$BIN" "$@"
        ;;
    esac
    ;;

  Linux)
    if ! command -v valgrind >/dev/null 2>&1; then
        echo "${RED}error:${RESET} 'valgrind' not found." >&2
        echo "       Debian/Ubuntu: ${GREEN}sudo apt install valgrind${RESET}" >&2
        echo "       Fedora/RHEL  : ${GREEN}sudo dnf install valgrind${RESET}" >&2
        exit 127
    fi
    case "$MODE" in
      basic)
        # --leak-check=full    : show each leak individually with the backtrace
        # --error-exitcode=1   : non-zero exit so 'make' / CI knows it failed
        exec valgrind --leak-check=full \
                      --error-exitcode=1 \
                      "$BIN" "$@"
        ;;
      verbose)
        # --track-origins=yes  : when an uninitialised value is read, trace
        #                        back to the allocation that produced it.
        #                        (Slower, but the diagnostic is invaluable.)
        # --show-leak-kinds=definite,indirect,possible
        #                      : skip 'still reachable' (cleaned up at exit).
        exec valgrind --leak-check=full \
                      --show-leak-kinds=definite,indirect,possible \
                      --track-origins=yes \
                      --error-exitcode=1 \
                      "$BIN" "$@"
        ;;
      strict)
        # --show-leak-kinds=all      : also report 'still reachable'
        # --errors-for-leak-kinds=all: any leak counts as an error for the
        #                              exit code (great for "no leaks ever" CI)
        # -s                         : print error summary at the end
        exec valgrind --leak-check=full \
                      --show-leak-kinds=all \
                      --track-origins=yes \
                      --errors-for-leak-kinds=all \
                      --error-exitcode=1 \
                      -s \
                      "$BIN" "$@"
        ;;
    esac
    ;;

  *)
    echo "${RED}error:${RESET} unsupported OS: $OS" >&2
    exit 1
    ;;
esac
