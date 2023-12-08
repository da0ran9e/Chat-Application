## Installation:
```
sudo apt update
```
```
sudo apt install postgresql-16
```
```
sudo apt-get install postgresql-client
```
## Start
To verify that the PostgreSQL server is running, run the following command:
```
sudo systemctl start postgresql.service
```
To switch to the postgres account on your server, execute the following command:
```
sudo -i -u postgres
```
See `psql --help` options:
```
psql is the PostgreSQL interactive terminal.

Usage:
  psql [OPTION]... [DBNAME [USERNAME]]

General options:
  -c, --command=COMMAND    run only single command (SQL or internal) and exit
  -d, --dbname=DBNAME      database name to connect to (default: "postgres")
  -f, --file=FILENAME      execute commands from file, then exit
  -l, --list               list available databases, then exit
  -v, --set=, --variable=NAME=VALUE
                           set psql variable NAME to VALUE
                           (e.g., -v ON_ERROR_STOP=1)
  -V, --version            output version information, then exit
  -X, --no-psqlrc          do not read startup file (~/.psqlrc)
  -1 ("one"), --single-transaction
                           execute as a single transaction (if non-interactive)
  -?, --help[=options]     show this help, then exit
      --help=commands      list backslash commands, then exit
      --help=variables     list special variables, then exit

Input and output options:
  -a, --echo-all           echo all input from script
  -b, --echo-errors        echo failed commands
  -e, --echo-queries       echo commands sent to server
  -E, --echo-hidden        display queries that internal commands generate
  -L, --log-file=FILENAME  send session log to file
  -n, --no-readline        disable enhanced command line editing (readline)
  -o, --output=FILENAME    send query results to file (or |pipe)
  -q, --quiet              run quietly (no messages, only query output)
  -s, --single-step        single-step mode (confirm each query)
  -S, --single-line        single-line mode (end of line terminates SQL command)

Output format options:
  -A, --no-align           unaligned table output mode
      --csv                CSV (Comma-Separated Values) table output mode
  -F, --field-separator=STRING
                           field separator for unaligned output (default: "|")
  -H, --html               HTML table output mode
  -P, --pset=VAR[=ARG]     set printing option VAR to ARG (see \pset command)
  -R, --record-separator=STRING
                           record separator for unaligned output (default: newline)
  -t, --tuples-only        print rows only
  -T, --table-attr=TEXT    set HTML table tag attributes (e.g., width, border)
  -x, --expanded           turn on expanded table output
  -z, --field-separator-zero
                           set field separator for unaligned output to zero byte
  -0, --record-separator-zero
                           set record separator for unaligned output to zero byte

Connection options:
  -h, --host=HOSTNAME      database server host or socket directory (default: "/var/run/postgresql")
  -p, --port=PORT          database server port (default: "5432")
  -U, --username=USERNAME  database user name (default: "postgres")
  -w, --no-password        never prompt for password
  -W, --password           force password prompt (should happen automatically)

For more information, type "\?" (for internal commands) or "\help" (for SQL
commands) from within psql, or consult the psql section in the PostgreSQL
documentation.
```
```
GNU bash, version 5.1.16(1)-release (x86_64-pc-linux-gnu)
These shell commands are defined internally.  Type `help' to see this list.
Type `help name' to find out more about the function `name'.
Use `info bash' to find out more about the shell in general.
Use `man -k' or `info' to find out more about commands not in this list.

