# pg_hba.conf

# TYPE  DATABASE        USER            ADDRESS                 METHOD
local   all             +postgresql                             trust
local   all             +rdbms-users                            trust
local   all             gardener                                trust
host    all             +postgresql     127.0.0.1/32            md5
host    all             +rdbms-users    127.0.0.1/32            md5
host    all             +postgresql     ::1/128                 md5
host    all             +rdbms-users    ::1/128                 md5

