INSERT INTO default.system_metrics
SELECT
    toDateTime(time) AS time,
    hostname,
    samples,
    avg_threads,
    comm,
    state
FROM input('time UInt32, hostname String, samples UInt32, avg_threads Float32, comm String, state String')
FORMAT CSV
