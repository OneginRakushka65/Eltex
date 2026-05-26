#!/bin/bash

PIPE="/tmp/run/cuckoo.$$"
LOG="cuckoo.log"

cleanup() {
        echo "$(date '+%D %T') Shutdown!" >> "$LOG"
        rm -f $PIPE
        rmdir /tmp/run 2>/dev/null
        exit 0
}

mkdir -p /tmp/run
mkfifo "$PIPE"
echo "Создан канал: $PIPE"
echo "$(date '+%D %T') Startup!" >> "$LOG"

trap 'cleanup' SIGTERM

while true; do
        if read request < "$PIPE"; then
                echo "Получен запрос: $request"

                if [[ "$request" =~ ^[0-9]+$ ]]; then
                        continue
                fi

                if [[ "$request" =~ ^([^[]+)\[([0-9]+)\]:\ how\ much\ time\ do\ I\ have\?$ ]]; then
                        name="${BASH_REMATCH[1]}"
                        pid="${BASH_REMATCH[2]}"

                        N=$(( RANDOM % 9 + 2 ))

                        echo "$(date '+%D %T') ${name}[${pid}] $N" >> "$LOG"
                        echo "$N" > "$PIPE" &
                        echo "Отправлен ответ: $N для ${name}[${pid}]"
                else
                        echo "Неверный формат запроса: $request" >> "$LOG"
                fi
        fi
done