#!/bin/bash

script_name=$(basename "$0")
if [[ "$script_name" == "template_task.sh" ]]; then
    echo "я бригадир, сам не работаю"
    exit 1
fi

report_file="report_${script_name%.sh}.log"
pid="$$"
timestamp() {
    date "+%Y-%m-%d %H:%M:%S"
}

echo "[$pid] $(timestamp) Скрипт запущен" >> "$report_file"
sleep_time=$(( RANDOM % (1800 - 30 + 1) + 30 ))
sleep "$sleep_time"
echo "[$pid] $(timestamp) Скрипт завершился, работал $sleep_time секунд" >> "$report_file"