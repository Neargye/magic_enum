#!/usr/bin/env bash
set -euo pipefail

phase="$1"
shift
diagnostics="$RUNNER_TEMP/magic-enum-diagnostics"
timing="$diagnostics/$phase-time.txt"
if ! mkdir -p "$diagnostics" || ! : > "$timing"; then
  echo "Resource diagnostics unavailable, running without them." >&2
  exec "$@"
fi

snapshot() {
  date -u
  uptime
  sysctl hw.model hw.physicalcpu hw.logicalcpu hw.memsize vm.swapusage
  vm_stat
}
snapshot > "$diagnostics/$phase-before.txt" 2>&1 || true
vm_stat 10 > "$diagnostics/$phase-memory.txt" 2>&1 &
monitor_pid=$!
cleanup() {
  kill "$monitor_pid" 2>/dev/null || true
  wait "$monitor_pid" 2>/dev/null || true
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

if /usr/bin/time -lp -o "$timing" "$@"; then
  result=0
else
  result=$?
fi
cleanup
trap - EXIT
snapshot > "$diagnostics/$phase-after.txt" 2>&1 || true
cat "$timing" || true

awk -v phase="$phase" -v result="$result" '
  $1 == "real" { wall = $2 }
  $1 == "user" { user = $2 }
  $1 == "sys" { system_time = $2 }
  /maximum resident set size/ { rss = $1 / 1048576 }
  END {
    printf "### %s diagnostics\n\n", phase
    print "| Wall (s) | User (s) | System (s) | Max RSS (MiB) | Exit code |"
    print "| --- | --- | --- | --- | --- |"
    printf "| %s | %s | %s | %.1f | %d |\n\n", wall, user, system_time, rss, result
  }
' "$timing" >> "$GITHUB_STEP_SUMMARY" || echo "Cannot write resource summary." >&2
exit "$result"
