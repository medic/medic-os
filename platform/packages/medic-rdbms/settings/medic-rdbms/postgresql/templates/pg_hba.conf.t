# pg_hba.conf

# TYPE  DATABASE        USER            ADDRESS                 METHOD
local   all             +postgresql                             trust
local   all             +rdbms-users                            trust
host    all             +postgresql     127.0.0.1/32            trust
host    all             +postgresql     ::1/128                 trust

