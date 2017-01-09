# postgresql.conf

listen_addresses = 'localhost'
port = 5432
max_connections = 256
superuser_reserved_connections = 3
unix_socket_directories = '/tmp'
bonjour = off
authentication_timeout = 30s
shared_buffers = 128MB
dynamic_shared_memory_type = posix
fsync = on
synchronous_commit = on
wal_sync_method = fdatasync
log_destination = 'stderr'
log_timezone = 'UTC'

autovacuum = on
log_autovacuum_min_duration = 0
autovacuum_max_workers = 3
autovacuum_naptime = 1min
autovacuum_vacuum_threshold = 50
autovacuum_analyze_threshold = 50
autovacuum_vacuum_scale_factor = 0.2
autovacuum_analyze_scale_factor = 0.1
autovacuum_freeze_max_age = 200000000
autovacuum_multixact_freeze_max_age = 400000000
autovacuum_vacuum_cost_delay = 20ms
autovacuum_vacuum_cost_limit = -1

datestyle = 'iso, mdy'
intervalstyle = 'postgres'
timezone = '{{{timezone}}}'

lc_time = '{{{lc_time}}}'
lc_numeric = '{{{lc_numeric}}}'
lc_messages = '{{{lc_messages}}}'
lc_monetary = '{{{lc_monetary}}}'

default_text_search_config = 'pg_catalog.english'

deadlock_timeout = 2s
max_locks_per_transaction = 128

include_dir = 'postgresql.conf.d'

