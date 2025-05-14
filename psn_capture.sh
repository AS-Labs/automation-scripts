#!/bin/bash
sudo ./psn -d 5 > ./psn.out
sudo chown as-labs:as-labs ./psn.out
echo "time,hostname,samples,avg_threads,comm,state" > ./psn_report.csv
cat ./psn.out|grep -Ev "^samples|expected|total|^runtime|Active Threads|^finished|^Sampling|^Linux Process|^---"\
	|sed 's/|/,/g'\
	|sed 's/\s//g'\
	|sed -r '/^\s*$/d'\
	|grep -Ev "^samples,"\
	|sed "s/^/$(hostname),/g"\
	|sed "s/^/$(date +%s),/g" >> ./psn_report.csv


#TODO # Send to clickhouse:

cat ./psn_report.csv |clickhouse-client --queries-file ./insert_data_clickhouse.sql

