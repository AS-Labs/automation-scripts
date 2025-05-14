CREATE TABLE system_metrics
(
    time DateTime,
    hostname String,
    samples UInt32,
    avg_threads Float32,
    comm String,
    state String
)
ENGINE = ReplacingMergeTree()
ORDER BY (time, hostname, comm)
PARTITION BY toYYYYMM(time)
TTL toDateTime(time) + INTERVAL 7 DAY;