A star (*) next to a name means that the command is disabled.

 job_spec [&]                                 history [-c] [-d offset] [n] or history ->
 (( expression ))                             if COMMANDS; then COMMANDS; [ elif COMMAN>
 . filename [arguments]                       jobs [-lnprs] [jobspec ...] or jobs -x co>
 :                                            kill [-s sigspec | -n signum | -sigspec] >
 [ arg... ]                                   let arg [arg ...]
 [[ expression ]]                             local [option] name[=value] ...
 alias [-p] [name[=value] ... ]               logout [n]
 bg [job_spec ...]                            mapfile [-d delim] [-n count] [-O origin]>
 bind [-lpsvPSVX] [-m keymap] [-f filename]>  popd [-n] [+N | -N]
 break [n]                                    printf [-v var] format [arguments]
 builtin [shell-builtin [arg ...]]            pushd [-n] [+N | -N | dir]
 caller [expr]                                pwd [-LP]
 case WORD in [PATTERN [| PATTERN]...) COMM>  read [-ers] [-a array] [-d delim] [-i tex>
 cd [-L|[-P [-e]] [-@]] [dir]                 readarray [-d delim] [-n count] [-O origi>
 command [-pVv] command [arg ...]             readonly [-aAf] [name[=value] ...] or rea>
 compgen [-abcdefgjksuv] [-o option] [-A ac>  return [n]
 complete [-abcdefgjksuv] [-pr] [-DEI] [-o >  select NAME [in WORDS ... ;] do COMMANDS;>
 compopt [-o|+o option] [-DEI] [name ...]     set [-abefhkmnptuvxBCHP] [-o option-name]>
 continue [n]                                 shift [n]
 coproc [NAME] command [redirections]         shopt [-pqsu] [-o] [optname ...]
 declare [-aAfFgiIlnrtux] [-p] [name[=value>  source filename [arguments]
 dirs [-clpv] [+N] [-N]                       suspend [-f]
 disown [-h] [-ar] [jobspec ... | pid ...]    test [expr]
 echo [-neE] [arg ...]                        time [-p] pipeline
 enable [-a] [-dnps] [-f filename] [name ..>  times
 eval [arg ...]                               trap [-lp] [[arg] signal_spec ...]
 exec [-cl] [-a name] [command [argument ..>  true
 exit [n]                                     type [-afptP] name [name ...]
 export [-fn] [name[=value] ...] or export >  typeset [-aAfFgiIlnrtux] [-p] name[=value>
 false                                        ulimit [-SHabcdefiklmnpqrstuvxPT] [limit]
 fc [-e ename] [-lnr] [first] [last] or fc >  umask [-p] [-S] [mode]
 fg [job_spec]                                unalias [-a] name [name ...]
 for NAME [in WORDS ... ] ; do COMMANDS; do>  unset [-f] [-v] [-n] [name ...]
 for (( exp1; exp2; exp3 )); do COMMANDS; d>  until COMMANDS; do COMMANDS; done
 function name { COMMANDS ; } or name () { >  variables - Names and meanings of some sh>
 getopts optstring name [arg ...]             wait [-fn] [-p var] [id ...]
 hash [-lr] [-p pathname] [-dt] [name ...]    while COMMANDS; do COMMANDS; done
 help [-dms] [pattern ...]                    { COMMANDS ; }
```
To access the PostgreSQL prompt:
```
psql
```
`\q` to exit and `\h` for query help:
```
Available help:
  ABORT                            CREATE USER MAPPING
  ALTER AGGREGATE                  CREATE VIEW
  ALTER COLLATION                  DEALLOCATE
  ALTER CONVERSION                 DECLARE
  ALTER DATABASE                   DELETE
  ALTER DEFAULT PRIVILEGES         DISCARD
  ALTER DOMAIN                     DO
  ALTER EVENT TRIGGER              DROP ACCESS METHOD
  ALTER EXTENSION                  DROP AGGREGATE
  ALTER FOREIGN DATA WRAPPER       DROP CAST
  ALTER FOREIGN TABLE              DROP COLLATION
  ALTER FUNCTION                   DROP CONVERSION
  ALTER GROUP                      DROP DATABASE
  ALTER INDEX                      DROP DOMAIN
  ALTER LANGUAGE                   DROP EVENT TRIGGER
  ALTER LARGE OBJECT               DROP EXTENSION
  ALTER MATERIALIZED VIEW          DROP FOREIGN DATA WRAPPER
  ALTER OPERATOR                   DROP FOREIGN TABLE
  ALTER OPERATOR CLASS             DROP FUNCTION
  ALTER OPERATOR FAMILY            DROP GROUP
  ALTER POLICY                     DROP INDEX
  ALTER PROCEDURE                  DROP LANGUAGE
  ALTER PUBLICATION                DROP MATERIALIZED VIEW
  ALTER ROLE                       DROP OPERATOR
  ALTER ROUTINE                    DROP OPERATOR CLASS
  ALTER RULE                       DROP OPERATOR FAMILY
  ALTER SCHEMA                     DROP OWNED
  ALTER SEQUENCE                   DROP POLICY
  ALTER SERVER                     DROP PROCEDURE
  ALTER STATISTICS                 DROP PUBLICATION
  ALTER SUBSCRIPTION               DROP ROLE
  ALTER SYSTEM                     DROP ROUTINE
  ALTER TABLE                      DROP RULE
  ALTER TABLESPACE                 DROP SCHEMA
  ALTER TEXT SEARCH CONFIGURATION  DROP SEQUENCE
  ALTER TEXT SEARCH DICTIONARY     DROP SERVER
  ALTER TEXT SEARCH PARSER         DROP STATISTICS
  ALTER TEXT SEARCH TEMPLATE       DROP SUBSCRIPTION
  ALTER TRIGGER                    DROP TABLE
  ALTER TYPE                       DROP TABLESPACE
  ALTER USER                       DROP TEXT SEARCH CONFIGURATION
  ALTER USER MAPPING               DROP TEXT SEARCH DICTIONARY
  ALTER VIEW                       DROP TEXT SEARCH PARSER
  ANALYZE                          DROP TEXT SEARCH TEMPLATE
  BEGIN                            DROP TRANSFORM
  CALL                             DROP TRIGGER
  CHECKPOINT                       DROP TYPE
  CLOSE                            DROP USER
  CLUSTER                          DROP USER MAPPING
  COMMENT                          DROP VIEW
  COMMIT                           END
  COMMIT PREPARED                  EXECUTE
  COPY                             EXPLAIN
  CREATE ACCESS METHOD             FETCH
  CREATE AGGREGATE                 GRANT
  CREATE CAST                      IMPORT FOREIGN SCHEMA
  CREATE COLLATION                 INSERT
  CREATE CONVERSION                LISTEN
  CREATE DATABASE                  LOAD
  CREATE DOMAIN                    LOCK
  CREATE EVENT TRIGGER             MERGE
  CREATE EXTENSION                 MOVE
  CREATE FOREIGN DATA WRAPPER      NOTIFY
  CREATE FOREIGN TABLE             PREPARE
  CREATE FUNCTION                  PREPARE TRANSACTION
  CREATE GROUP                     REASSIGN OWNED
  CREATE INDEX                     REFRESH MATERIALIZED VIEW
  CREATE LANGUAGE                  REINDEX
  CREATE MATERIALIZED VIEW         RELEASE SAVEPOINT
  CREATE OPERATOR                  RESET
  CREATE OPERATOR CLASS            REVOKE
  CREATE OPERATOR FAMILY           ROLLBACK
  CREATE POLICY                    ROLLBACK PREPARED
  CREATE PROCEDURE                 ROLLBACK TO SAVEPOINT
  CREATE PUBLICATION               SAVEPOINT
  CREATE ROLE                      SECURITY LABEL
  CREATE RULE                      SELECT
  CREATE SCHEMA                    SELECT INTO
  CREATE SEQUENCE                  SET
  CREATE SERVER                    SET CONSTRAINTS
  CREATE STATISTICS                SET ROLE
  CREATE SUBSCRIPTION              SET SESSION AUTHORIZATION
  CREATE TABLE                     SET TRANSACTION
  CREATE TABLE AS                  SHOW
  CREATE TABLESPACE                START TRANSACTION
  CREATE TEXT SEARCH CONFIGURATION TABLE
  CREATE TEXT SEARCH DICTIONARY    TRUNCATE
  CREATE TEXT SEARCH PARSER        UNLISTEN
  CREATE TEXT SEARCH TEMPLATE      UPDATE
  CREATE TRANSFORM                 VACUUM
  CREATE TRIGGER                   VALUES
  CREATE TYPE                      WITH
  CREATE USER                      
```

